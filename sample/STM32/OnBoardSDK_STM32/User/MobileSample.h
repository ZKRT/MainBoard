#ifndef MOBILESAMPLE_H
#define MOBILESAMPLE_H

#include <cstdint>
#include <dji_vehicle.hpp>

void controlAuthorityMobileCallback(DJI::OSDK::Vehicle*      vehiclePtr,
                                    DJI::OSDK::RecvContainer recvFrame,
                                    DJI::OSDK::UserData      userData);
void actionMobileCallback(DJI::OSDK::Vehicle*      vehiclePtr,
                          DJI::OSDK::RecvContainer recvFrame,
                          DJI::OSDK::UserData      userData);

void parseFromMobileCallback(DJI::OSDK::Vehicle*      vehicle,
                             DJI::OSDK::RecvContainer recvFrame,
                             DJI::OSDK::UserData      userData);

void
parseFromMobileCallback_v2(DJI::OSDK::Vehicle*      vehicle,
                        DJI::OSDK::RecvContainer recvFrame,
                        DJI::OSDK::UserData      userData);

void djiBroadcastCallback(DJI::OSDK::Vehicle*      vehicle,
                        DJI::OSDK::RecvContainer recvFrame,
                        DJI::OSDK::UserData      userData);
bool setupMSDKParsing();

typedef struct AckReturnToMobile
{
  uint16_t cmdID;
  uint16_t ack;
} AckReturnToMobile;

//zkrt
typedef struct
{

  float roll;           /*!< degree */
  float pitch;          /*!< degree */
  float yaw;            /*!< degree */
  uint8_t   pitchLimit : 1; /*!< 1 - axis reached limit, 0 - no */
  uint8_t   rollLimit : 1;  /*!< 1 - axis reached limit, 0 - no */
  uint8_t   yawLimit : 1;   /*!< 1 - axis reached limit, 0 - no */
  uint8_t   reserved : 5;
} Gimbalst;	
typedef struct
{
	Gimbalst gimbal;
}use_broad_data_st;
extern use_broad_data_st dji_broaddata;
#endif // MOBILESAMPLE_H
