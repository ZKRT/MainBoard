/*! @file MobileSample.h
 *  @version 3.3
 *  @date May 2017
 *
 *  @brief
 *  MSDK Communication STM32 example.
 *
 *  @Copyright (c) 2016-2017 DJI
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

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
