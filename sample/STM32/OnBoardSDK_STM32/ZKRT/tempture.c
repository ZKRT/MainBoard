#include "ds18b20.h"
#include "adc.h"
#include "dev_handle.h"
#include "appprotocol.h"
#include "tempture.h"

static u8 temperature_value_check(short *value);

/**
  * @brief  read temperture in adc or ds18b20 or sensor board by #define
  * @param  None
  * @retval None 1-value readed,  0-value not readed
  */
char read_temperature(void)
{
	short *t1 = &zkrt_devinfo.temperature1;
	short *t2 = &zkrt_devinfo.temperature2;
#ifdef _TEMPTURE_ADC_
	if (adc_start_count - TimingDelay >= 160)
	{
		adc_start_count = TimingDelay;
		ADC_SoftwareStartConv(ADC1); 		/*启动ADC*/
	}
#endif

	if (temperature_read_count - TimingDelay < 320)
		return 0;

	temperature_read_count = TimingDelay;

#ifdef _TEMPTURE_IO_
	*t1 = DS18B20_Get_Temp(DS18B20_NUM1);
	*t2 = DS18B20_Get_Temp(DS18B20_NUM2);
	if ((*t0 == 0XFFFF) || (*t1 == 0XFFFF))
		_ALARM_LED = 0;
	else
		_ALARM_LED = 1;
#endif

#ifdef _TEMPTURE_ADC_
	*t1 = ADC1_get_value(_T1_value) - TEMPTURE_CALIBRATE;  //zkrt_notice: 温度补偿处理，由于AD采集温度受芯片内部温度的影响，实际运行一段时间后，芯片内部温度上升，导致AD采集温度过高，故减10度补偿。
	*t2 = ADC1_get_value(_T2_value) - TEMPTURE_CALIBRATE;
#endif

#ifdef USE_SESORINTEGRATED
	//because temperature read in sersorIntegratedHandle, so there not need read

#endif

	//check value is ok
	zkrt_devinfo.status_t1 = temperature_value_check(t1);
	zkrt_devinfo.status_t2 = temperature_value_check(t2);

	ZKRT_LOG(LOG_NOTICE, "[read temperature] t0:%d, t1:%d!\r\n", *t1, *t2);
	return 1;
}
/**
  * @brief  check the temperature value, see if it's in the normal range
  * @param  None
  * @retval None 温度值的有效状态
  */
static u8 temperature_value_check(short *value)
{
	u8 status = TEMP_NOMAL;
	short tv = *value;
	short tempture_low = zkrt_devinfo.temperature_low;
	short tempture_high = zkrt_devinfo.temperature_high;

	if (tv <= TEMPTURE_LOW_EXTRA)
	{
		status = TEMP_INVALID;
	}
	else if (tv >= TEMPTURE_HIGH_EXTRA)
	{
		status = TEMP_INVALID;
	}
	else if (tv < tempture_low)
	{
		status = TEMP_OVER_LOW;
	}
	else if (tv > tempture_high)
	{
		status = TEMP_OVER_HIGH;
	}
	else
		status = TEMP_NOMAL;

	return status;
}
