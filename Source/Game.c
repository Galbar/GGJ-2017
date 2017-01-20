/* *************************************
 * 	Includes
 * *************************************/

#include "Game.h"

/* *************************************
 * 	Defines
 * *************************************/

#define TILE_MIRROR_FLAG ( (uint8_t)0x80 )

/* **************************************
 * 	Structs and enums					*
 * *************************************/

enum
{
	MOUSE_W = 8,
	MOUSE_H = 8,
	MOUSE_X = X_SCREEN_RESOLUTION - (X_SCREEN_RESOLUTION >> 1),
	MOUSE_Y = Y_SCREEN_RESOLUTION - (Y_SCREEN_RESOLUTION >> 1),
};

/* *************************************
 * 	Local Prototypes
 * *************************************/

static void GameInit(void);
static void GameLoadLevel(void);
static bool GamePause(void);
static void GameEmergencyMode(void);
static void GameCalculations(void);
static void GamePlayerHandler(TYPE_PLAYER * ptrPlayer);
static void GameGraphics(void);
static void GameRenderLevel(void);
static void GameClock(void);

/* *************************************
 * 	Global Variables
 * *************************************/

bool GameStartupFlag;
// Instances for player-specific data
TYPE_PLAYER PlayerData[MAX_PLAYERS];
// Instances for wave-specific data
TYPE_WAVE WaveData[MAX_WAVES];

/* *************************************
 * 	Local Variables
 * *************************************/

static GsSprite GameMouseSpr;


static char * GameFileList[] = {	"cdrom:\\DATA\\SPRITES\\MOUSE.TIM;1"	};

static void * GameFileDest[] = {	(GsSprite*)&GameMouseSpr		};

//static char GameLevelTitle[LEVEL_TITLE_SIZE];

//Game local time
static uint8_t GameHour;
static uint8_t GameMinutes;

void Game(void)
{
	GameInit();

	while(1)
	{
		if(GamePause() == true)
		{
			// Exit game
			break;
		}

		GameEmergencyMode();

		GameCalculations();

		GameGraphics();

		if(GameStartupFlag == true)
		{
			GameStartupFlag = false;
		}
	}

	EndAnimation();

	SfxPlayTrack(INTRO_TRACK);
}

bool GamePause(void)
{
	TYPE_PLAYER * ptrPlayer;
	uint8_t i;
	bool pause_flag = false;

	if(GameStartupFlag == true)
	{
		return false;
	}

	for(i = 0 ; i < MAX_PLAYERS ; i++)
	{
		ptrPlayer = &PlayerData[i];
		// Run player-specific functions for each player
		//dprintf("Released callback = 0x%08X\n", ptrPlayer->PadKeyReleased_Callback);
		if(ptrPlayer->PadKeyReleased_Callback(PAD_START) == true)
		{
			dprintf("Player %d set pause_flag to true!\n",i);
			pause_flag = true;
			break;
		}
	}

	if(pause_flag == true)
	{
		// Blocking function:
		// 	* Returns true if player pointed to by ptrPlayer wants to exit game
		//	* Returns false if player pointed to by ptrPlayer wants to resume game
		return GameGuiPauseDialog(ptrPlayer);
	}

	return false;
}

void GameInit(void)
{
	uint32_t track;

	GameStartupFlag = true;

	LoadMenu(	GameFileList,
				GameFileDest,
				sizeof(GameFileList) / sizeof(char*),
				sizeof(GameFileDest) /sizeof(void*)	);

	GameLoadLevel();

	GameGuiInit();

	PlayerData[PLAYER_ONE].PadKeyPressed_Callback = &PadOneKeyPressed;
	PlayerData[PLAYER_ONE].PadKeyReleased_Callback = &PadOneKeyReleased;
	PlayerData[PLAYER_ONE].PadDirectionKeyPressed_Callback = &PadOneDirectionKeyPressed;

	PlayerData[PLAYER_TWO].PadKeyPressed_Callback = &PadTwoKeyPressed;
	PlayerData[PLAYER_TWO].PadKeyReleased_Callback = &PadTwoKeyReleased;
	PlayerData[PLAYER_TWO].PadDirectionKeyPressed_Callback = &PadTwoDirectionKeyPressed;

	CameraInit();

	GfxSetGlobalLuminance(0);

	track = SystemRand(GAMEPLAY_FIRST_TRACK,GAMEPLAY_LAST_TRACK);

	LoadMenuEnd();

	SfxPlayTrack(track);
}

void GameEmergencyMode(void)
{
	enum
	{
		ERROR_RECT_X = 32,
		ERROR_RECT_W = X_SCREEN_RESOLUTION - (ERROR_RECT_X << 1),

		ERROR_RECT_Y = 16,
		ERROR_RECT_H = Y_SCREEN_RESOLUTION - (ERROR_RECT_Y << 1),

		ERROR_RECT_R = 0,
		ERROR_RECT_G = 32,
		ERROR_RECT_B = NORMAL_LUMINANCE
	};

	GsRectangle errorRct;

	bzero((GsRectangle*)&errorRct, sizeof(GsRectangle));

	while(SystemGetEmergencyMode() == true)
	{
		// Pad one has been disconnected during gameplay
		// Show an error screen until it is disconnected again.

		GsSortCls(0,0,0);

		errorRct.x = ERROR_RECT_X;
		errorRct.w = ERROR_RECT_W;
		errorRct.y = ERROR_RECT_Y;
		errorRct.h = ERROR_RECT_H;

		errorRct.r = ERROR_RECT_R;
		errorRct.g = ERROR_RECT_G;
		errorRct.b = ERROR_RECT_B;

		GsSortRectangle(&errorRct);
		GfxDrawScene();
	}
}

void GameCalculations(void)
{
	uint8_t i;

	GameClock();

	for(i = 0 ; i < MAX_PLAYERS ; i++)
	{
		// Run player-specific functions for each player
		GamePlayerHandler(&PlayerData[i]);
	}
}

void GamePlayerHandler(TYPE_PLAYER * ptrPlayer)
{

}

void GameClock(void)
{
	if(System1SecondTick() == true)
	{
		GameMinutes++;

		if(GameMinutes >= 60)
		{
			GameHour++;
			GameMinutes = 0;
		}

		if(GameHour >= 24)
		{
			GameHour = 0;
		}
	}
}

void GameGraphics(void)
{
	while(	(GfxIsGPUBusy() == true)
					||
			(SystemRefreshNeeded() == false)	);

	GameRenderLevel();

	GameGuiClock(GameHour,GameMinutes);

	GfxDrawScene();
}

void GameLoadLevel(void)
{

}


void GameRenderLevel(void)
{

}

void GameSetTime(uint8_t hour, uint8_t minutes)
{
	GameHour = hour;
	GameMinutes = minutes;
}
