#include "stm32l476xx.h"
#include "simonBase.h"

#include "wiring.h"

void TestWireInit()
{
	//Enable GPIOH clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOHEN;
	
	//Pins PH0 and 1 are used
	//Set 0 to input and 1 to output
	GPIOH->MODER &= 0xFFFFFFF0;
	GPIOH->MODER |= 0x00000004;
	
	//Set 0 to pull-down and 1 to no pull
	GPIOH->PUPDR &= 0xFFFFFFF0;
	GPIOH->PUPDR |= 0x00000002;
	
	//Set 1 output to high
	GPIOH->ODR |= 0x2;
}

void LoseWireInit()
{
	//Enable GPIOE clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN;
	
	//Pins PE14 and 15 are used
	//Set 14 to input and 15 to output
	GPIOE->MODER &= 0x0FFFFFFF;
	GPIOE->MODER |= 0x40000000;
	
	//Set 14 to pull-down and 15 to no pull
	GPIOE->PUPDR &= 0x0FFFFFFF;
	GPIOE->PUPDR |= 0x20000000;
	
	//Initialize 15 output to high
	GPIOE->ODR |= 0x8000;
}

void SetLoseOutput(int outVal)
{
	if(outVal)
	{
		GPIOE->ODR |= 0x8000; 
	}
	else
	{
		GPIOE->ODR &= 0xFFFF7FFF;
	}
}

int GetLoseInput()
{
	int input = GPIOE->IDR & 0x4000;
	if(input) return 1;
	else return 0;
}

void InputTransmitWireInit(enum PlayerIDs PlayerID)
{
	//Enable GPIOE clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN;
	
	//Pins PE10 and 11 are used
	if(PlayerID == e_IDP1)
	{
		//Set 10 and 11 to output
		GPIOE->MODER &= 0xFF0FFFFF;
		GPIOE->MODER |= 0x00500000;
		
		//Set 10 and 11 to no pull
		GPIOE->PUPDR &= 0xFF0FFFFF;
	}
	else
	{
		//Set 10 and 11 to input
		GPIOE->MODER &= 0xFF0FFFFF;
		
		//Set 10 and 11 to pull-down
		GPIOE->PUPDR &= 0xFF0FFFFF;
		GPIOE->PUPDR |= 0x00A00000;
	}
}

int ReadInputWire()
{
	int input = GPIOE->IDR & 0xC00;
	input >>= 10;
	return input;
}

/*
//Enable GPIOE clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN;
	
	//Pins PE10 and 11 are used
	if(PlayerID == e_IDP1)
	{
		//Set 10 and 11 to output
		GPIOC->MODER &= 0xFF0FFFFF;
		GPIOC->MODER |= 0x00500000;
		
		//Set 10 and 11 to no pull
		GPIOC->PUPDR &= 0xFF0FFFFF;
	}
	else
	{
		//Set 14 and 15 to input
		GPIOC->MODER &= 0xFF0FFFFF;
*/

void ModeWireInit(enum PlayerIDs PlayerID)
{
	//Enable GPIOE clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN;
	
	if(PlayerID == e_IDP1)
	{
		//Set PE13 to output
		GPIOE->MODER &= 0xF3FFFFFF;
		GPIOE->MODER |= 0x04000000;
		
		//Set 13 to no pull
		GPIOE->PUPDR &= 0xF3FFFFFF;
	}
	else
	{
		//Set PE13 to input
		GPIOE->MODER &= 0xF3FFFFFF;
		
		//Set 13 to pull-down
		GPIOE->PUPDR &= 0xF3FFFFFF;
		GPIOE->PUPDR |= 0x08000000;
	}
}


void SetModeOutput(int outVal)
{
	if(outVal)
	{
		GPIOE->ODR |= 0x2000; 
	}
	else
	{
		GPIOE->ODR &= 0xFFFFDFFF;
	}
}

int GetModeInput()
{
	int input = GPIOE->IDR & 0x2000;
	if(input == 0x2000) return 1;
	else return 0;
}


void AcknowledgeWireInit()
{
	//Enable GPIOE clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN;
	
	//Set PE8 to input and 12 to output
	GPIOE->MODER &= 0xFCFCFFFF;
	GPIOE->MODER |= 0x01000000;
	
	//Set PE8 to pull-down and 12 to no pull
	GPIOE->PUPDR &= 0xFCFCFFFF;
	GPIOE->PUPDR |= 0x00020000;
}

void ToggleAcknowledgeWireOutput()
{
	int currOut = GPIOE->ODR & 0x1000;
	if(currOut)
	{
		GPIOE->ODR &= 0xFFFFEFFF;
	}
	else
	{
		GPIOE->ODR |= 0x1000;
	}
}

void SetAcknowledgeWireOutput(int outVal)
{
	GPIOE->ODR &= 0xFFFFEFFF;
	if(outVal)
	{
		GPIOE->ODR |= 0x1000;
	}
}

int GetAcknowledgeWireInput()
{
	int input = GPIOE->IDR & 0x100;
	if(input) return 1;
	else return 0;
}

void DependentWireSetup(enum PlayerIDs PlayerID)
{
	InputTransmitWireInit(PlayerID);
	ModeWireInit(PlayerID);
}