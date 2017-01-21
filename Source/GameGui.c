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
	PAUSE_DIALOG_X = 92,
	PAUSE_DIALOG_Y = 28,
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
	CLOCK_Y = 16
};

enum
{
	GAME_GUI_SECOND_DISPLAY_X = 384,
	GAME_GUI_SECOND_DISPLAY_Y = 256,
	GAME_GUI_SECOND_DISPLAY_TPAGE = 22,
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

static char * GameFileList[] = {"cdrom:\\DATA\\FONTS\\FONT_1.FNT;1"		,
								"cdrom:\\DATA\\SPRITES\\ARROWS.TIM;1"	,
								"cdrom:\\DATA\\SPRITES\\LEFTARR.TIM;1"	,
								"cdrom:\\DATA\\SPRITES\\RIGHTARR.TIM;1"	};
								
static void * GameFileDest[] = {(TYPE_FONT*)&RadioFont		,
								(GsSprite*)&ArrowsSpr		,
								(GsSprite*)&LeftArrowSpr	,
								(GsSprite*)&RightArrowSpr	};

void GameGuiInit(void)
{
	LoadMenu(	GameFileList,
				GameFileDest,
				sizeof(GameFileList) / sizeof(char*),
				sizeof(GameFileDest) /sizeof(void*)	);
				
	PauseRect.x[0] = PAUSE_DIALOG_X;
	PauseRect.x[1] = PAUSE_DIALOG_X + PAUSE_DIALOG_W;
	PauseRect.x[2] = PAUSE_DIALOG_X;
	PauseRect.x[3] = PAUSE_DIALOG_X + PAUSE_DIALOG_W;
	
	PauseRect.y[0] = PAUSE_DIALOG_Y;
	PauseRect.y[1] = PAUSE_DIALOG_Y;
	PauseRect.y[2] = PAUSE_DIALOG_Y + PAUSE_DIALOG_H;
	PauseRect.y[3] = PAUSE_DIALOG_Y + PAUSE_DIALOG_H;
	
	PauseRect.r[0] = PAUSE_DIALOG_R0;
	PauseRect.r[1] = PAUSE_DIALOG_R1;
	PauseRect.r[2] = PAUSE_DIALOG_R2;
	PauseRect.r[3] = PAUSE_DIALOG_R3;
	
	PauseRect.b[0] = PAUSE_DIALOG_B0;
	PauseRect.b[1] = PAUSE_DIALOG_B1;
	PauseRect.b[2] = PAUSE_DIALOG_B2;
	PauseRect.b[3] = PAUSE_DIALOG_B3;
	
	PauseRect.g[0] = PAUSE_DIALOG_G0;
	PauseRect.g[1] = PAUSE_DIALOG_G1;
	PauseRect.g[2] = PAUSE_DIALOG_G2;
	PauseRect.g[3] = PAUSE_DIALOG_G3;
	
	PauseRect.attribute |= ENABLE_TRANS | TRANS_MODE(0);
}

bool GameGuiPauseDialog(TYPE_PLAYER * ptrPlayer)
{
	bool show_pause_dialog = true;
	
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
		
		if(System1SecondTick() == true)
		{
			show_pause_dialog = show_pause_dialog ? false : true;
		}
		
		if(show_pause_dialog == true)
		{
			GsSortGPoly4(&PauseRect);
		}
		
		GfxDrawScene_Slow();
		
	}while(ptrPlayer->PadKeyReleased_Callback(PAD_START) == false);
	
	return false;
}

void GameGuiClock(uint8_t hour, uint8_t min)
{
	static char strClock[6]; // HH:MM + \0 (6 characters needed)
	
	if(GameStartupFlag || System1SecondTick() == true)
	{
		memset(strClock, 0, 6);
		snprintf(strClock,6,"%02d:%02d",hour, min);
	}
	
	RadioFont.flags = FONT_NOFLAGS;
	RadioFont.max_ch_wrap = 0;
	FontPrintText(&RadioFont,CLOCK_X,CLOCK_Y,strClock);
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
