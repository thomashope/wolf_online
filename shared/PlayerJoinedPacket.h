#ifndef PLAYERJOINED_PACKET_H
#define PLAYERJOINED_PACKET_H

#include "vec2.h"
#include "BasePacket.h"

#define PLAYERJOINED_PACKET_SIZE (1 + 1 + 8)

// Tells other clients

class PlayerJoinedPacket : public BasePacket
{
public:
  PlayerJoinedPacket() : BasePacket( PT_PLAYER_JOINED, PLAYERJOINED_PACKET_SIZE ) {}
  PlayerJoinedPacket( Uint8 ID ) : PlayerJoinedPacket() { SetID( ID ); }
  ~PlayerJoinedPacket() {}

  void SetID( Uint8 ID ) { data_[1] = ID; }
  void SetPosition( Vec2 position );

  Uint8 GetID() const { return data_[1]; }
  Vec2 GetPosition() const;

  void Print() const override {
	  std::cout << "Player ID: " << (int)GetID() << " joined the session" << std::endl;
  }
};

#endif
