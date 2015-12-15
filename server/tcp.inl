void process_tcp( )
{
	if( clients.size( ) > 0 )
	{
		// Check each socket
		for( auto client = clients.begin( ); client != clients.end( ); )
		{
			if( SDLNet_SocketReady( (*client)->GetTCPSocket( ) ) )
			{
				// Receive data directly into the universal packet
				if( SDLNet_TCP_Recv( (*client)->GetTCPSocket( ), uniPacket.Data( ), uniPacket.Size( ) ) > 0 )
				{
					std::unique_ptr<BasePacket> recvd = uniPacket.CreateFromContents( );

					// check if its a join request
					if( recvd )
					{
						// print to console whe the packe is recvd
						recvd->Print( );

						if( recvd->Type( ) == PT_INFO_REQUEST )
						{
							InfoRequestPacket* p = (InfoRequestPacket*)recvd.get( );

							if( p->GetRequested( ) == RT_MAP_DATA )
							{
								// Send the current map
								std::cout << "player asked for map data" << std::endl;
								MapDataPacket response;
								response.SetMapData( mapData );
								(*client)->TCPSend( response );
							}
							else if( p->GetRequested( ) == RT_PLAYER_LIST )
							{
								// Send the list of connected players
								for( size_t i = 0; i < clients.size( ); i++ )
								{
									PlayerJoinedPacket currentPlayer;
									currentPlayer.SetID( clients[i]->GetID( ) );
									currentPlayer.SetPosition( clients[i]->GetPosition( ) );

									// dont tell the new client about themselvs
									if( clients[i]->GetID( ) != p->GetID( ) )
									{
										get_client( p->GetID( ) )->TCPSend( currentPlayer );
									}
								}
							}
						}
						else if( recvd->Type( ) == PT_MOVE )
						{
							MovePacket* packet = (MovePacket*)recvd.get( );

							std::cout << "ID: " << packet->GetID( ) << " x: " << packet->GetPosition( ).x << " y: " << packet->GetPosition( ).y << std::endl;
						}
						else {
							std::cout << "TCP type not recognised" << std::endl;
						}
					}
					else {
						std::cout << "TCP packet not supported" << std::endl;
					}

					// move to the next client
					client++;
				}
				else
				{
					std::cout << "SDLNet_TCP_Recv: %s\n" << SDLNet_GetError( ) << std::endl;
					std::cout << "Could not communciate with player: " << (int)(*client)->GetID( ) << std::endl;
					PlayerDisconnectedPacket disconnect( (*client)->GetID( ) );
					client = clients.erase( client );
					tcp_send_all( disconnect );
				}
			}
			else
			{
				client++;
			}
		}
	}
}

void tcp_send_all( const BasePacket& packet )
{
	for( auto& client : clients )
	{
		client->TCPSend( packet );
	}
}

void tcp_send_all_except( Uint8 ID, const BasePacket& packet )
{
	for( auto& client : clients )
	{
		if( client->GetID( ) != ID )
		{
			client->TCPSend( packet );
		}
	}
}

void tcp_send_to( Uint8 ID, const BasePacket& packet )
{
	Client* client = get_client( ID );
	if( client ) client->TCPSend( packet );
}