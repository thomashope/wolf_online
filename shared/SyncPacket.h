#ifndef SYNC_PACKET_H
#define SYNC_PACKET_H

#include "BasePacket.h"

#define SYNC_PACKET_SIZE (1 + 1 + 1 + 4)

enum SyncMode : Uint8 {
  SYNC_RETURN = 0,        // Send the packet back immidiately
  SYNC_SET = 1            // Set the local time to the packets time
};

class SyncPacket : public BasePacket
{
public:
    SyncPacket() : BasePacket( PT_SYNC, SYNC_PACKET_SIZE ) {}
    ~SyncPacket() {}

    void SetMode( SyncMode sm ) { data_[1] = sm;                  }
    void SetID( Uint8 ID )      { data_[2] = ID;                  }
    void SetTime( Uint32 t)     { SDLNet_Write32( t, &data_[3] ); }

    SyncMode  GetMode()   { return (SyncMode)data_[1];         }
    Uint8     GetID()     { return (Uint8)data_[2];            }
    Uint32    GetTime()   { return SDLNet_Read32( &data_[3] ); }
};

#endif
