#ifndef QTOANGLE_H
#define QTOANGLE_H

#include "dji_vehicle.hpp"

/*!
 * Top-level namespace
 */
namespace DJI
{
/*!
 * Onboard SDK related commands
 */
namespace OSDK
{
/*! @brief This namespace encapsulates all available telemetry topics through
 * either
 * Broadcast or Subscribe
 */
namespace Telemetry
{
//! @note This struct will replace EulerianAngle in a future release.
typedef struct EulerAngle
{
  double yaw;
  double roll;
  double pitch;
} EulerAngle;
}
}
}

using namespace DJI::OSDK::Telemetry;

double getYaw(Quaternion quaternionData);
double getRoll(Quaternion quaternionData);
double getPitch(Quaternion quaternionData);
double radiansToDegrees(double rad);

#endif // TELEMETRYSAMPLE_H
