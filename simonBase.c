#include "stm32l476xx.h"
#include "simonBase.h"
#include "lcd.h"
#include "wiring.h"
#include <stdlib.h>

void updateLCDandBars(uint8_t *string, char bars[4])
{
	LCD_DisplayString(string);
	while ((LCD->SR & LCD_SR_UDR) != 0); // Wait for Update Display Request Bit
	if(bars[0])
	{
		LCD->RAM[6] |= 1U << 8;
	}
	if(bars[1])
	{
		LCD->RAM[4] |= 1U << 8;
	}
	if(bars[2])
	{
		LCD->RAM[6] |= 1U << 25;
	}
	if(bars[3])
	{
		LCD->RAM[4] |= 1U << 25;
	}
	LCD->SR |= LCD_SR_UDR; 
}

uint8_t genInput(uint8_t *inputDesired, char *lives)
{
	int choice = ((rand() % 4) + (int) inputDesired + lives[5]) % 4;
	switch(choice)
	{
		case 0:
			*inputDesired = 'l';
			updateLCDandBars((uint8_t *)"Left", lives);
			break;
		case 1:
			*inputDesired = 'r';
			updateLCDandBars((uint8_t *)"Right", lives);
			break;
		case 2:
			*inputDesired = 'u';
			updateLCDandBars((uint8_t *)"Up  ", lives);
			break;
		case 3:
			*inputDesired = 'd';
			updateLCDandBars((uint8_t *)"Down", lives);
			break;
	}
	return choice;
}

uint8_t genInputNoDisplay(const uint8_t *inputDesired, const char *lives)
{
	int choice = ((rand() % 4) + (int) *inputDesired + lives[5]) % 4;
	switch(choice)
	{
		case 0:
			return 'l';
		case 1:
			return 'r';
		case 2:
			return 'u';
		case 3:
			return 'd';
	}
}

void DisplayDesiredInput(char input, char * lives)
{
	switch(input)
	{
		case 'l':
			updateLCDandBars("Left  ", lives);
			break;
		case 'r':
			updateLCDandBars("Right ", lives);
			break;
		case 'u':
			updateLCDandBars("Up    ", lives);
			break;
		case 'd':
			updateLCDandBars("Down  ", lives);
			break;
	}
}

void rotateString(uint8_t *str, int strLen)
{
	char tempStart;
	tempStart = str[0];
	for(int i = 1; i < strLen; i++)
	{
		if(i == 0)
		{
			tempStart = str[i];
		}
		else
		{
			str[i-1] = str[i];
			
		}
	}
	str[strLen - 1] = tempStart;
}


void resetString(uint8_t *base, uint8_t *rotator, int len)
{
	for(int i = 0; i < len; i++)
	{
		rotator[i] = base[i];
	}
}

void JoystickInit()
{
	//Make sure pins PA1-3,5 are set to input mode
	GPIOA->MODER &= 0xFFFFF300;
	//Make sure the same pins are set to push pull;
	//GPIOA->OTYPER &= 0xFFFFFFF8;
	//Set the pins to Pull-down
	GPIOA->PUPDR &= 0xFFFFF300;
	GPIOA->PUPDR |= 0X000008AA;
	
	//Enable SYSCFG clock
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	
	//Enable interrupt for PA0
	SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI0;
	SYSCFG->EXTICR[0] |=  SYSCFG_EXTICR1_EXTI0_PA;
	
	//Enable interrupt for PA1
	SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI1;
	SYSCFG->EXTICR[0] |=  SYSCFG_EXTICR1_EXTI1_PA;
	
	//Enable interrupt for PA2
	SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI2;
	SYSCFG->EXTICR[0] |=  SYSCFG_EXTICR1_EXTI2_PA;
	
	//Enable interrupt for PA3
	SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI3;
	SYSCFG->EXTICR[0] |=  SYSCFG_EXTICR1_EXTI3_PA;
	
	SYSCFG->EXTICR[0] &= ~(0xFFF);
	
	//Enable interrupt for PA5
	SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR2_EXTI5;
	SYSCFG->EXTICR[0] |=  SYSCFG_EXTICR2_EXTI5_PA;
	
	SYSCFG->EXTICR[0] &= ~(0xFFF);

	//Enable rising edge, disable falling edge, enable interrupt for PA0
	EXTI->RTSR1 |= EXTI_RTSR1_RT0;
	EXTI->FTSR1 &= ~EXTI_FTSR1_FT0;
	EXTI->IMR1  |= EXTI_IMR1_IM0; 

	//Enable rising edge, disable falling edge, enable interrupt for PA1
	EXTI->RTSR1 |= EXTI_RTSR1_RT1;
	EXTI->FTSR1 &= ~EXTI_FTSR1_FT1;
	EXTI->IMR1  |= EXTI_IMR1_IM1; 
	
	//Enable rising edge, disable falling edge, enable interrupt for PA2
	EXTI->RTSR1 |= EXTI_RTSR1_RT2;
	EXTI->FTSR1 &= ~EXTI_FTSR1_FT2;
	EXTI->IMR1  |= EXTI_IMR1_IM2;
	
	//Enable rising edge, disable falling edge, enable interrupt for PA3
	EXTI->RTSR1 |= EXTI_RTSR1_RT3;
	EXTI->FTSR1 &= ~EXTI_FTSR1_FT3;
	EXTI->IMR1  |= EXTI_IMR1_IM3;
	
	//Enable rising edge, disable falling edge, enable interrupt for PA5
	EXTI->RTSR1 |= EXTI_RTSR1_RT5;
	EXTI->FTSR1 &= ~EXTI_FTSR1_FT5;
	EXTI->IMR1  |= EXTI_IMR1_IM5;
	
	//Set priority to 1
	NVIC_SetPriority(EXTI0_IRQn, 1);
	NVIC_SetPriority(EXTI1_IRQn, 1);
	NVIC_SetPriority(EXTI2_IRQn, 1);
	NVIC_SetPriority(EXTI3_IRQn, 1);
	NVIC_SetPriority(EXTI9_5_IRQn, 1);
	
	//Enable the interrupts
	NVIC_EnableIRQ(EXTI0_IRQn);
	NVIC_EnableIRQ(EXTI1_IRQn);
	NVIC_EnableIRQ(EXTI2_IRQn);
	NVIC_EnableIRQ(EXTI3_IRQn);	
	NVIC_EnableIRQ(EXTI9_5_IRQn);
}

void sendInput(char input)
{
	GPIOE->ODR &= 0xFFFFF3FF;
	switch(input)
	{
		case 'l':
			GPIOE->ODR |= 0x000;
			break;
		case 'r':
			GPIOE->ODR |= 0x400;
			break;
		case 'u':
			GPIOE->ODR |= 0x800;
			break;
		case 'd':
			GPIOE->ODR |= 0xC00;
			break;
	}
}

char receiveInput()
{
	int input = ReadInputWire();
	switch(input)
	{
		case 0:
			return 'l';
		case 1:
			return 'r';
		case 2:
			return 'u';
		case 3:
			return 'd';
		default:
			LCD_DisplayString("INVALID");
	}
}

