// receives and dispatches udp packets
void process_udp();

// sends the packet the specified client
void udp_send_to( Uint8 ID, const BasePacket& packet );

// sends the packet to all connected clients
void udp_send_all( const BasePacket& packet );			

// sends the packet to all clients except the specified client
void udp_send_all_except( Uint8 ID, const BasePacket& packet );