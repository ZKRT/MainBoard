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
#include "stm32f4xx.h"
#include "Activate.h"
#include "djiCtrl.h"

extern Vehicle  vehicle;
extern Vehicle* v;

void
userActivate() {
  //! At your DJI developer account look for: app_key and app ID
  static char key_buf[65] = "84b4e585486fe21768f09930f79a68a2edfa6d80431d6426461d844e685b3902";//"ac5100a048cc4c4a08ce23200481d058d7455e69b8668fc0e49f29bd856c185a"; /*"your app_key"*/
  DJI::OSDK::Vehicle::ActivateData user_act_data;
  user_act_data.ID = 1065209;//1031967; /*your app ID here*/

  user_act_data.encKey = key_buf;

//  v->activate(&user_act_data);
  v->activate (&user_act_data);
}

void
Vehicle::activateCallbackv2(Vehicle* vehiclePtr, RecvContainer recvFrame,
                            UserData userData) {
  uint16_t ack_data;
  if (recvFrame.recvInfo.len - OpenProtocol::PackageMin <= 2) {
    ack_data = recvFrame.recvData.ack;

    vehiclePtr->ackErrorCode.data = ack_data;
    vehiclePtr->ackErrorCode.info = recvFrame.recvInfo;

    if (ACK::getError(vehiclePtr->ackErrorCode) &&
        ack_data ==
        OpenProtocolCMD::ErrorCode::ActivationACK::OSDK_VERSION_ERROR) {
      DERROR("SDK version did not match\n");
      vehiclePtr->getDroneVersion();
    }

    //! Let user know about other errors if any
    ACK::getErrorCodeMessage(vehiclePtr->ackErrorCode, __func__);
  } else {
    DERROR("ACK is exception, sequence %d\n", recvFrame.recvInfo.seqNumber);
  }

  if (ack_data == OpenProtocolCMD::ErrorCode::ActivationACK::SUCCESS &&
      vehiclePtr->accountData.encKey) {
    vehiclePtr->protocolLayer->setKey(vehiclePtr->accountData.encKey);
    djisdk_state.run_status = avtivated_ok_djirs; //add by yanly
  }
}
