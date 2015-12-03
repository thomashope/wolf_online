#ifndef PLAYERJOINED_PACKET_H
#define PLAYERJOINED_PACKET_H

#include "BasePacket.h"

#define PLAYERJOINED_PACKET_SIZE (1 + 1)

// Tells other clients

class PlayerJoinedPacket : public BasePacket
{
public:
  PlayerJoinedPacket() : BasePacket( PT_PLAYER_JOINED, PLAYERJOINED_PACKET_SIZE ) {}
  PlayerJoinedPacket( Uint8 ID ) : PlayerJoinedPacket() { SetID( ID ); }
  ~PlayerJoinedPacket() {}

  void SetID( Uint8 ID );

  Uint8 GetID() const;

  void Print() const override;
};

#endif
