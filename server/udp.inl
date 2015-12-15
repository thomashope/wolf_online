void process_udp( )
{
	// recv all pending udp packets
	while( SDLNet_UDP_Recv( UDP_socket, &UDP_packet ) )
	{
		auto recvd = uniPacket.CreateFromContents( );

		if( recvd )
		{
			if( recvd->Type( ) == PT_MOVE )
			{
				recvd->Print( );

				MovePacket* packet = (MovePacket*)recvd.get( );

				Client* client = get_client( packet->GetID( ) );
				if( client )
				{
					// update the servers copy of the client
					client->SetPosition( packet->GetPosition( ) );
					// tell all the other clients
					udp_send_all_except( packet->GetID( ), *packet );
				}
				else
				{
					std::cout << "packet destined for unknown client" << std::endl;
				}

			}
			else if( recvd->Type( ) == PT_HEARTBEAT )
			{
				HeartbeatPacket* packet = (HeartbeatPacket*)recvd.get( );

				recvd->Print( );

				// Set the clients UDP address
				Client* sender = get_client( packet->GetID( ) );
				if( sender )
				{
					sender->SetUDPAddress( UDP_packet.address );
				}

				// Send a sync packet to the client with the server ime
				SyncPacket sync;
				sync.SetMode( SYNC_RETURN );
				sync.SetID( packet->GetID( ) );
				sync.SetTime( SDL_GetTicks( ) );
				sender->UDPSend( sync );
			}
			else if( recvd->Type( ) == PT_SYNC )
			{
				SyncPacket* packet = (SyncPacket*)recvd.get( );

				// if the server receives a returned packet
				if( packet->GetMode( ) == SYNC_RETURN )
				{
					// calculate the send time
					Uint32 sendTime = (SDL_GetTicks( ) - packet->GetTime( )) / 2;
					SyncPacket sync;
					sync.SetMode( SYNC_SET );
					sync.SetID( packet->GetID( ) );
					Client* sender = get_client( packet->GetID( ) );
					sync.SetTime( SDL_GetTicks( ) + sendTime ); 		// the current time plus the time the packet will take to arrive
					if( sender ) {
						sender->UDPSend( sync );
					}
					else {
						std::cout << "An unknown client tried to sync" << std::endl;
					}
				}
			}
			else
			{
				recvd->Print( );
				std::cout << "UDP type not recognised" << std::endl;
			}
		}
		else
		{
			std::cout << "UDP packet not supported" << std::endl;
		}
	}
}

void udp_send_all_except( Uint8 ID, const BasePacket& packet )
{
	for( auto& client : clients )
	{
		if( client->GetID( ) != ID )
		{
			client->UDPSend( packet );
		}
	}
}

void udp_send_to( Uint8 ID, const BasePacket& packet )
{
	Client* client = get_client( ID );
	if( client ) client->UDPSend( packet );
}

void udp_send_all( const BasePacket& packet )
{
	for( auto& client : clients )
	{
		client->UDPSend( packet );
	}
}