#ifndef __GAME_HEADER__
#define __GAME_HEADER__

/* *************************************
 * 	Includes
 * *************************************/

#include "Global_Inc.h"
#include "GameStructures.h"
#include "LoadMenu.h"
#include "System.h"
#include "Camera.h"
#include "GameGui.h"

/* *************************************
 * 	Defines
 * *************************************/

#define PLAYER_ONE 0
#define PLAYER_TWO 1
#define MAX_PLAYERS (PLAYER_TWO + 1)

#define TILE_SIZE 64
#define LEVEL_TITLE_SIZE 32
#define TILE_SIZE_BIT_SHIFT 6

/* *************************************
 * 	Global variables
 * *************************************/

extern bool GameStartupFlag;
extern TYPE_PLAYER PlayerData[];

/* *************************************
 * 	Global prototypes
 * *************************************/

void 		Game(bool two_players);
char * 		GetGameLevelTitle(void);
void 		GameSetTime(uint8_t hour, uint8_t minutes);

#endif //__GAME_HEADER__
