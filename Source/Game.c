/* *************************************
 * 	Includes
 * *************************************/

#include "Game.h"

/* *************************************
 * 	Defines
 * *************************************/

#define TILE_MIRROR_FLAG ( (uint8_t)0x80 )
#define BALL_ONE_INIT_POS (fix16_from_int(0))
#define BALL_TWO_INIT_POS (fix16_from_int(256))
#define BALL_RADIUS (fix16_from_int(16))
#define WIND_SLOTS 4

/* **************************************
 * 	Structs and enums					*
 * *************************************/

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
static void GameRenderWaves(void);
static void GameClock(void);
static void GameRenderBall(TYPE_PLAYER * ptrPlayer);

/* *************************************
 * 	Global Variables
 * *************************************/

bool GameStartupFlag;
// Instances for player-specific data
TYPE_PLAYER PlayerData[MAX_PLAYERS];
// Instances for wave-specific data
TYPE_WAVE WaveData[MAX_WAVES];
TYPE_WAVE WaveSecondRowData[MAX_SECONDROW_WAVES];

/* *************************************
 * 	Local Variables
 * *************************************/

static GsSprite PlayerOneBall;
static GsSprite PlayerTwoBall;
static GsSprite ParallaxSpr;

static char * GameFileList[] = {"cdrom:\\DATA\\SPRITES\\BALL_01.TIM;1"	,
								"cdrom:\\DATA\\SPRITES\\BALL_02.TIM;1"	,
								"cdrom:\\DATA\\SPRITES\\PARALLAX.TIM;1"	};

static void * GameFileDest[] = {(GsSprite*)&PlayerOneBall,
								(GsSprite*)&PlayerTwoBall,
								(GsSprite*)&ParallaxSpr	 };

//Game local time
static uint8_t GameMinutes;
static uint8_t GameSeconds;
static bool timeout_flag;

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
	uint8_t i;

	GameStartupFlag = true;

	LoadMenu(	GameFileList,
				GameFileDest,
				sizeof(GameFileList) / sizeof(char*),
				sizeof(GameFileDest) /sizeof(void*)	);

	GameLoadLevel();

	GameGuiInit();

	PlayerData[PLAYER_ONE].position.x = BALL_ONE_INIT_POS;
	PlayerData[PLAYER_ONE].position.y = fix16_from_int(128); //TEST, remove ASAP
	PlayerData[PLAYER_ONE].PadKeyPressed_Callback = &PadOneKeyPressed;
	PlayerData[PLAYER_ONE].PadKeyReleased_Callback = &PadOneKeyReleased;
	PlayerData[PLAYER_ONE].PadDirectionKeyPressed_Callback = &PadOneDirectionKeyPressed;
	PlayerData[PLAYER_ONE].ptrSprite = &PlayerOneBall;
	PlayerData[PLAYER_ONE].radius = BALL_RADIUS;
	PlayerData[PLAYER_ONE].StateOnWater = true;
	PlayerData[PLAYER_ONE].wind_slots = WIND_SLOTS;

	PlayerData[PLAYER_TWO].position.x = BALL_TWO_INIT_POS;
	PlayerData[PLAYER_TWO].position.y = fix16_from_int(128); //TEST, remove ASAP
	PlayerData[PLAYER_TWO].PadKeyPressed_Callback = &PadTwoKeyPressed;
	PlayerData[PLAYER_TWO].PadKeyReleased_Callback = &PadTwoKeyReleased;
	PlayerData[PLAYER_TWO].PadDirectionKeyPressed_Callback = &PadTwoDirectionKeyPressed;
	PlayerData[PLAYER_TWO].ptrSprite = &PlayerTwoBall;
	PlayerData[PLAYER_TWO].radius = BALL_RADIUS;
	PlayerData[PLAYER_TWO].StateOnWater = true;
	PlayerData[PLAYER_TWO].wind_slots = WIND_SLOTS;

	CameraInit();

	GfxSetGlobalLuminance(0);

	track = SystemRand(GAMEPLAY_FIRST_TRACK,GAMEPLAY_LAST_TRACK);

	timeout_flag = false;

	for(i = 0; i < MAX_WAVES; i++)
	{
		WaveData[i].position.x = fix16_from_int(i << 6); // i * 64
		
		WaveData[i].min_value = 240 - 32;
		WaveData[i].max_value = Y_SCREEN_RESOLUTION - 16;
		
		WaveData[i].position.y = fix16_from_int(SystemRand(240 - 32 , Y_SCREEN_RESOLUTION - 16) );
		
		dprintf("[%d] = {%d, %d}, max = {%d, %d}\n",
				i,
				fix16_to_int(WaveData[i].position.x),
				fix16_to_int(WaveData[i].position.y),
				fix16_to_int(WaveData[i].min_value),
				fix16_to_int(WaveData[i].max_value)	);
				
		WaveData[i].decrease = SystemRand(false, true);
	}
	
	for(i = 0; i < MAX_SECONDROW_WAVES; i++)
	{
		WaveSecondRowData[i].position.x = fix16_from_int(i << 5); // i * 32
		
		WaveSecondRowData[i].min_value = 240 - 48;
		WaveSecondRowData[i].max_value = Y_SCREEN_RESOLUTION - 32;
		
		WaveSecondRowData[i].position.y = fix16_from_int(SystemRand(240 - 48 , Y_SCREEN_RESOLUTION - 32));
		
		dprintf("[%d] = {%d, %d}, max = {%d, %d}\n",
				i,
				fix16_to_int(WaveSecondRowData[i].position.x),
				fix16_to_int(WaveSecondRowData[i].position.y),
				fix16_to_int(WaveSecondRowData[i].min_value),
				fix16_to_int(WaveSecondRowData[i].max_value)	);
				
		WaveSecondRowData[i].decrease = SystemRand(false, true);
	}

	LoadMenuEnd();

	GameSetTime(2,30 /* TODO: Set time by macros?? */);

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
	
	GamePhysicsCheckCollisions();
	//GamePhysicsResolveCollisions(); // CHECK HERE!

	for(i = 0 ; i < MAX_PLAYERS ; i++)
	{
		// Run player-specific functions for each player
		GamePlayerHandler(&PlayerData[i]);
		GamePhysicsBallHandler(&PlayerData[i]);
	}

	for(i = 0; i < MAX_WAVES; i++)
	{
		GamePhysicsWaveHandler(&WaveData[i]);
	}
	
	for(i = 0; i < MAX_SECONDROW_WAVES; i++)
	{
		GamePhysicsWaveHandler(&WaveSecondRowData[i]);
	}

	CameraHandler(&PlayerData[PLAYER_ONE], &PlayerData[PLAYER_TWO]);
}

void GamePlayerHandler(TYPE_PLAYER * ptrPlayer)
{

}

void GameClock(void)
{
	if(System1SecondTick() == true)
	{
		if((--GameSeconds) == 0)
		{
			GameSeconds = 60;

			if(GameMinutes > 0)
			{
				GameMinutes--;
			}
			else
			{
				// Time out!!!
				timeout_flag = true;
			}
		}
	}
}

void GameGraphics(void)
{
	uint8_t i;

	while(	(GfxIsGPUBusy() == true)
					||
			(SystemRefreshNeeded() == false)	);
			
	ParallaxSpr.x = 0;
	ParallaxSpr.y = 0;
	ParallaxSpr.r = NORMAL_LUMINANCE;
	ParallaxSpr.g = NORMAL_LUMINANCE;
	ParallaxSpr.b = NORMAL_LUMINANCE;
	
	GsSortCls(0,0,0);

	GfxSortSprite(&ParallaxSpr);

	GameRenderWaves();

	for(i = 0; i < MAX_PLAYERS; i++)
	{
		GameRenderBall(&PlayerData[i]);
		GameGuiBeachSign(&PlayerData[i], i);
	}

	GameGuiClock(GameMinutes, GameSeconds);

	CameraDrawTarget();

	GfxDrawScene();
}

void GameLoadLevel(void)
{

}


void GameRenderWaves(void)
{
	uint8_t i;
	uint8_t j;
	GsGPoly4 WaveGPoly4;

	bzero((GsGPoly4*)&WaveGPoly4, sizeof(GsGPoly4)); // Reset data
	
	for(i = 0; i < MAX_SECONDROW_WAVES -1 ; i++)
	{
		WaveGPoly4.x[0] = (short)fix16_to_int(WaveSecondRowData[i].position.x);
		WaveGPoly4.x[1] = (short)fix16_to_int(WaveSecondRowData[i + 1].position.x);
		
		WaveGPoly4.x[2] = WaveGPoly4.x[0];
		WaveGPoly4.x[3] = (short)fix16_to_int(WaveSecondRowData[i + 1].position.x);
		
		WaveGPoly4.y[0] = (short)fix16_to_int(WaveSecondRowData[i].position.y);
		WaveGPoly4.y[1] = (short)fix16_to_int(WaveSecondRowData[i + 1].position.y);
		
		WaveGPoly4.y[2] = Y_SCREEN_RESOLUTION;
		WaveGPoly4.y[3] = Y_SCREEN_RESOLUTION;
		
		for(j = 0; j < 2; j++)
		{
			WaveGPoly4.r[j] = NORMAL_LUMINANCE >> 1;
			WaveGPoly4.g[j] = NORMAL_LUMINANCE >> 1;
		}
		
		for(j = 2; j < 4; j++)
		{
			WaveGPoly4.r[j] = NORMAL_LUMINANCE >> 2;
			WaveGPoly4.g[j] = NORMAL_LUMINANCE >> 2;
		}
		
		for(j = 0; j < 4; j++)
		{
			WaveGPoly4.b[j] = NORMAL_LUMINANCE >> 1;
		}
		
		CameraApplyCoordinatesToGsGPoly4(&WaveGPoly4);
		GfxSortGsGPoly4(&WaveGPoly4);
	}
	
	for(i = 0; i < MAX_WAVES -1 ; i++)
	{
		WaveGPoly4.x[0] = (short)fix16_to_int(WaveData[i].position.x);
		WaveGPoly4.x[1] = (short)fix16_to_int(WaveData[i + 1].position.x);

		WaveGPoly4.x[2] = WaveGPoly4.x[0];
		WaveGPoly4.x[3] = (short)fix16_to_int(WaveData[i + 1].position.x);

		WaveGPoly4.y[0] = (short)fix16_to_int(WaveData[i].position.y);
		WaveGPoly4.y[1] = (short)fix16_to_int(WaveData[i + 1].position.y);

		WaveGPoly4.y[2] = Y_SCREEN_RESOLUTION;
		WaveGPoly4.y[3] = Y_SCREEN_RESOLUTION;

		for(j = 0; j < 2; j++)
		{
			WaveGPoly4.r[j] = NORMAL_LUMINANCE >> 1;
			WaveGPoly4.g[j] = NORMAL_LUMINANCE >> 1;
		}

		for(j = 2; j < 4; j++)
		{
			WaveGPoly4.r[j] = NORMAL_LUMINANCE >> 2;
			WaveGPoly4.g[j] = NORMAL_LUMINANCE >> 2;
		}

		for(j = 0; j < 4; j++)
		{
			WaveGPoly4.b[j] = NORMAL_LUMINANCE;
		}

		CameraApplyCoordinatesToGsGPoly4(&WaveGPoly4);
		GfxSortGsGPoly4(&WaveGPoly4);
	}
}

void GameSetTime(uint8_t minutes, uint8_t seconds)
{
	GameMinutes = minutes;
	GameSeconds = seconds;
}

void GameRenderBall(TYPE_PLAYER * ptrPlayer)
{
	short final_x = (short)fix16_to_int(ptrPlayer->position.x - (ptrPlayer->radius));
	short final_y = (short)fix16_to_int(ptrPlayer->position.y - (ptrPlayer->radius));

	ptrPlayer->ptrSprite->x = final_x;
	ptrPlayer->ptrSprite->y = final_y;

	CameraApplyCoordinatesToSprite(ptrPlayer->ptrSprite);

	GfxSortSprite(ptrPlayer->ptrSprite);
}
