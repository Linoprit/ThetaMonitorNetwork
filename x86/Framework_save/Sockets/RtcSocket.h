/*
 * RtcSocket.h
 *
 *  Created on: 13.07.2019
 *      Author: harald
 */

#ifndef SOCKETS_RTCSOCKET_H_
#define SOCKETS_RTCSOCKET_H_

#include <stdint.h>

// STM32 interfacing
/* Coded in BCD format */
#define RTC_MONTH_JANUARY              ((uint8_t)0x01)
#define RTC_MONTH_FEBRUARY             ((uint8_t)0x02)
#define RTC_MONTH_MARCH                ((uint8_t)0x03)
#define RTC_MONTH_APRIL                ((uint8_t)0x04)
#define RTC_MONTH_MAY                  ((uint8_t)0x05)
#define RTC_MONTH_JUNE                 ((uint8_t)0x06)
#define RTC_MONTH_JULY                 ((uint8_t)0x07)
#define RTC_MONTH_AUGUST               ((uint8_t)0x08)
#define RTC_MONTH_SEPTEMBER            ((uint8_t)0x09)
#define RTC_MONTH_OCTOBER              ((uint8_t)0x10)
#define RTC_MONTH_NOVEMBER             ((uint8_t)0x11)
#define RTC_MONTH_DECEMBER             ((uint8_t)0x12)

#define RTC_WEEKDAY_MONDAY             ((uint8_t)0x01)
#define RTC_WEEKDAY_TUESDAY            ((uint8_t)0x02)
#define RTC_WEEKDAY_WEDNESDAY          ((uint8_t)0x03)
#define RTC_WEEKDAY_THURSDAY           ((uint8_t)0x04)
#define RTC_WEEKDAY_FRIDAY             ((uint8_t)0x05)
#define RTC_WEEKDAY_SATURDAY           ((uint8_t)0x06)
#define RTC_WEEKDAY_SUNDAY             ((uint8_t)0x00)

typedef struct
{
	uint8_t WeekDay;  /*!< Specifies the RTC Date WeekDay (not necessary for HAL_RTC_SetDate).
                         This parameter can be a value of @ref RTC_WeekDay_Definitions */


	uint8_t Month;    /*!< Specifies the RTC Date Month (in BCD format).
                         This parameter can be a value of @ref RTC_Month_Date_Definitions */

	uint8_t Date;     /*!< Specifies the RTC Date.
                         This parameter must be a number between Min_Data = 1 and Max_Data = 31 */

	uint8_t Year;     /*!< Specifies the RTC Date Year.
                         This parameter must be a number between Min_Data = 0 and Max_Data = 99 */

} RTC_DateTypeDef;

typedef struct
{
	uint8_t Hours;            /*!< Specifies the RTC Time Hour.
                                 This parameter must be a number between Min_Data = 0 and Max_Data = 23 */

	uint8_t Minutes;          /*!< Specifies the RTC Time Minutes.
                                 This parameter must be a number between Min_Data = 0 and Max_Data = 59 */

	uint8_t Seconds;          /*!< Specifies the RTC Time Seconds.
                                 This parameter must be a number between Min_Data = 0 and Max_Data = 59 */

} RTC_TimeTypeDef;

/**
 * Remember: the socket implementations differs from target to target.
 * The interface to the application should be the same.
 *
 * This RtcSocket has nothing to do with a working clock. It provides
 * some get and set functions to enable testing.
 */

// must be defined outside the class
bool operator ==(const RTC_TimeTypeDef right, const RTC_TimeTypeDef left);

class RtcSocket {
public:
	RtcSocket();
	virtual ~RtcSocket() { };

	void set_time_date(RTC_TimeTypeDef sTime, RTC_DateTypeDef sDate);
	void set_date(RTC_DateTypeDef sDate);
	void set_time(RTC_TimeTypeDef sTime);

	void get_time_date(RTC_TimeTypeDef *sTime, RTC_DateTypeDef *sDate);
	RTC_DateTypeDef   get_date(void);
	RTC_TimeTypeDef   get_time(void);

	uint32_t get_time_minutes(void); // since sunday, 0:00;

	RTC_DateTypeDef operator = (const RTC_DateTypeDef right);
	RTC_TimeTypeDef operator = (const RTC_TimeTypeDef right);


private:
	RTC_DateTypeDef act_date;
	RTC_TimeTypeDef act_time;
};

#endif /* SOCKETS_RTCSOCKET_H_ */

