/*! @file telemetry_sample.cpp
 *  @version 3.3
 *  @date Jun 05 2017
 *
 *  @brief
 *  Telemetry API usage in a Linux environment.
 *  Shows example usage of the new data subscription API.
 *
 *  @copyright
 *  2017 DJI. All rights reserved.
 * */

#include "BoardcastSample.h"
#include <math.h>

extern Vehicle  vehicle;
extern Vehicle* v;

using namespace DJI::OSDK;
using namespace DJI::OSDK::Telemetry;

double getYaw(Quaternion quaternionData);
double getRoll(Quaternion quaternionData);
double getPitch(Quaternion quaternionData);
double radiansToDegrees(double rad);

bool
boardcastSample(void)
{

	v->broadcast->setBroadcastFreqDefaults();
	
	uint32_t PRINT_TIMEOUT = 300000; // milliseconds
  uint32_t RETRY_TICK    = 500;   // milliseconds
  uint32_t nextRetryTick = 0;     // millisesonds
  uint32_t timeoutTick;
	double pitch;
	
  timeoutTick = v->protocolLayer->getDriver()->getTimeStamp() + PRINT_TIMEOUT;
  do
	{
    printf("-------\n");
		printf("RC Commands (r/p/y/thr) = %d, %d, %d, %d\n",
		  v->broadcast->getRC().roll, v->broadcast->getRC().pitch, v->broadcast->getRC().yaw, v->broadcast->getRC().throttle);
//		printf("height = %f\n", v->broadcast->getGlobalPosition().height);
//    printf("Attitude Quaternion (w,x,y,z) = %.3f, %.3f, %.3f, %.3f\n", 
//		  v->broadcast->getQuaternion().q0, v->broadcast->getQuaternion().q1, v->broadcast->getQuaternion().q2, v->broadcast->getQuaternion().q3);	
    printf("Velocity (vx,vy,vz) = %.3f, %.3f, %.3f\n",
		  v->broadcast->getVelocity().x, v->broadcast->getVelocity().y, v->broadcast->getVelocity().z);		
		pitch = getPitch(v->broadcast->getQuaternion());
		printf("vehicle pitch= %f, degree= %f\n",
		  pitch, radiansToDegrees(pitch));
//		printf("controlDevice(cM/dS/fS/vS) = %d, %d, %d, %d\n",
//		  v->broadcast->getSDKInfo().controlMode, v->broadcast->getSDKInfo().deviceStatus, v->broadcast->getSDKInfo().flightStatus, v->broadcast->getSDKInfo().vrcStatus);
//		printf("Flight Status = %d\n", v->broadcast->getStatus().flight);
    printf("-------\n\n");
    delay_nms(500);
    nextRetryTick = v->protocolLayer->getDriver()->getTimeStamp() + RETRY_TICK;
  } while (nextRetryTick < timeoutTick);	
	
  return true;
}
void printf_boardcast_data(void)
{
	double pitch;
	printf("Velocity (vx,vy,vz) = %.3f, %.3f, %.3f\n",
		v->broadcast->getVelocity().x, v->broadcast->getVelocity().y, v->broadcast->getVelocity().z);		
	pitch = getPitch(v->broadcast->getQuaternion());
	printf("vehicle pitch= %f, degree= %f\n",
		pitch, radiansToDegrees(pitch));	
}
//! @note This struct will replace EulerianAngle in a future release.
typedef struct EulerAngle
{
  double yaw;
  double roll;
  double pitch;
} EulerAngle;

EulerAngle toEulerAngle(Quaternion quaternionData)
{
  EulerAngle ans;

  double q2sqr = quaternionData.q2 * quaternionData.q2;
  double t0 = -2.0 * (q2sqr + quaternionData.q3 * quaternionData.q3) + 1.0;
  double t1 = +2.0 * (quaternionData.q1 * quaternionData.q2 + quaternionData.q0 * quaternionData.q3);
  double t2 = -2.0 * (quaternionData.q1 * quaternionData.q3 - quaternionData.q0 * quaternionData.q2);
  double t3 = +2.0 * (quaternionData.q2 * quaternionData.q3 + quaternionData.q0 * quaternionData.q1);
  double t4 = -2.0 * (quaternionData.q1 * quaternionData.q1 + q2sqr) + 1.0;

  t2 = t2 > 1.0 ? 1.0 : t2;
  t2 = t2 < -1.0 ? -1.0 : t2;

  ans.pitch = asin(t2);
  ans.roll = atan2(t3, t4);
  ans.yaw = atan2(t1, t0);

  return ans;
}
double getYaw(Quaternion quaternionData)
{
#ifdef USE_SIMULATION
  if (simulating)
    return AngularSim.yaw;
  else
#endif // USE_SIMULATION
  return toEulerAngle(quaternionData).yaw;
}

double getRoll(Quaternion quaternionData)
{
#ifdef USE_SIMULATION
  if (simulating)
    return AngularSim.roll;
  else
#endif // USE_SIMULATION
  return toEulerAngle(quaternionData).roll;
}

double getPitch(Quaternion quaternionData)
{
#ifdef USE_SIMULATION
  if (simulating)
    return AngularSim.pitch;
  else
#endif // USE_SIMULATION
  return toEulerAngle(quaternionData).pitch;
}
double radiansToDegrees(double rad)
{
	double degrees = rad * (180.0/3.14159);
	return degrees;
}
