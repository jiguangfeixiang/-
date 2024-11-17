/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "oled.h"
#include "MPU6050.h"
#include "l298n.h"
#include "interrupt.h"
#include "end.h"
#define SPEED 380
#define chasu 170
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

// 180   273983
// 360   544518
uint8_t ID;
int16_t AX, AY, AZ, GX, GY, GZ;
int gd1, gd2, gd3, gd4, gd5; // 5-1:左到右
int speed_l, speed_r;
int GZ_JZ; // 去零飘后的陀螺仪角速度
extern int count;
extern int angle; // 陀螺仪积分角度
int target_angle; // 陀螺仪目标角度
float angle_kp;
float angle_kd;
float angle_ki;
int angle_error;
int angle_error_last;
int angle_inte;
int angle_maxinte = 20000;
int pricepwm;
int toggle_flag;
int toggle_time;
int goway;	 // 1为陀螺仪循迹,2为红外循迹
int key = 4; // 改赛题
int startflag;
int turnway; // 转弯方向
int stopflag;
int qes_num;
int lost_flag;		   // 丢线标志位
extern int lost_count; // 丢线时间
int loop_count = 4;	   // 小车循环数量
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void speed_control()
{
	if (speed_l > 900)
		speed_l = 900;
	if (speed_l < -900)
		speed_l = -900;
	if (speed_r > 900)
		speed_r = 900;
	if (speed_r < -900)
		speed_r = -900;

	if (speed_r > 0)
		A0_Control(1, speed_r);
	else
		A0_Control(0, -speed_r);

	if (speed_l > 0)
		A1_Control(1, speed_l);
	else
		A1_Control(0, -speed_l);
}
void gd_read() // 光电读取
{
	gd1 = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11);
	gd2 = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10);
	gd3 = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1);
	gd4 = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);
	gd5 = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7);
}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

	/* USER CODE BEGIN 1 */
	angle_kp = 0.01;
	angle_kd = 0.08;
	angle_ki = 0.002;
	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_TIM2_Init();
	MX_TIM1_Init();
	MX_TIM3_Init();
	/* USER CODE BEGIN 2 */
	OLED_Init(); // 屏幕初始化
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); // 开启定时器2通道2的PWM模式
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1); // 开启定时器2通道3的PWM模式
	MPU6050_Init();							  // 陀螺仪初始化
	HAL_TIM_Base_Start_IT(&htim1);			  // 开启定时器中断
	//	OLED_ShowString(1, 1, "ID:");
	//	ID = MPU6050_GetID();
	//	OLED_ShowHexNum(1, 4, ID, 2);
	while (1)
	{
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		if (goway == 1) // 角度循迹模式
		{
			angle_error_last = angle_error;
			angle_error = target_angle - angle;
			//		if(angle_error<5000&&angle_error>-5000)
			angle_inte += angle_error;
			if (angle_inte > angle_maxinte)
				angle_inte = angle_maxinte;
			if (angle_inte < -angle_maxinte)
				angle_inte = -angle_maxinte;
			pricepwm = angle_error * angle_kp + (angle_error - angle_error_last) * angle_kd + angle_inte * angle_ki;
			speed_l = SPEED + pricepwm;
			speed_r = SPEED - pricepwm;
		}
		if (goway == 2) // 红外循迹模式
		{
			if (gd5 == 1 || gd1 == 1)
			{
				if (gd5 == 1)
				{
					turnway = 1;
				}
				if (gd1 == 1)
					turnway = 2;
			}
			if (turnway > 0)
			{
				if (turnway == 1)
				{
					speed_l = SPEED + 00;
					speed_r = SPEED - chasu;
				}
				else
				{
					speed_r = SPEED + 00;
					speed_l = SPEED - chasu;
				}
			}
			if (gd4 == 0 && gd2 == 0)
			{
				if (turnway == 0)
				{
					speed_l = SPEED;
					speed_r = SPEED;
				}
			}
			else
			{
				turnway = 0;
				if (gd4 == 0 && gd2 == 1)
				{
					speed_l = SPEED - chasu;
					speed_r = SPEED + 00;
				}
				else if (gd4 == 1 && gd2 == 0)
				{
					speed_l = SPEED + 00;
					speed_r = SPEED - chasu;
				}
				else
				{
					speed_l = SPEED;
					speed_r = SPEED;
				}
			}
		}
		if (stopflag == 1)
		{
			speed_l = 0;
			speed_r = 0;
		}
		////////////////////////////////////////////
		//////////////第一题////////////////////////
		////////////////////////////////////////////
		if (key == 1)
		{
			startflag = 1; // 第一题
			goway = 1;	   // 角度循迹
			key = 0;
		}
		if (startflag == 1)
		{
			if (gd1 == 1 || gd2 == 1 || gd4 == 1 || gd5 == 1)
			{
				stopflag = 1; // 停车
			}
		}
		/////////////////////////////////////////
		//////////////////第二题/////////////////
		/////////////////////////////////////////
		if (key == 2)
		{
			startflag = 2; // 第二题
			goway = 1;	   // 角度循迹
			key = 0;
			qes_num = 1; // 进入第一阶段
		}
		if (startflag == 2)
		{
			if (qes_num == 1)
			{
				if (gd1 == 1 || gd2 == 1 || gd4 == 1 || gd5 == 1)
				{
					goway = 2;	 // 红外循迹
					qes_num = 2; // 进入第二阶段
								 ////						stopflag=1;
				}
			}
			if (qes_num == 2)
			{
				if (gd1 == 0 && gd2 == 0 && gd4 == 0 && gd5 == 0 && angle < -250000)
				{
					lost_flag = 1;
					if (lost_count > 10)
					{
						goway = 1;	 // 角度循迹
						qes_num = 3; // 进入第三阶段
						//								stopflag=1;
						lost_flag = 0;
						lost_count = 0;
					}
				}
			}
			if (qes_num == 3)
			{
				target_angle = -280000; // 改变目标角度
				goway = 1;				// 角度循迹
				qes_num = 4;			// 进入第四阶段
			}
			if (qes_num == 4)
			{
				if (gd1 == 1 || gd2 == 1 || gd4 == 1 || gd5 == 1)
				{
					goway = 2;	 // 红外循迹
					qes_num = 5; // 进入第五阶段??
								 //  						stopflag=1;
				}
			}
			if (qes_num == 5)
			{
				if (gd1 == 0 && gd2 == 0 && gd4 == 0 && gd5 == 0 && angle < -500000)
				{
					lost_flag = 1;
					if (lost_count > 10)
					{
						stopflag = 1; // 停车
						lost_flag = 0;
						lost_count = 0;
					}
				}
			}
		}

		/////////////////////////////////////////////////
		//////////////////第三题/////////////////////////
		/////////////////////////////////////////////////
		if (key == 3)
		{
			startflag = 3;		   // 第三题
			target_angle = -60000; // 改变目标角度
			goway = 1;			   // 角度循迹
			key = 0;
			qes_num = 1; // 进入第一阶段??
		}
		if (startflag == 3)
		{
			if (qes_num == 1)
			{
				if (gd1 == 1 || gd2 == 1 || gd4 == 1 || gd5 == 1)
				{
					goway = 2;	 // 红外循迹
					qes_num = 2; // 进入第二阶段??
								 //  						stopflag=1;
				}
			}
			if (qes_num == 2)
			{
				if (gd1 == 0 && gd2 == 0 && gd4 == 0 && gd5 == 0 && angle > 280000)
				{
					lost_flag = 1;
					if (lost_count > 0)
					{
						qes_num = 3; // 进入第三阶段
						//								stopflag=1;
						lost_flag = 0;
						lost_count = 0;
					}
				}
			}
			if (qes_num == 3)
			{
				target_angle = 360000; // 改变目标角度
				goway = 1;			   // 角度循迹
				qes_num = 4;		   // 进入第四阶段??
			}
			if (qes_num == 4)
			{
				if (gd1 == 1 || gd2 == 1 || gd4 == 1 || gd5 == 1)
				{
					goway = 2;	 // 红外循迹
					qes_num = 5; // 进入第五阶段??
								 //  						stopflag=1;
				}
			}
			if (qes_num == 5)
			{
				if (gd1 == 0 && gd2 == 0 && gd4 == 0 && gd5 == 0 && angle < 55000)
				{
					lost_flag = 1;
					if (lost_count > 20)
					{
						stopflag = 1; // 停车
						lost_flag = 0;
						lost_count = 0;
					}
				}
			}
		}
		////////////////////////////////////////////////////
		//////////////////////第四题////////////////////////
		////////////////////////////////////////////////////
		if (key == 4 && loop_count != 0)
		{
			startflag = 4;		   // 第四题
			target_angle = -60000; // 改变目标角度
			goway = 1;			   // 角度循迹
			qes_num = 1;		   // 进入第一阶段??
			loop_count = loop_count - 1;
		}
		if (startflag == 4)
		{
			if (qes_num == 1)
			{
				if (gd1 == 1 || gd2 == 1 || gd4 == 1 || gd5 == 1)
				{
					goway = 2;	 // 红外循迹
					qes_num = 2; // 进入第二阶段??
								 //  						stopflag=1;
				}
			}
			if (qes_num == 2)
			{
				if (gd1 == 0 && gd2 == 0 && gd4 == 0 && gd5 == 0 && angle > 270000)
				{
					lost_flag = 1;
					if (lost_count > 0)
					{	
						qes_num = 3; // 进入第三阶段
						//								stopflag=1;
						lost_flag = 0;
						lost_count = 0;
					}
				}
			}
			if (qes_num == 3)
			{
				target_angle = 350000; // 改变目标角度
				goway = 1;			   // 角度循迹
				qes_num = 4;		   // 进入第四阶段??
			}
			if (qes_num == 4)
			{
				if (gd1 == 1 || gd2 == 1 || gd4 == 1 || gd5 == 1)
				{
					goway = 2;	 // 红外循迹
					qes_num = 5; // 进入第五阶段??
								 //  						stopflag=1;
				}
			}
			if (qes_num == 5)
			{
				if (gd1 == 0 && gd2 == 0 && gd4 == 0 && gd5 == 0 && angle < 40000)
				{
					lost_flag = 1;
					qes_num = 1;
					target_angle = -55000;
					angle -= 25000;
					goway = 1;
					// angle+=2000;
					// stopflag=1;//停车
					lost_flag = 0;
					lost_count = 0;
					startflag = 4;
					angle_inte=0;
					OLED_ShowString(2, 5, "enter five stages");
				}
			}
		}
		speed_control();
		gd_read();

		//		OLED_ShowNum(2, 1, gd1,1);
		//		OLED_ShowNum(2, 5, gd2,1);
		//		OLED_ShowNum(3, 1, gd3,1);
		//		OLED_ShowNum(3, 5, gd4,1);
		//		OLED_ShowNum(4, 1, gd5,1);

		MPU6050_GetData(&AX, &AY, &AZ, &GX, &GY, &GZ); // 陀螺仪读取数据
		// OLED_ShowSignedNum(2, 1, AX, 5);//屏幕显示数据
		//		OLED_ShowSignedNum(3, 1, AY, 5);
		// OLED_ShowSignedNum(4, 1, AZ, 5);
		//	OLED_ShowSignedNum(2, 8, GZ_JZ, 5);//
		OLED_ShowSignedNum(3, 5, angle, 9);
		OLED_ShowNum(1, 5, qes_num, 5);
		//	OLED_ShowNum(4, 8,count, 5);
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
	   ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
