// receives and dispatches tcp packets
void process_tcp();

// sends the packet the specified client
void tcp_send_to( Uint8 ID, const BasePacket& packet );

// sends the packet to all connected clients
void tcp_send_all( const BasePacket& packet );

// sends the packet to all clients except the specified client
void tcp_send_all_except( Uint8 ID, const BasePacket& packet );