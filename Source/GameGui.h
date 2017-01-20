#ifndef __GAME_GUI_HEADER__
#define __GAME_GUI_HEADER__

/* *************************************
 * 	Includes
 * *************************************/

#include "Global_Inc.h"
#include "System.h"
#include "GameStructures.h"
#include "Gfx.h"

/* *************************************
 * 	Defines
 * *************************************/

/* *************************************
 * 	Structs and enums
 * *************************************/

/* *************************************
 * 	Global variables
 * *************************************/

/* *************************************
 * 	Global prototypes
 * *************************************/

void GameGuiInit(void);
bool GameGuiPauseDialog(TYPE_PLAYER * ptrPlayer);
void GameGuiClock(uint8_t hour, uint8_t min);

#endif //__GAME_GUI_HEADER__
