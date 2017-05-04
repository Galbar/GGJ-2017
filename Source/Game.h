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
#include "GamePhysics.h"
#include "Menu.h"

/* *************************************
 * 	Defines
 * *************************************/

#define PLAYER_ONE 0
#define PLAYER_TWO 1
#define MAX_PLAYERS (PLAYER_TWO + 1)
#define MAX_WAVES 13
#define MAX_SECONDROW_WAVES (MAX_WAVES * 2)

#define TILE_SIZE 64
#define LEVEL_TITLE_SIZE 32
#define TILE_SIZE_BIT_SHIFT 6

#define LEVEL_X_SIZE 768
#define LEVEL_Y_SIZE 240

/* *************************************
 * 	Global variables
 * *************************************/

extern bool GameStartupFlag;
extern TYPE_PLAYER PlayerData[];
extern TYPE_WAVE WaveData[];
extern SsVag SplashSnd;
extern SsVag DeathSnd;

/* *************************************
 * 	Global prototypes
 * *************************************/

void 		Game(void);
char * 		GetGameLevelTitle(void);
void 		GameSetTime(uint8_t minutes, uint8_t seconds);

#endif //__GAME_HEADER__
