#ifndef __GAME_GUI_HEADER__
#define __GAME_GUI_HEADER__

/* *************************************
 * 	Includes
 * *************************************/

#include "Global_Inc.h"
#include "System.h"
#include "GameStructures.h"
#include "Gfx.h"
#include "BeachSignAni.h"

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
void GameGuiClock(uint8_t min, uint8_t sec);
void GameGuiBeachSign(TYPE_PLAYER * ptrPlayer, uint8_t i);
void GameGuiWindSlots(TYPE_PLAYER * ptrPlayer, uint8_t id);

#endif //__GAME_GUI_HEADER__
