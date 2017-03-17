/*! @file Activate.cpp
 *  @version 3.1.8
 *  @date Aug 05 2016
 *
 *  @brief
 *  Activation process for the STM32 example App.
 *
 *  Copyright 2016 DJI. All right reserved.
 *
 * */

#include "Activate.h"
#include "sys.h"
#include "djiCtrl.h"

extern CoreAPI defaultAPI;
extern CoreAPI *coreApi;
extern Flight flight;

void zkrt_activateCallback(DJI::onboardSDK::CoreAPI *myapi, Header *protocolHeader, DJI::UserData userData __UNUSED);

void
User_Activate ()
{
	ZKRT_LOG(LOG_INOTICE,"@@@@@@@@User_Activate ()!@@@@@@@@@\n");
	static char key_buf[65] = "ac5100a048cc4c4a08ce23200481d058d7455e69b8668fc0e49f29bd856c185a"; /*"your app_key"*/

	ActivateData user_act_data;
	user_act_data.ID = 1031967; /*need your key in number like: 111000*/


  //! Change the version string to your platform/version as defined in DJI_Version.h
  //! Starting from OSDK 3.2.1, you do not need to do this. Instead, call droneVersion before activation.
//  user_act_data.version = SDK_VERSION;  //zkrt_modify: 不再需要调用

  user_act_data.encKey = key_buf;

//	coreApi->activate (&user_act_data); 
  coreApi->activate (&user_act_data, zkrt_activateCallback); //add by yanly

}
/**
  * @brief  zkrt_activateCallback.  飞控激活指令回调，激活的返回信息解析
  * @param
  * @param
  * @param
  * @retval None
  */
void zkrt_activateCallback(DJI::onboardSDK::CoreAPI *myapi, Header *protocolHeader, DJI::UserData userData __UNUSED)
{
  unsigned short ack_data;
  if (protocolHeader->length - EXC_DATA_SIZE <= 2)
  {
    memcpy((unsigned char *)&ack_data, ((unsigned char *)protocolHeader) + sizeof(Header),
        (protocolHeader->length - EXC_DATA_SIZE));
    
    // Write activation status to the broadcast data
    myapi->setBroadcastActivation(ack_data);

    switch (ack_data)
    {
      case ACK_ACTIVE_SUCCESS:
//				API_LOG(myapi->serialDevice, STATUS_LOG, "Activated successfully\n");
				ZKRT_LOG(LOG_INOTICE, "Activated successfully\n");
				djisdk_state.run_status = avtivated_ok_djirs; //add by yanly
				if (myapi->getAccountData().encKey)
          myapi->setKey(myapi->getAccountData().encKey);
        return;
      case ACK_ACTIVE_NEW_DEVICE:
        API_LOG(myapi->serialDevice, STATUS_LOG, "New device, please link DJIGO to your "
            "remote controller and try again\n");
        break;
      case ACK_ACTIVE_PARAMETER_ERROR:
        API_LOG(myapi->serialDevice, ERROR_LOG, "Wrong parameter\n");
        break;
      case ACK_ACTIVE_ENCODE_ERROR:
        API_LOG(myapi->serialDevice, ERROR_LOG, "Encode error\n");
        break;
      case ACK_ACTIVE_APP_NOT_CONNECTED:
        API_LOG(myapi->serialDevice, ERROR_LOG, "DJIGO not connected\n");
        break;
      case ACK_ACTIVE_NO_INTERNET:
        API_LOG(myapi->serialDevice, ERROR_LOG, "DJIGO not "
            "connected to the internet\n");
        break;
      case ACK_ACTIVE_SERVER_REFUSED:
        API_LOG(myapi->serialDevice, ERROR_LOG, "DJI server rejected "
            "your request, please use your SDK ID\n");
        break;
      case ACK_ACTIVE_ACCESS_LEVEL_ERROR:
        API_LOG(myapi->serialDevice, ERROR_LOG, "Wrong SDK permission\n");
        break;
      case ACK_ACTIVE_VERSION_ERROR:
        API_LOG(myapi->serialDevice, ERROR_LOG, "SDK version did not match\n");
        break;
      default:
        if (!myapi->decodeACKStatus(ack_data))
        {
          API_LOG(myapi->serialDevice, ERROR_LOG, "While calling this function");
        }
        break;
    }
  }
  else
  {
    API_LOG(myapi->serialDevice, ERROR_LOG, "ACK is exception, session id %d,sequence %d\n",
        protocolHeader->sessionID, protocolHeader->sequenceNumber);
  }
}
//void zkrt_setFrequencyCallback(DJI::onboardSDK::CoreAPI *api __UNUSED, Header *protocolHeader, DJI::UserData userData __UNUSED)
//{
//  unsigned short ack_data = ACK_COMMON_NO_RESPONSE;

//  if (protocolHeader->length - EXC_DATA_SIZE <= 2)
//  {
//    memcpy((unsigned char *)&ack_data, ((unsigned char *)protocolHeader) + sizeof(Header),
//        (protocolHeader->length - EXC_DATA_SIZE));
//  }
//  switch (ack_data)
//  {
//    case 0x0000:
//      API_LOG(api->serialDevice, STATUS_LOG, "Frequency set successfully\n");
//			djisdk_state.run_status = set_broadcastFreq_ok; //add by yanly
//      break;
//    case 0x0001:
//      API_LOG(api->serialDevice, ERROR_LOG, "Frequency parameter error\n");
//      break;
//    default:
//      if (!api->decodeACKStatus(ack_data))
//      {
//        API_LOG(api->serialDevice, ERROR_LOG, "While calling this function\n");
//      }
//      break;
//  }
//}
