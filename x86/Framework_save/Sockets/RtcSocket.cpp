/*
 * RtcSocket.cpp
 *
 *  Created on: 13.07.2019
 *      Author: harald
 */

#include <Sockets/RtcSocket.h>



bool operator ==(const RTC_TimeTypeDef left, const RTC_TimeTypeDef right) {
	if( (right.Seconds == left.Seconds)
			&& (right.Minutes == left.Minutes)
			&& (right.Hours   == left.Hours) 	)
	{
		return true;
	}

	return false;
}

RtcSocket::RtcSocket() {
	act_date.Date 	 = 1u;
	act_date.Month 	 = 1u;
	act_date.Year 	 = 00u;
	act_date.WeekDay = RTC_WEEKDAY_SATURDAY;

	act_time.Seconds	= 0u;
	act_time.Minutes	= 0u;
	act_time.Hours		= 0u;
}

void RtcSocket::set_time_date(RTC_TimeTypeDef sTime, RTC_DateTypeDef sDate) {
	set_date(sDate);
	set_time(sTime);
}

void RtcSocket::set_date(RTC_DateTypeDef sDate) {
	act_date = sDate;
}

void RtcSocket::set_time(RTC_TimeTypeDef sTime) {
	act_time = sTime;
}

void RtcSocket::get_time_date(RTC_TimeTypeDef* sTime, RTC_DateTypeDef* sDate) {
	*sTime = get_time();
	*sDate = get_date();
}

RTC_DateTypeDef RtcSocket::get_date(void) {
	return act_date;
}

RTC_TimeTypeDef RtcSocket::get_time(void) {
	return act_time;
}

uint32_t RtcSocket::get_time_minutes(void) {
	RTC_TimeTypeDef sTime = get_time();
	RTC_DateTypeDef sDate = get_date();

	return (sDate.WeekDay * 24 * 60) + (sTime.Hours * 60) + sTime.Minutes;
}

RTC_DateTypeDef RtcSocket::operator =(const	RTC_DateTypeDef right) {
	RTC_DateTypeDef result;
	result.Date    = right.Date;
	result.Month   = right.Month;
	result.Year    = right.Year;
	result.WeekDay = right.WeekDay;

	return result;
}

RTC_TimeTypeDef RtcSocket::operator =(const RTC_TimeTypeDef right) {
	RTC_TimeTypeDef result;
	result.Seconds = right.Seconds;
	result.Minutes = right.Minutes;
	result.Hours   = right.Hours;

	return result;
}



