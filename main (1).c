#include "stm32l476xx.h"
#include "lcd.h"
#include "simonBase.h"
#include "wiring.h"
#include "displayStrings.h"

//*************************************  32L476GDISCOVERY ***************************************************************************
// STM32L4:  STM32L476VGT6 MCU = ARM Cortex-M4 + FPU + DSP, 
//           LQFP100, 1 MB of Flash, 128 KB of SRAM
//           Instruction cache = 32 lines of 4x64 bits (1KB)
//           Data cache = 8 lines of 4x64 bits (256 B)
//
// Joystick (MT-008A): 
//   Right = PA2        Up   = PA3         Center = PA0
//   Left  = PA1        Down = PA5
//***********************************************************************************************************************************

int count = 0, maxReload= 100, countLim = 100;
int lifeCount = 4;
char livesBase[4] = {1,1,1,1};
char lives[4] = {1,1,1,1};
uint8_t inputReceived = 0;
uint8_t inputDesired = 0;
uint8_t lastInupt = 0;
int gameOver = 0;

int playerConnected = 0;


int menuCountLim = 12, countDownCountLim = 40;
int idSelPrepped = 0;

char inputs[100];
int nextOpenIndex = 0, nextReplaceIndex = 0, inputsInitialized = 0, roundCount = 1;

int prevAckVal, currAckVal;

///////////////////////////////////// ENUM INSTANTIATIONS /////////////////////////////////////////////

enum GameStates GameState = e_MENU;
enum CDSubStates CDSubState = e_CDSUB_READY;
enum MenuSubStates MenuSubState = e_MENUSUB_1P;
enum PlayerIDs PlayerID = e_IDP1;
enum GameOverSubStates GameOverSubState;
enum VSSubStates VSSubState = e_VSSUB_LEVEL1;

///////////////////////////////////// STATE SETUPS /////////////////////////////////////

void ResetEnums()
{
	GameState = e_MENU;
	CDSubState = e_CDSUB_READY;
	MenuSubState = e_MENUSUB_1P;
	PlayerID = e_IDP1;
	//GameOverSubState = e_GOSUB_LOSER;
	VSSubState = e_VSSUB_LEVEL1;
}

void OnePInit()
{
	count = 0;
	maxReload = 100;
	countLim = 100;
	lifeCount = 4;
	resetString(livesBase, lives, 4);
	resetString(scoreBase, score, scoreLen);
}

/*
//Get the initial 30 inputs for the inputs array
void TwoPInitInputs(enum GameStates mode)
{
	nextOpenIndex = 0;
	int numOfInputs = 0;
	if(mode == e_2P_ENDLESS) numOfInputs = 100;
	else if(mode == e_2P_VS) numOfInputs = 15;
	
	if(PlayerID == e_IDP1)
	{
		//Generate the initial input. Make sure it won't have the encoding 00 (default state of wires)
		char inputRet, lastAckVal = 0, newAckVal = GetAcknowledgeWireInput();
		genInputNoDisplay(&inputRet, lives);
		inputs[0] = inputRet;
		if(inputs[0] == 'l') inputs[0] = 'u';
		sendInput(inputs[0]);
		ToggleAcknowledgeWireOutput();
		
		//Generate the remaining inputs
		for(nextOpenIndex = 1; nextOpenIndex < numOfInputs; nextOpenIndex++)
		{
			//Ensure other player got the input
			while(newAckVal == lastAckVal)
			{
				newAckVal = GetAcknowledgeWireInput();
			}
			lastAckVal = newAckVal;
			
			while(inputRet == inputs[nextOpenIndex - 1])
			{
				genInputNoDisplay(&inputRet, lives);
			}
			inputs[nextOpenIndex] = inputRet;
			sendInput(inputs[nextOpenIndex]);
			ToggleAcknowledgeWireOutput();
		}
	}
	else
	{
		int lastAckVal = 0, newAckVal = GetAcknowledgeWireInput();
		for(; nextOpenIndex < numOfInputs; nextOpenIndex++)
		{
			while(newAckVal == lastAckVal)
			{
				newAckVal = GetAcknowledgeWireInput();
			}
			lastAckVal = newAckVal;
			
			int input = receiveInput();
			inputs[nextOpenIndex] = input;
			//Acknowledge the input was received by toggling the ack wire
			ToggleAcknowledgeWireOutput();
		}
	}
	
	inputsInitialized = 1;
}*/

void Gen2PEndlessInputs()
{
	char input = genInputNoDisplay(&inputDesired, lives);
	inputs[0] = input;
	for(nextOpenIndex = 1; nextOpenIndex < 100; nextOpenIndex++)
	{
		input = genInputNoDisplay(&inputDesired, lives);
		while(input == inputs[nextOpenIndex - 1])
		{
			 input = genInputNoDisplay(&inputDesired, lives);
		}
		inputs[nextOpenIndex] = input;
	}
}

//RELIES ON ACK WIRES STARTING LOW
void TransmitInputs(enum GameStates mode)
{
	int numOfInputs = 0, nextTransIndex = 0, prevAckVal = 0, currAckVal = GetAcknowledgeWireInput();
	if(mode == e_2P_ENDLESS) numOfInputs = 100;
	else if(mode == e_2P_VS)
	{
		numOfInputs = 15;
	}
	
	if(PlayerID == e_IDP1)
	{
		//Send initial input
		sendInput(inputs[0]);
		ToggleAcknowledgeWireOutput();
		
		//Generate the remaining inputs
		for(nextOpenIndex = 1; nextOpenIndex < numOfInputs; nextOpenIndex++)
		{
			//Ensure other player got the input
			while(currAckVal == prevAckVal)
			{
				currAckVal = GetAcknowledgeWireInput();
			}
			prevAckVal = currAckVal;
			
			//Send next input
			sendInput(inputs[nextOpenIndex]);
			ToggleAcknowledgeWireOutput();
		}
	}
	else
	{
		for(; nextOpenIndex < numOfInputs; nextOpenIndex++)
		{
			while(currAckVal == prevAckVal)
			{
				currAckVal = GetAcknowledgeWireInput();
			}
			prevAckVal = currAckVal;
			
			int input = receiveInput();
			inputs[nextOpenIndex] = input;
			//Acknowledge the input was received by toggling the ack wire
			ToggleAcknowledgeWireOutput();
		}
	}
	
	//Reset ack wires
	SetAcknowledgeWireOutput(0);
	inputsInitialized = 1;
	if(mode == e_2P_ENDLESS) MenuSubState = e_MENUSUB_2P_ENDLESS;
	if(mode == e_2P_VS)
	{
		prevAckVal = 0;
		MenuSubState = e_MENUSUB_2P_VS;
		if(PlayerID == e_IDP1) VSSubState = e_VSSUB_WAITING;
	}
}

void TwoPInit()
{
	count = 0;
	maxReload = 100;
	countLim = 100;
	lifeCount = 2;
	lives[0] = 1;
	lives[1] = 1;
	lives[2] = 0;
	lives[3] = 0;
	nextReplaceIndex = 0;
	resetString(scoreBase, score, scoreLen);
	//Display first input
	inputDesired = inputs[0];
	DisplayDesiredInput(inputDesired, lives);
}

void VSInit()
{
	count = 0;
	if(PlayerID == e_IDP1) VSSubState = e_VSSUB_GENNING;
	if(roundCount == 1) 
	{
		VSSubState = e_VSSUB_LEVEL1;
		countLim = 90;
		lifeCount = 2;
		lives[0] = 1;
		lives[1] = 1;
		lives[2] = 0;
		lives[3] = 0;
		resetString(scoreBase, score, scoreLen);
	}
	else if(roundCount == 2)
	{
		VSSubState = e_VSSUB_LEVEL2;
		countLim = 75;
	}
	else if(roundCount == 3)
	{
		VSSubState = e_VSSUB_LEVEL3;
		countLim = 60;
	}
		nextReplaceIndex = 0;
		//Display first input
		inputDesired = inputs[0];
		DisplayDesiredInput(inputDesired, lives);
}

void PrepMenu()
{
	countLim = menuCountLim;
	count = 0;
	resetString(OnePPrintBase, OnePPrintRotator, OnePPrintLen);
	resetString(TwoPPrintBase, TwoPPrintRotator, TwoPPrintLen);
	resetString(VSPrintBase, VSPrintRotator, VSPrintLen);
}

void PrepCountDown()
{
	countLim = countDownCountLim;
	count = 0;
}

void PrepIDSel()
{
	countLim = menuCountLim;
	count = 0;
	idSelPrepped = 1;
}



///////////////////////////////////// WIRE SETUPS /////////////////////////////////////



///////////////////////////////////// BUTTON INTERRUPT HANDLERS /////////////////////////////////////

//Center Button Handler
void EXTI0_IRQHandler()
{
	
	if(GameState == e_MENU)
	{
		//If the player selects 1 player mode, simply initiate countdown
		if(MenuSubState == e_MENUSUB_1P)
		{
			PrepCountDown();
			GameState = e_COUNTDOWN;
		}
		else if (MenuSubState == e_MENUSUB_2P_ENDLESS)
		{
			//If second player connected and they are ready to start, only then initiate countdown
			if(playerConnected && PlayerID == e_IDP1)
			{
				SetAcknowledgeWireOutput(0);
				SetLoseOutput(0);
				if(!inputsInitialized) 
				{
					Gen2PEndlessInputs();
					TransmitInputs(e_2P_ENDLESS);
				}
				PrepCountDown();
				GameState = e_COUNTDOWN;
			}
		}
		else if(MenuSubState == e_MENUSUB_2P_VS)
		{
			if(playerConnected && PlayerID == e_IDP1)
			{
				SetAcknowledgeWireOutput(0);
				SetModeOutput(1);
				SetLoseOutput(0);
				GameState = e_2P_VS;
				VSSubState = e_VSSUB_GENNING;
			}
		}
	}
	
	else if(GameState == e_IDSEL)
	{
		PlayerID = e_IDP1;
		ToggleAcknowledgeWireOutput();
		DependentWireSetup(PlayerID);
		PrepMenu();
		GameState = e_MENU;
	}
	
	//If the center button was pressed when the game is over, reset menu displays and return to menu
	else if(GameState == e_GAMEOVER)
	{
		PrepMenu();
		GameState = e_MENU;
	}
	
	//Clear interrupt
	EXTI->PR1 |= EXTI_PR1_PIF1;
}

void EXTI1_IRQHandler()
{
	//Left Button Handler
	if(GameState == e_2P_VS && VSSubState == e_VSSUB_GENNING && PlayerID == e_IDP1 && inputs[nextOpenIndex - 1] != 'l')
	{
		inputs[nextOpenIndex] = 'l';
		nextOpenIndex++;
	}
	else
	{
		inputReceived = 'l';
		count = countLim;
	}
	
	//Clear interrupt
	EXTI->PR1 |= EXTI_PR1_PIF1;
}

void EXTI2_IRQHandler()
{
	//Right Button Handler
	if(GameState == e_2P_VS && VSSubState == e_VSSUB_GENNING && PlayerID == e_IDP1 && inputs[nextOpenIndex - 1] != 'r')
	{
		inputs[nextOpenIndex] = 'r';
		nextOpenIndex++;
	}
	else
	{
		inputReceived = 'r';
		count = countLim;
	}
	
	//Clear interrupt
	EXTI->PR1 |= EXTI_PR1_PIF2;
}

void EXTI3_IRQHandler()
{
	//Up Button Handler
	if(GameState == e_MENU)
	{
		if(MenuSubState == e_MENUSUB_1P)
		{
			resetString(OnePPrintBase, OnePPrintRotator, OnePPrintLen);
			MenuSubState = e_MENUSUB_2P_VS;
		}
		else if(MenuSubState == e_MENUSUB_2P_ENDLESS)
		{
			resetString(TwoPPrintBase, TwoPPrintRotator, TwoPPrintLen);
			MenuSubState = e_MENUSUB_1P;
		}
		else if(MenuSubState == e_MENUSUB_2P_VS)
		{
			resetString(VSPrintBase, VSPrintRotator, VSPrintLen);
		  MenuSubState = e_MENUSUB_2P_ENDLESS;
		}
	}
	
	else if(GameState == e_2P_VS && VSSubState == e_VSSUB_GENNING && PlayerID == e_IDP1 && inputs[nextOpenIndex - 1] != 'u')
	{
		inputs[nextOpenIndex] = 'u';
		nextOpenIndex++;
	}
	
	else
	{
		inputReceived = 'u';
		count = countLim;
	}
	
	//Clear interrupt
	EXTI->PR1 |= EXTI_PR1_PIF3;
}

void EXTI9_5_IRQHandler()
{
	//Down Button Handler
	if(GameState == e_MENU)
	{
		if(MenuSubState == e_MENUSUB_1P)
		{
			resetString(OnePPrintBase, OnePPrintRotator, OnePPrintLen);
			MenuSubState = e_MENUSUB_2P_ENDLESS;
		}
		else if(MenuSubState == e_MENUSUB_2P_ENDLESS)
		{
			resetString(TwoPPrintBase, TwoPPrintRotator, TwoPPrintLen);
			MenuSubState = e_MENUSUB_2P_VS;
		}
		else if(MenuSubState == e_MENUSUB_2P_VS)
		{
			resetString(VSPrintBase, VSPrintRotator, VSPrintLen);
		  MenuSubState = e_MENUSUB_1P;
		}
	}
	
	else if(GameState == e_2P_VS && VSSubState == e_VSSUB_GENNING && PlayerID == e_IDP1 && inputs[nextOpenIndex - 1] != 'd')
	{
		inputs[nextOpenIndex] = 'd';
		nextOpenIndex++;
	}
	
	else
	{
		inputReceived = 'd';
		count = countLim;
	}
	
	//Debounce
	while((GPIOA->IDR & 0x20) == 0x20);
	
	//Clear interrupt
	EXTI->PR1 |= EXTI_PR1_PIF5;
}




///////////////////////////////////// TIMER STUFF /////////////////////////////////////

void enableTIM2()
{
	//Enable timer 2 clock and set direction to up counting
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
	TIM2->CR1 &= ~TIM_CR1_CMS;
	TIM2->CR1 &= ~TIM_CR1_DIR;
	TIM2->DIER |= TIM_DIER_UIE;
	
	//Select master mode
	TIM2->CR2 &= ~TIM_CR2_MMS;
	TIM2->CR2 |= ~TIM_CR2_MMS_2;
	
	//Output compare 1 mode
	TIM2->CR2 &= ~TIM_CCMR1_OC1M;
	TIM2->CR2 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;
	
	//Frequency setup
	TIM2->PSC = 7;
	TIM2->ARR = 9999;
	TIM2->CCR1 = 500;
	
	//Enable interrupt and timer
	TIM2->CCER |= TIM_CCER_CC1E;
	NVIC_EnableIRQ(TIM2_IRQn);
	NVIC_SetPriority(TIM2_IRQn, 2);
	TIM2->CR1 |= TIM_CR1_CEN;
}






void TIM2_IRQHandler()
{
	//If lose input is zero and board is p2, start the appropriate 2 player game mode
	if(!inputsInitialized && GetLoseInput() == 0 && PlayerID == e_IDP2)
	{
		SetLoseOutput(0);
		if(GetModeInput() == 0)
		{
			TransmitInputs(e_2P_ENDLESS);
			PrepCountDown();
			GameState = e_COUNTDOWN;
		}
		else 
		{
			if(VSSubState != e_VSSUB_WAITING)
			{
				GameState = e_2P_VS;
				VSSubState = e_VSSUB_WAITING;
				SetAcknowledgeWireOutput(1);
			}
		}
	}
	
	if(PlayerID == e_IDP2 && GameState == e_MENU)
	{
		if(count >= countLim)
		{
			LCD_DisplayString(WaitingOnP1Rotator);
			rotateString(WaitingOnP1Rotator, WaitingOnP1Len);
			count = 0;
		}
		else count++;
	}
	
	///////////////////////////////////// MENU /////////////////////////////////////
	
	//Handle case that the menu is currently open
	else if(GameState == e_MENU)
	{
		//When count reaches countLim, rotate the appropriate string across the screen
			//so that all text can be seen and reset count
		if(count >= countLim)
		{
			if(MenuSubState == e_MENUSUB_1P)
			{
				LCD_DisplayString(OnePPrintRotator);
				rotateString(OnePPrintRotator, OnePPrintLen);
			}
			if(MenuSubState == e_MENUSUB_2P_ENDLESS)
			{
				LCD_DisplayString(TwoPPrintRotator);
				rotateString(TwoPPrintRotator, TwoPPrintLen);
			}
			if(MenuSubState == e_MENUSUB_2P_VS)
			{
				LCD_DisplayString(VSPrintRotator);
				rotateString(VSPrintRotator, VSPrintLen);
			}
			count = 0;
		}
		//If not at countLim, ensure countLim is correct and increment count
		else
		{
			countLim = menuCountLim;
			count++;
		}
	}
	
	///////////////////////////////////// COUNTDOWN /////////////////////////////////////
	
	//If in countdown state,
	else if(GameState == e_COUNTDOWN)
	{
		//Move to next count state (ready, set, go) when count reaches countLim
		if(count >= countLim)
		{
			if(CDSubState == e_CDSUB_READY) CDSubState = e_CDSUB_SET;
			else if(CDSubState == e_CDSUB_SET) CDSubState = e_CDSUB_GO;
			//If the state is already go, start the game
			else if(CDSubState == e_CDSUB_GO)
			{
				CDSubState = e_CDSUB_READY;
				if(MenuSubState == e_MENUSUB_1P && PlayerID == e_IDP1)
				{
					GameState = e_1P;
					OnePInit();
					genInput(&inputDesired, lives);
				}
				else if(MenuSubState == e_MENUSUB_2P_ENDLESS)
				{
					TwoPInit();
					GameState = e_2P_ENDLESS;
				}
				else if(MenuSubState == e_MENUSUB_2P_VS)
				{
					VSInit();
					GameState = e_2P_VS;
				}
			}
			
			count = 0;
		}
		//If not at countLim, display the appropriate string for the count state, ensure countLim is correct,
			//and increment count
		else
		{
			if(CDSubState == e_CDSUB_READY) LCD_DisplayString(CountDown3);
			else if(CDSubState == e_CDSUB_SET) LCD_DisplayString(CountDown2);
			else if(CDSubState == e_CDSUB_GO) LCD_DisplayString(CountDown1);
			
			countLim = countDownCountLim;
			count++;
		}
	}
	
	///////////////////////////////////// IDSEL /////////////////////////////////////
	
	//If in IDSel state,
	else if(GameState == e_IDSEL)
	{
		currAckVal = GetAcknowledgeWireInput();
		if(currAckVal != prevAckVal)
		{
			PlayerID = e_IDP2;
			DependentWireSetup(PlayerID);
			PrepMenu();
			GameState = e_MENU;
		}
		
		else if(count >= countLim)
		{
			LCD_DisplayString(IDSelPrintRotator);
			rotateString(IDSelPrintRotator, IDSelPrintLen);
			
			count = 0;
		}
		else
		{
			count++;
		}
	}
	
	///////////////////////////////////// 1P MODE /////////////////////////////////////
	
	else if(GameState == e_1P)
	{
		if(count >= countLim){
			if(inputReceived == inputDesired)
			{
				//If correct input, change the count limit based on the lifeCount and increment score
				if(countLim - lifeCount < 1) countLim = 1;
				else countLim -= lifeCount;
				score[7]++;
				if(score[7] > '9')
				{
					score[6]++;
					score[7] = '0';
				}
			}
			else
			{
				//If the input was wrong or an input wasn't provided fast enough, decrement the number of lives,
					//Update the life bar display, give the user a bit more time for the next input, and check if
					//the game should end
				lifeCount--;
				lives[lifeCount] = 0;
				countLim += 10;
				if(lifeCount == 0) GameState = e_GAMEOVER;
			}
			//If the game isn't yet over, generate a new input
			if(GameState != e_GAMEOVER) 
			{
				lastInupt = inputDesired;
				while(inputDesired == lastInupt)
				{
					genInput(&inputDesired, lives);
				}
			}
			count = 0;
			inputReceived = 0;
		}
		count++;
	}
	
	///////////////////////////////////// 2P ENDLESS MODE /////////////////////////////////////
	
	else if(GameState == e_2P_ENDLESS)
	{
		//Check if other player lost
		if(GetLoseInput() == 1)
		{
			GameState = e_GAMEOVER;
			GameOverSubState = e_GOSUB_ENDLESS_WINNER;
			for(int i = 7; i < twoPEndlessScoreWinnerLen; i++)
			{
				TwoPEndlessScoreWinnerBase[i] = score[i-7];
				TwoPEndlessScoreWinnerRotator[i] = score[i-7];
			}
		}
		else if(count >= countLim){
			if(inputReceived == inputDesired)
			{
				//If correct input, change the count limit based on the lifeCount and increment score
				if(countLim - lifeCount < 1) countLim = 1;
				else countLim -= lifeCount;
				score[7]++;
				if(score[7] > '9')
				{
					score[6]++;
					score[7] = '0';
				}
			}
			else
			{
				//If the input was wrong or an input wasn't provided fast enough, decrement the number of lives,
					//Update the life bar display, give the user a bit more time for the next input, and check if
					//the game should end
				lifeCount--;
				lives[lifeCount] = 0;
				countLim += 10;
				if(lifeCount == 0)
				{
					GameState = e_GAMEOVER;
					GameOverSubState = e_GOSUB_ENDLESS_LOSER;
					for(int i = 6; i < twoPEndlessScoreLoserLen; i++)
					{
						TwoPEndlessScoreLoserBase[i] = score[i-6];
						TwoPEndlessScoreLoserRotator[i] = score[i-6];
					}
					SetLoseOutput(1);
				}
			}
			//If the game isn't yet over, generate a new input
			if(GameState != e_GAMEOVER) 
			{
				//inputs[nextReplaceIndex] = 0;
				//FOR TESTING, CHANGE LATER
				nextReplaceIndex = (nextReplaceIndex + 1) % nextOpenIndex;
				inputDesired = inputs[nextReplaceIndex];
				DisplayDesiredInput(inputDesired, lives);
			}
			count = 0;
		}
		//Case that count < countLim
		else
		{
			count++;
		}
	}
	
	///////////////////////////////////// 2P VS MODE /////////////////////////////////////
	
	else if(GameState == e_2P_VS)
	{
		if(PlayerID == e_IDP1)
		{
			//Check if all 15 inputs were collected
			if(VSSubState == e_VSSUB_GENNING && nextOpenIndex == 15)
			{
				ToggleAcknowledgeWireOutput();
				//Wait for other player to be ready for transmission
					while(GetAcknowledgeWireInput() == 1);
					SetAcknowledgeWireOutput(0);
				TransmitInputs(e_2P_VS);
				VSSubState = e_VSSUB_WAITING;
				nextOpenIndex = 0;
				countLim = menuCountLim;
				roundCount++;
			}
			//Check if P2 lost
			else if(VSSubState == e_VSSUB_WAITING && GetLoseInput() == 1)
			{
				GameState = e_GAMEOVER;
				GameOverSubState = e_GOSUB_VS_WINNER;
			}
			else if(VSSubState == e_VSSUB_WAITING)
			{
				currAckVal = GetAcknowledgeWireInput();
				if(currAckVal != prevAckVal)
				{
					if(roundCount == 4)
					{
						GameState = e_GAMEOVER;
						GameOverSubState = e_GOSUB_VS_LOSER;
					}
					else
					{
						VSSubState = e_VSSUB_GENNING;
					}
				}
			}
			
			if(count >= countLim)
			{
				if(VSSubState == e_VSSUB_GENNING)
				{
					LCD_DisplayString(GenningPrintRotator);
					rotateString(GenningPrintRotator, GenningPrintLen);
				}
				else if(VSSubState == e_VSSUB_WAITING)
				{
					LCD_DisplayString(WaitingOnP2Rotator);
					rotateString(WaitingOnP2Rotator, WaitingOnP2Len);
				}
				else LCD_DisplayString("Blep");
				
				count = 0;
			}
			else count++;
		}
		
		else if(PlayerID == e_IDP2)
		{
			//Check if all 15 inputs were exhausted
			/*if(nextReplaceIndex == nextOpenIndex - 1)
			{
				if(VSSubState == e_VSSUB_LEVEL3)
				{
					//FIX THIS TO FLIP ACK WIRE
					SetLoseOutput(1);
					GameState = e_GAMEOVER;
					GameOverSubState = e_GOSUB_VS_WINNER;
				}
				else
				{
					TransmitInputs(e_2P_VS);
					VSInit();
				}
			}*/
			
			if(count >= countLim){
				if(VSSubState == e_VSSUB_WAITING)
				{
					LCD_DisplayString(WaitingOnP1Rotator);
					rotateString(WaitingOnP1Rotator, WaitingOnP1Len);
					currAckVal = GetAcknowledgeWireInput();
					if(currAckVal != prevAckVal){
						prevAckVal = currAckVal;
						//Wait for other player to be ready for transmission
						SetAcknowledgeWireOutput(0);
						while(GetAcknowledgeWireInput() == 1);
						
						TransmitInputs(e_2P_VS);
						VSInit();
						GameState = e_COUNTDOWN;
						MenuSubState = e_MENUSUB_2P_VS;
					}
				}
				//If correct input, change the count limit based on the lifeCount and increment score
				else if(inputReceived == inputDesired)
				{
					if(countLim - lifeCount < 1) countLim = 1;
					else countLim -= lifeCount;
					score[7]++;
					if(score[7] > '9')
					{
						score[6]++;
						score[7] = '0';
					}
				}
				else
				{
					//If the input was wrong or an input wasn't provided fast enough, decrement the number of lives,
						//Update the life bar display, give the user a bit more time for the next input, and check if
						//the game should end
					lifeCount--;
					lives[lifeCount] = 0;
					countLim += 10;
					if(lifeCount == 0)
					{
						GameState = e_GAMEOVER;
						GameOverSubState = e_GOSUB_VS_LOSER;
						SetLoseOutput(1);
					}
				}
				
				//If the game isn't yet over, generate a new input
				if(PlayerID == e_IDP2 && (VSSubState == e_VSSUB_LEVEL1 || VSSubState == e_VSSUB_LEVEL2 || VSSubState == e_VSSUB_LEVEL3)) 
				{
					nextReplaceIndex++;
					if(nextReplaceIndex == 15)
					{
						if(VSSubState == e_VSSUB_LEVEL3)
						{
							//FIX THIS TO FLIP ACK WIRE
							ToggleAcknowledgeWireOutput();
							GameState = e_GAMEOVER;
							GameOverSubState = e_GOSUB_VS_WINNER;
						}
						else
						{
							ToggleAcknowledgeWireOutput();
							VSSubState = e_VSSUB_WAITING;
							nextOpenIndex = 0;
							prevAckVal = GetAcknowledgeWireInput();
							countLim = menuCountLim;
							roundCount++;
						}
					}
					else
					{
						inputDesired = inputs[nextReplaceIndex];
						DisplayDesiredInput(inputDesired, lives);
					}
				}
				count = 0;
			}
			else count++;
		}
	}
	
	///////////////////////////////////// GAMEOVER /////////////////////////////////////
	
	else if(GameState == e_GAMEOVER)
	{
		if(count >= countLim)
		{
			//2P mode prints
			if(MenuSubState != e_MENUSUB_1P || PlayerID == e_IDP2)
			{
				if(GameOverSubState == e_GOSUB_ENDLESS_WINNER)
				{
					LCD_DisplayString(TwoPEndlessScoreWinnerRotator);
					rotateString(TwoPEndlessScoreWinnerRotator, twoPEndlessScoreWinnerLen);
				}
				else if(GameOverSubState == e_GOSUB_ENDLESS_LOSER)
				{
					LCD_DisplayString(TwoPEndlessScoreLoserRotator);
					rotateString(TwoPEndlessScoreLoserRotator, twoPEndlessScoreLoserLen);
				}
				else if(GameOverSubState == e_GOSUB_VS_WINNER)
				{
					LCD_DisplayString(TwoPVSWinnerRotator);
					rotateString(TwoPVSWinnerRotator, TwoPVSWinnerLen);
				}
				else if(GameOverSubState == e_GOSUB_VS_LOSER)
				{
					LCD_DisplayString(TwoPVSLoserRotator);
					rotateString(TwoPVSLoserRotator, TwoPVSLoserLen);
				}
			}
			else
			{
				LCD_DisplayString(score);
				rotateString(score, scoreLen);
			}
			count = 0;
		}
		else
		{
			countLim = 12;
			count++;
		}
	}
	
	//Prevent re-entry
	if((TIM2->SR & TIM_SR_UIF) !=0){
		TIM2->SR &= ~(TIM_SR_UIF);
	}
}



int main(void){
	
	//Initialize needed pins and clocks
	LCD_Initialization();
	JoystickInit();
	LCD_Clear();
	enableTIM2();
	TestWireInit();
	LoseWireInit();
	AcknowledgeWireInit();
	prevAckVal = GetAcknowledgeWireInput();
	//int barInit[4] = {1,1,1,1};
	//updateLCDandBars((uint8_t *)"Ready", barInit);
	
	
	/*LCD_DisplayString((uint8_t*)"Test");
	
	LCD_Clear();
	LCD_DisplayName();
	LCD_Clear();*/
	while(1)
	{
		//If the test wire reads high, mark a second player as connected
		if((GPIOH->IDR & 0x1) == 0x1)
		{
			if(!idSelPrepped)
			{
				PrepIDSel();
				GameState = e_IDSEL;
			}
			playerConnected = 1;
		}
		else
		{
			idSelPrepped = 0;
			playerConnected = 0;
			PlayerID = e_IDP1;
		}
	}
}


