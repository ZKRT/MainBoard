#include "QtoAngle.h"
#include <math.h>

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
