/*! @file driver.cpp
 *  @version 3.1.8
 *  @date Aug 05 2016
 *
 *  @brief
 *  Implementation of HardDriver for the STM32F4Discovery board.
 *
 *  Copyright 2016 DJI. All right reserved.
 *
 * */

#include <driver.h>

size_t STM32F4::send(const uint8_t* buf, size_t len)
{
  char* p = (char*)buf;

  if (NULL == buf)
  {
    return 0;
  }

  while (len--)
  {
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
      ;
    USART_SendData(USART1, *p++);
  }
  return 1;
}

DJI::time_ms STM32F4::getTimeStamp()
{
  return tick; //目前代码里的tick定时没有触发，这会导致DJI SDK里用到定时的地方用不了，查看代码发现可能只有session逻辑会受影响。//by yanly
}

