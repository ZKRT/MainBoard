#include "stm32f4xx.h"
#include "MobileSample.h"
#include "zkrt.h"

using namespace DJI;
using namespace DJI::OSDK;

extern Vehicle  vehicle;
extern Vehicle* v;

////////////////////////////////////////////////////////////////////////////////////zkrt use
#include "mobileDataHandle.h"
#include "sys.h"
#include "led.h"

void
parseFromMobileCallback_v2(DJI::OSDK::Vehicle*      vehicle,
                        DJI::OSDK::RecvContainer recvFrame,
                        DJI::OSDK::UserData      userData)
{
	msg_handle_st *msgData = (msg_handle_st*) userData;
	uint8_t *data = msgData->data_recv_app;
	uint8_t *s_data = recvFrame.recvData.raw_ack_array;
	uint16_t datalen = recvFrame.recvInfo.len - Protocol::PackageMin - 2; // minus cmd id and cmd set size //zkrt_test  
	
	if((datalen >= ZK_FIXED_LEN)&&(datalen <= ZK_MAX_LEN))
	{
		memcpy(data, s_data, datalen);
		msgData->datalen_recvapp = datalen;
	}
}

extern "C" void sendToMobile(uint8_t *data, uint8_t len)
{
  v->moc->sendDataToMSDK(data, len);
	_FLIGHT_UART_TX_LED = 0;
  usart1_tx_flag = TimingDelay;
}
////////////////////////////////////////////////////////////////////////////////////
void
parseFromMobileCallback(DJI::OSDK::Vehicle*      vehicle,
                        DJI::OSDK::RecvContainer recvFrame,
                        DJI::OSDK::UserData      userData)
{

  uint16_t mobile_data_id;
  mobile_data_id =
    *(reinterpret_cast<uint16_t*>(&recvFrame.recvData.raw_ack_array));

  switch (mobile_data_id)
  {
    case 2:
      v->obtainCtrlAuthority(controlAuthorityMobileCallback);
      break;
    case 3:
      v->releaseCtrlAuthority(controlAuthorityMobileCallback);
      break;
    case 5:
      if(v->getFwVersion() != Version::M100_31)
      {
	v->control->action(Control::FlightCommand::startMotor,
        actionMobileCallback);
      }
      else
      {
        v->control->armMotors(actionMobileCallback);
      }
      break;
    case 6:
      if(v->getFwVersion() != Version::M100_31)
      {
        v->control->action(Control::FlightCommand::stopMotor,
        actionMobileCallback);
      }
      else
      {
	v->control->disArmMotors(actionMobileCallback);
      }
      break;
    default:
      break;
  }
}

void
controlAuthorityMobileCallback(Vehicle* vehiclePtr, RecvContainer recvFrame,
                               DJI::OSDK::UserData userData)
{
  ACK::ErrorCode ack;
  ack.data = OpenProtocol::ErrorCode::CommonACK::NO_RESPONSE_ERROR;

  unsigned char data    = 0x1;
  int           cbIndex = vehiclePtr->callbackIdIndex();

  if (recvFrame.recvInfo.len - Protocol::PackageMin <= sizeof(uint16_t))
  {
    ack.data = recvFrame.recvData.ack;
    ack.info = recvFrame.recvInfo;
  }
  else
  {
    DERROR("ACK is exception, sequence %d\n", recvFrame.recvInfo.seqNumber);
  }

  if (ack.data == OpenProtocol::ErrorCode::ControlACK::SetControl::
                    OBTAIN_CONTROL_IN_PROGRESS)
  {
    vehiclePtr->obtainCtrlAuthority(controlAuthorityMobileCallback);
  }
  else if (ack.data == OpenProtocol::ErrorCode::ControlACK::SetControl::
                         RELEASE_CONTROL_IN_PROGRESS)
  {
    vehiclePtr->releaseCtrlAuthority(controlAuthorityMobileCallback);
  }
  else
  {
    // We have a success case.
    // Send this data to mobile
    AckReturnToMobile mobileAck;
    // Find out which was called: obtain or release
    if (recvFrame.recvInfo.buf[2] == ACK::OBTAIN_CONTROL)
    {
      mobileAck.cmdID = 0x02;
    }
    else if (recvFrame.recvInfo.buf[2] == ACK::RELEASE_CONTROL)
    {
      mobileAck.cmdID = 0x03;
    }
    mobileAck.ack = static_cast<uint16_t>(ack.data);
    vehiclePtr->moc->sendDataToMSDK(reinterpret_cast<uint8_t*>(&mobileAck),
                                    sizeof(mobileAck));
  }
}

void
actionMobileCallback(Vehicle* vehiclePtr, RecvContainer recvFrame,
                     DJI::OSDK::UserData userData)
{
  ACK::ErrorCode ack;

  if (recvFrame.recvInfo.len - Protocol::PackageMin <= sizeof(uint16_t))
  {

    ack.info = recvFrame.recvInfo;

    if (vehiclePtr->getFwVersion() != Version::M100_31)
      ack.data = recvFrame.recvData.commandACK;
    else
      ack.data = recvFrame.recvData.ack;

    if (ACK::getError(ack))
      ACK::getErrorCodeMessage(ack, __func__);

    AckReturnToMobile mobileAck;
    const uint8_t     cmd[] = { recvFrame.recvInfo.cmd_set,
                            recvFrame.recvInfo.cmd_id };

    // startMotor supported in FW version >= 3.3
    // setArm supported only on Matrice 100
    if (recvFrame.recvInfo.buf[2] == Control::FlightCommand::startMotor ||
        (memcmp(cmd, OpenProtocol::CMDSet::Control::setArm, sizeof(cmd)) &&
         recvFrame.recvInfo.buf[2] == true))
    {
      mobileAck.cmdID = 0x05;
      mobileAck.ack   = static_cast<uint16_t>(ack.data);
      vehiclePtr->moc->sendDataToMSDK(reinterpret_cast<uint8_t*>(&mobileAck),
                                      sizeof(mobileAck));
    }
    else if (recvFrame.recvInfo.buf[2] == Control::FlightCommand::stopMotor ||
             (memcmp(cmd, OpenProtocol::CMDSet::Control::setArm, sizeof(cmd)) &&
              recvFrame.recvInfo.buf[2] == false))
    {
			mobileAck.cmdID = 0x06;
      mobileAck.ack   = static_cast<uint16_t>(ack.data);
      vehiclePtr->moc->sendDataToMSDK(reinterpret_cast<uint8_t*>(&mobileAck),
                                      sizeof(mobileAck));
    }
  }
  else
  {
    DERROR("ACK is exception, sequence %d\n", recvFrame.recvInfo.seqNumber);
  }
}
