#include "l298n.h"
#include "main.h"
#include "tim.h"

void A0_Control(uint8_t dir, uint32_t speed)
{
      if( dir == 1 )
      {
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);	
      }
      else
      {
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);	
      }

      __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, speed);
}

void A1_Control(uint8_t dir, uint32_t speed)
{
      if( dir == 1 )
      {
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);	
      }
      else
      {
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);	
      }

      __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, speed);
}
