#ifndef SIMONBASE_H
#define SIMONBASE_H

#include "stm32l476xx.h"

enum GameStates{
	e_MENU,
	e_COUNTDOWN,
	e_IDSEL,
	e_1P,
	e_2P_ENDLESS,
	e_2P_VS,
	e_GAMEOVER
};

enum CDSubStates{
	e_CDSUB_READY,
	e_CDSUB_SET,
	e_CDSUB_GO
};

enum MenuSubStates{
	e_MENUSUB_1P,
	e_MENUSUB_2P_ENDLESS,
	e_MENUSUB_2P_VS
};

enum PlayerIDs{
	e_IDP1,
	e_IDP2
};

enum GameOverSubStates{
	e_GOSUB_ENDLESS_WINNER,
	e_GOSUB_VS_WINNER,
	e_GOSUB_ENDLESS_LOSER,
	e_GOSUB_VS_LOSER,
};

enum VSSubStates{
	e_VSSUB_LEVEL1,
	e_VSSUB_LEVEL2,
	e_VSSUB_LEVEL3,
	e_VSSUB_GENNING,
	e_VSSUB_WAITING
};
/*
enum States{
	e_MENU,
	e_COUNTDOWN,
	e_IDSel,
	e_1P,
	e_2P,
	e_2PVS,
	e_GAMEOVER
};

enum CDStates{
	e_READY,
	e_SET,
	e_GO
};

enum MenuOptions{
	e_1PLAYER,
	e_VS,
	e_2PLAYER
};

enum PlayerIDOptions{
	e_IDP1,
	e_IDP2
};

enum PlayerIDMenuOptions{
	e_IDMenu1,
	e_IDMenu2
};

enum GameOverOptions{
	e_WINNER,
	e_LOSER
};

enum VSModeStates{
	e_LEVEL1,
	e_LEVEL2,
	e_LEVEL3
};
*/

//////////////////////////////////// FUNCTIONS ////////////////////////////////////////////

void updateLCDandBars(uint8_t *str, char bars[4]);
uint8_t genInput(uint8_t *inputDesired, char *lives);
uint8_t genInputNoDisplay(const uint8_t *inputDesired, const char *lives);
void DisplayDesiredInput(char input, char *lives);
void rotateString(uint8_t *str, int strLen);
void resetString(uint8_t *base, uint8_t *rotator, int len);
void JoystickInit(void);
void sendInput(char input);
char receiveInput(void);

#endif /* SIMONBASE_H */