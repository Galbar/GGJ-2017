/* *************************************
 * 	Includes
 * *************************************/

#include "GameGui.h"

/* *************************************
 * 	Defines
 * *************************************/

/* **************************************
 * 	Structs and enums					*
 * *************************************/
 
enum
{
	PAUSE_DIALOG_X = 48,
	PAUSE_DIALOG_Y = 32,
	PAUSE_DIALOG_W = 200,
	PAUSE_DIALOG_H = 184,
	
	PAUSE_DIALOG_R0 = 0,
	PAUSE_DIALOG_R1 = PAUSE_DIALOG_R0,
	PAUSE_DIALOG_R2 = 0,
	PAUSE_DIALOG_R3 = PAUSE_DIALOG_R2,
	
	PAUSE_DIALOG_G0 = NORMAL_LUMINANCE,
	PAUSE_DIALOG_G1 = PAUSE_DIALOG_G0,
	PAUSE_DIALOG_G2 = 0,
	PAUSE_DIALOG_G3 = PAUSE_DIALOG_G2,
	
	PAUSE_DIALOG_B0 = 40,
	PAUSE_DIALOG_B1 = PAUSE_DIALOG_B0,
	PAUSE_DIALOG_B2 = 0,
	PAUSE_DIALOG_B3 = PAUSE_DIALOG_B2,
};


enum
{
	CLOCK_X = (X_SCREEN_RESOLUTION >> 1) - 48,
	CLOCK_Y = 8
};

enum
{
	GAME_GUI_SECOND_DISPLAY_X = 384,
	GAME_GUI_SECOND_DISPLAY_Y = 256,
	GAME_GUI_SECOND_DISPLAY_TPAGE = 22,
};

enum
{
	GAME_GUI_WIND_SLOT_PLAYER_ONE_X = 16,
	GAME_GUI_WIND_SLOT_PLAYER_ONE_Y = 24,
	
	GAME_GUI_WIND_SLOT_PLAYER_TWO_X = X_SCREEN_RESOLUTION - 104,
	GAME_GUI_WIND_SLOT_PLAYER_TWO_Y = 24,
	
	GAME_GUI_WIND_SLOT_PLAYER_W = 16,
	GAME_GUI_WIND_SLOT_PLAYER_H = 8
};

enum
{
	GAME_GUI_LIFES_PLAYER_ONE_X = 16,
	GAME_GUI_LIFES_PLAYER_ONE_Y = 48,
	
	GAME_GUI_LIFES_PLAYER_TWO_X = X_SCREEN_RESOLUTION - 72,
	GAME_GUI_LIFES_PLAYER_TWO_Y = 48,
	
	GAME_GUI_LIFES_PLAYER_W = 16,
	GAME_GUI_LIFES_PLAYER_H = 8
};

/* **************************************
 * 	Local prototypes					*
 * *************************************/

/* **************************************
 * 	Local variables						*
 * *************************************/

static GsSprite ArrowsSpr;
static GsGPoly4 PauseRect;
static GsSprite SecondDisplay;

static GsSprite LeftArrowSpr;
static GsSprite RightArrowSpr;

static GsSprite PauseSpr;

static GsSprite TimeTableSpr;

static GsSprite WindSlotSpr[MAX_PLAYERS];

static GsSprite HeartSpr;

static char * GameFileList[] = {"cdrom:\\DATA\\SPRITES\\ARROWS.TIM;1"	,
								"cdrom:\\DATA\\SPRITES\\LEFTARR.TIM;1"	,
								"cdrom:\\DATA\\SPRITES\\RIGHTARR.TIM;1"	,
								"cdrom:\\DATA\\SPRITES\\PAUSE.TIM;1"	,
								"cdrom:\\DATA\\SPRITES\\TIMETBL.TIM;1"	,
								"cdrom:\\DATA\\SPRITES\\WINDPL1.TIM;1"	,
								"cdrom:\\DATA\\SPRITES\\WINDPL2.TIM;1"	,
								"cdrom:\\DATA\\SPRITES\\HEART.TIM;1"	};
								
static void * GameFileDest[] = {(GsSprite*)&ArrowsSpr				,
								(GsSprite*)&LeftArrowSpr			,
								(GsSprite*)&RightArrowSpr			,
								(GsSprite*)&PauseSpr				,
								(GsSprite*)&TimeTableSpr			,	
								(GsSprite*)&WindSlotSpr[PLAYER_ONE]	,	
								(GsSprite*)&WindSlotSpr[PLAYER_TWO]	,	
								(GsSprite*)&HeartSpr				};	

void GameGuiInit(void)
{
	static bool first_run = true;
	
	if(first_run == true)
	{
		first_run = false;
		
		LoadMenu(	GameFileList,
					GameFileDest,
					sizeof(GameFileList) / sizeof(char*),
					sizeof(GameFileDest) /sizeof(void*)	);
	}
	
	PauseRect.attribute |= ENABLE_TRANS | TRANS_MODE(0);
}

bool GameGuiPauseDialog(TYPE_PLAYER * ptrPlayer)
{

	GfxSaveDisplayData(&SecondDisplay);
				
	DrawFBRect(0, 0, X_SCREEN_RESOLUTION, VRAM_H, 0, 0, 0);
				
	while(GfxIsGPUBusy() == true); // After DrawFBRect, we should wait for GPU to finish
	
	do
	{
		if(ptrPlayer->PadKeyReleased_Callback(PAD_CROSS) == true)
		{
			return true;
		}
		
		GfxSortSprite(&SecondDisplay);
		
		PauseSpr.attribute |= GFX_2HZ_FLASH;
		
		PauseSpr.x = PAUSE_DIALOG_X;
		PauseSpr.y = PAUSE_DIALOG_Y;
		
		GfxSortSprite(&PauseSpr);
		
		RadioFont.spr.r = 0;
		RadioFont.spr.g = 0;
		RadioFont.spr.b = 0;
		
		FontPrintText(&RadioFont, 48, 128, "Press X to exit\nSTART to resume");
		
		RadioFont.spr.r = NORMAL_LUMINANCE;
		RadioFont.spr.g = NORMAL_LUMINANCE;
		RadioFont.spr.b = NORMAL_LUMINANCE;
		
		GfxDrawScene_Slow();
		
	}while(ptrPlayer->PadKeyReleased_Callback(PAD_START) == false);
	
	return false;
}

void GameGuiClock(uint8_t min, uint8_t sec)
{
	static char strClock[6]; // HH:MM + \0 (6 characters needed)
	
	if(GameStartupFlag || System1SecondTick() == true)
	{
		memset(strClock, 0, 6);
		snprintf(strClock,6,"%02d:%02d",min, sec);
	}
	
	TimeTableSpr.x = CLOCK_X;
	TimeTableSpr.y = 0;
	
	GfxSortSprite(&TimeTableSpr);
	
	RadioFont.flags = FONT_NOFLAGS;
	RadioFont.max_ch_wrap = 0;
	FontPrintText(&RadioFont,CLOCK_X + 6,CLOCK_Y - 8,strClock);
}

void GameGuiBeachSign(TYPE_PLAYER * ptrPlayer, uint8_t i)
{
	static bool reminder[MAX_PLAYERS] = {false, false};
	static int movement_angle = 0x168000;
	static uint8_t movement_index[MAX_PLAYERS];
		
	if(GfxIsSpriteInsideScreenArea(ptrPlayer->ptrSprite) == false)
	{
		if(reminder[i] == false)
		{
			reminder[i] = true;
			movement_index[i] = 0;
		}
		
		movement_angle = 0x168000 + BeachSignAni[movement_index[i]];
		
		if( (movement_index[i] + 1) < BEACH_SIGN_ANI_SIZE)
		{
			movement_index[i]++;
		}
		
		RightArrowSpr.mx = 22;
		RightArrowSpr.my = 2;
		
		LeftArrowSpr.mx = 25;
		LeftArrowSpr.my = 4;
		
		LeftArrowSpr.x = 16;
		LeftArrowSpr.y = fix16_to_int(ptrPlayer->position.y) - 34;
		LeftArrowSpr.rotate = movement_angle;
		
		RightArrowSpr.x = X_SCREEN_RESOLUTION - RightArrowSpr.w - 16;
		RightArrowSpr.y = fix16_to_int(ptrPlayer->position.y) - 34;
		RightArrowSpr.rotate = movement_angle;
		
		if(ptrPlayer->ptrSprite->x < 0)
		{
			GfxSortSprite(&LeftArrowSpr);
		}
		else if(ptrPlayer->ptrSprite->x > X_SCREEN_RESOLUTION)
		{
			GfxSortSprite(&RightArrowSpr);
		}
		
	}
	else
	{
		reminder[i] = false;
		movement_index[i] = 0;
	}
	
}

void GameGuiWindSlots(TYPE_PLAYER * ptrPlayer, uint8_t id)
{
	uint8_t i;
	short x;
	short y;
	
	if(id == PLAYER_ONE)
	{
		x = GAME_GUI_WIND_SLOT_PLAYER_ONE_X;
		y = GAME_GUI_WIND_SLOT_PLAYER_ONE_Y;
	}
	else if(id == PLAYER_TWO)
	{
		x = GAME_GUI_WIND_SLOT_PLAYER_TWO_X;
		y = GAME_GUI_WIND_SLOT_PLAYER_TWO_Y;
	}
	else
	{
		dprintf("Invalid player ID!\n");
		return;
	}
	

	WindSlotSpr[id].x = x;
	WindSlotSpr[id].y = y;
	
	for(i = 0; i < ptrPlayer->wind_slots; i++)
	{
		WindSlotSpr[id].x = x + (i * (WindSlotSpr[id].w + 2));
		GfxSortSprite(&WindSlotSpr[id]);
	}
}

void GameGuiLifes(TYPE_PLAYER * ptrPlayer, uint8_t id)
{
	uint8_t i;
	short x;
	short y;
	
	if(id == PLAYER_ONE)
	{
		x = GAME_GUI_LIFES_PLAYER_ONE_X;
		y = GAME_GUI_LIFES_PLAYER_ONE_Y;
	}
	else if(id == PLAYER_TWO)
	{
		x = GAME_GUI_LIFES_PLAYER_TWO_X;
		y = GAME_GUI_LIFES_PLAYER_TWO_Y;
	}
	else
	{
		dprintf("Invalid player ID!\n");
		return;
	}

	HeartSpr.x = x;
	HeartSpr.y = y;
	
	for(i = 0; i < ptrPlayer->lifes_left; i++)
	{
		HeartSpr.x = x + (i * (HeartSpr.w + 2));
		GfxSortSprite(&HeartSpr);
	}
}
