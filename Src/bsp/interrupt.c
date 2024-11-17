#include "interrupt.h"

int count;
int angle;
extern int GZ_JZ;
extern int16_t AX, AY, AZ, GX, GY, GZ;
extern int lost_flag;//丢线标志位
int lost_count;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)///定时器中断函数
{
//	if(htim->Instance==TIM1)
//	{
//		count++;
//	}
			count++;
			GZ_JZ=GZ+20;//消除零漂,根据陀螺仪确定加或减的数值
	if(GZ_JZ<-10||GZ_JZ>10)
	   angle +=GZ_JZ;
	if(lost_flag==1)
	{
		lost_count++;
	}
}
