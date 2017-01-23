/**
  ******************************************************************************
  * @file    test.c 
  * @author  ZKRT
  * @version V0.0.1
  * @date    13-December-2016
  * @brief   test program body
  *          + (1) 
  *          + (2)  --161213 by yanly
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include "commonzkrt.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Hex2String program.
  * @param  pbDest
  * @param  pbSrc
  * @param  nLen	
  * @retval None
  */
void Hex2String(unsigned char *pbDest,unsigned char *pbSrc, int nLen)
{
	char ddl,ddh;
	int i;
	for (i=0; i<nLen; i++)
	{
		ddh = 48 + pbSrc[i] / 16;
		ddl = 48 + pbSrc[i] % 16;
		if (ddh > 57) ddh = ddh + 7;
		if (ddl > 57) ddl = ddl + 7;
		pbDest[i*2] = ddh;
		pbDest[i*2+1] = ddl;
	}
	pbDest[nLen*2] = '\0';
}
/**
  * @brief  String2Hex program.
  * @param  inString
  * @param  outData
  * @retval None
  */
void String2Hex(char *inString,char *outData)
{
	int		len, n = 0;
	char	tmp[2] = {0, 0};
	char	*po = outData;
	char	*pi = inString;

	len = strlen(inString);

	if (len == 0) return;

	while(1)
	{
		memcpy(tmp,pi,1);

		if (n % 2 == 0)
		{
			*po = ((char)strtoul(tmp, NULL, 16)) << 4;
		}
		else
		{
			*po |= ((char)strtoul(tmp, NULL, 16)) & 0xf;
			po++;
		}

		pi++;
		len--;
		n++;
		if (len == 0) break;
	}

	return;
}
/***
 * @param pStart: Head pointer of Data Body
 * @param len: Length of Data Body
 * @return Check code value
 */
unsigned short SerCalcCheckCode(unsigned char* pStart, unsigned short len)
{
	unsigned short result = 0;
#ifdef USE_CHECK_CODE
	unsigned short i;
	if (len == 0x00 ) {
		result = 0x0;
	}
	else if (len == 0x01) {
		result ^= (*pStart << 8) | 0x00;
	}
	else
	{
	/* Calculation */
		for (i = 0; i < (len/2); i++)
		{
			result ^= (*(pStart + i*2) << 8) | *(pStart + i*2 + 1);
		}
	/* If the "len" is odd, a calculation is performed with "0x00" interpolated */
	/* into the last byte. */
		if ( len % 2)
		{
			result ^= (*(pStart + len - 1) << 8) | 0x00;
		}
	}
	/* The calculation result is converted. */
	result = ((result & 0xff00) >> 8) | ((result & 0x00ff) << 8);
#endif
	return result;
}
/***
 * @brief 判断数组是否全为0
 * @param carry
 * @param len: Length of carry
 * @return 0-yes, 1-no
 */
unsigned char check_array_is_all_0(void* carry, unsigned short len)
{
	unsigned char result = 0;
	unsigned char *_carry = (unsigned char *)carry;
	unsigned short i;
	for(i=0; i<len; i++)
	{
		if(_carry[i] !=0)
		{
			result = 1;
			break;
		}
	}
	return result;
}
/**
  * @brief  判断s2是否是s1的子串，如果是返回在s1出现的s2的首地址，如果不是返回NULL
  * @param  s1
  * @param  s2
  * @retval char* 
  */
char* zkrt_strstr(const char*s1, const char*s2)
{
	int n;
	if(*s2 != NULL)
	{
		while(*s1)
		{
			for(n=0;*(s1+n)==*(s2+n);n++)
			{
				if(!*(s2+n+1))
				return (char*)s1;
			}
			s1++;
		}
		return NULL;
	}
	else
	{
		return(char*)s1; 
	}
}
/**
  * @brief  generate the rand buffer of specified length
  * @param  len
  * @param  buffer
  * @retval None
  */
void generate_randcode(int len,char* buffer)
{
	static const char string[]= "0123456789abcdefghiljklnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int i = 0;
//	srand(get_running_time());
//	srand(get_running_time_ms()); //以时间为种子
	for(; i < len; i++)
	{
		buffer[i] = string[rand()%strlen(string)];
	}
}
/**
  * @brief  printf_uchar
  * @param  data
  * @param  length
  * @retval None
  */
void printf_uchar(unsigned char *data, u16 length)
{
#ifdef LOG_PRINTF_UCHAR	
	int i;
	for(i=0; i < length; i++)
	{
		printf("0x%x ", data[i]);
	}
	printf("\r\n");
#endif	
}
/**
  * @}
  */ 

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/
