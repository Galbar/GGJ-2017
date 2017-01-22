/* **************************************
 * 	Includes							*
 * *************************************/

#include "Menu.h"

/* **************************************
 * 	Defines								*
 * *************************************/

#define MAIN_MENU_FILES 1
#define BUTTON_SIZE 64
#define SELECTED_BUTTON_LUMINANCE 0xC0

/* **************************************
 * 	Structs and enums					*
 * *************************************/

typedef enum
{
	PLAY_CREDITS_LEVEL = 0,
	CREDITS_LEVEL
}MainMenuLevel;

enum
{
	MAIN_MENU_PLAY_CREDITS_LEVEL_BUTTONS = 2
};

typedef enum
{
	PLAY_BUTTON_INDEX = 0,
	CREDITS_BUTTON_INDEX,

	MAIN_MENU_BUTTONS_MAX
}MMBtn_Index;

enum //384x240
{
	MAIN_MENU_PLAY_BUTTON_X = 92,
	MAIN_MENU_PLAY_BUTTON_Y = 148,

	MAIN_MENU_CREDITS_BUTTON_X = 224,
	MAIN_MENU_CREDITS_BUTTON_Y = 148,
};

enum
{
	MAIN_MENU_LOGO_X = 96,
	MAIN_MENU_LOGO_Y = 0
};

typedef enum
{
	PLAY_BUTTON_U_OFFSET = 0,
	PLAY_BUTTON_Y_OFFSET = 256-64,

	CREDITS_BUTTON_U_OFFSET = PLAY_BUTTON_U_OFFSET + 64,
	CREDITS_BUTTON_Y_OFFSET = PLAY_BUTTON_Y_OFFSET,

	DEFAULT_BUTTON_U_OFFSET = 0,
	DEFAULT_BUTTON_V_OFFSET = 0

}MMBtn_Offset;

#pragma pack(1)
typedef struct
{
	MMBtn_Offset offset_u;
	MMBtn_Offset offset_v;

	// Timer for absolute, sine-like animation
	short timer;

	// Pointer to function executed by pressing the button
	void (*f)();

	MMBtn_Index i;

	bool selected;
	bool was_selected;

}TYPE_MMBtn;
#pragma pack()


/* **************************************
 * 	Global variables					*
 * **************************************/

GsSprite ParallaxSpr;

/* **************************************
 * 	Local prototypes					*
 * **************************************/

static void MainMenuDrawButton(TYPE_MMBtn * btn);
static void CreditsDraw(void);
static void PlayMenu(void);
static void CreditsMenu(void);
//static void OnePlayerMenu(void);
//static void TwoPlayerMenu(void);
static void MainMenuButtonHandler(void);
static void MainMenuRestoreInitValues(void);
static void MenuTestCheat(void);

/* **************************************
 * 	Local variables						*
 * **************************************/

static GsSprite MenuSpr;
static SsVag BellSnd;
static SsVag AcceptSnd;
static TYPE_CHEAT TestCheat;
static TYPE_CHEAT StackCheckCheat;
static GsSprite LogoSpr;

static char * MainMenuFiles[] = {	"cdrom:\\DATA\\SPRITES\\MAINMENU.TIM;1"	,
									"cdrom:\\DATA\\SOUNDS\\BELL.VAG;1"		,
									"cdrom:\\DATA\\SOUNDS\\ACCEPT.VAG;1"	,
									"cdrom:\\DATA\\SPRITES\\PSXDISK.TIM;1"	,
									"cdrom:\\DATA\\SPRITES\\INTROFNT.TIM;1"	,
									"cdrom:\\DATA\\SPRITES\\BUTTONS.TIM;1"	,
									"cdrom:\\DATA\\SPRITES\\GPL.TIM;1"		,
									"cdrom:\\DATA\\SPRITES\\OPENSRC.TIM;1"	,
									"cdrom:\\DATA\\SOUNDS\\TRAYCL.VAG;1"	,
									"cdrom:\\DATA\\SOUNDS\\SPINDISK.VAG;1"	,
									"cdrom:\\DATA\\SPRITES\\PARALLAX.TIM;1" ,
									"cdrom:\\DATA\\SPRITES\\LOGO.TIM;1" 	};

static void * MainMenuDest[] = {	(GsSprite*)&MenuSpr			,
									(SsVag*)&BellSnd			,
									(SsVag*)&AcceptSnd			,
									(GsSprite*)&PsxDisk			,
									(GsSprite*)&PSXSDKIntroFont	,
									(GsSprite*)&PSXButtons		,
									(GsSprite*)&GPL_Logo		,
									(GsSprite*)&OpenSource_Logo	,
									(SsVag*)&TrayClSnd			,
									(SsVag*)&SpinDiskSnd		,
									(GsSprite*)&ParallaxSpr		,
									(GsSprite*)&LogoSpr			};

static TYPE_MMBtn MainMenuBtn[MAIN_MENU_BUTTONS_MAX];
static MainMenuLevel menuLevel;
static MMBtn_Index MainMenuMinimumBtn;

void PlayMenu(void)
{
	EndAnimation();
	Game();
}

void CreditsMenu(void)
{
	menuLevel = CREDITS_LEVEL;

	MainMenuBtn[PLAY_BUTTON_INDEX].selected = false;
	MainMenuBtn[PLAY_BUTTON_INDEX].was_selected = false;
	MainMenuBtn[PLAY_BUTTON_INDEX].timer = 0;
	MainMenuBtn[CREDITS_BUTTON_INDEX].selected = false;
	MainMenuBtn[CREDITS_BUTTON_INDEX].was_selected = false;
	MainMenuBtn[CREDITS_BUTTON_INDEX].timer = 0;
}

void MainMenuInit(void)
{
	LoadMenu(	MainMenuFiles,MainMenuDest,
				sizeof(MainMenuFiles) / sizeof(char*) ,
				sizeof(MainMenuDest) / sizeof(void*) );

	MainMenuBtn[PLAY_BUTTON_INDEX].offset_u = PLAY_BUTTON_U_OFFSET;
	MainMenuBtn[PLAY_BUTTON_INDEX].offset_v = PLAY_BUTTON_Y_OFFSET;
	MainMenuBtn[PLAY_BUTTON_INDEX].timer = 0;
	MainMenuBtn[PLAY_BUTTON_INDEX].f = &PlayMenu;
	MainMenuBtn[PLAY_BUTTON_INDEX].i = PLAY_BUTTON_INDEX;

	MainMenuBtn[CREDITS_BUTTON_INDEX].offset_u = CREDITS_BUTTON_U_OFFSET;
	MainMenuBtn[CREDITS_BUTTON_INDEX].offset_v = CREDITS_BUTTON_Y_OFFSET;
	MainMenuBtn[CREDITS_BUTTON_INDEX].timer = 0;
	MainMenuBtn[CREDITS_BUTTON_INDEX].f = &CreditsMenu;
	MainMenuBtn[CREDITS_BUTTON_INDEX].i = CREDITS_BUTTON_INDEX;

	menuLevel = PLAY_CREDITS_LEVEL;

	MainMenuMinimumBtn = PLAY_BUTTON_INDEX;
	
	LogoSpr.x = MAIN_MENU_LOGO_X;
	LogoSpr.y = MAIN_MENU_LOGO_Y;
	
	LogoSpr.r = NORMAL_LUMINANCE;
	LogoSpr.g = NORMAL_LUMINANCE;
	LogoSpr.b = NORMAL_LUMINANCE;

	TestCheat.Callback = &MenuTestCheat;
	memset(TestCheat.Combination,0,CHEAT_ARRAY_SIZE);
	//memcpy(myarray, (int [5]){a,b,c,d,e}, 5*sizeof(int));

	memcpy(	TestCheat.Combination,
			(unsigned short[CHEAT_ARRAY_SIZE])
			{	PAD_CIRCLE, PAD_CIRCLE, PAD_CROSS, PAD_TRIANGLE,
				PAD_TRIANGLE, PAD_TRIANGLE, 0 , 0 ,
				0, 0, 0, 0,
				0, 0, 0, 0	} , sizeof(unsigned short) * CHEAT_ARRAY_SIZE);

	PadAddCheat(&TestCheat);

	StackCheckCheat.Callback = &SystemPrintStackPointerAddress;
	memset(StackCheckCheat.Combination, 0, CHEAT_ARRAY_SIZE);

	memcpy(	StackCheckCheat.Combination,
			(unsigned short[CHEAT_ARRAY_SIZE])
			{	PAD_TRIANGLE, PAD_TRIANGLE, PAD_CROSS, PAD_TRIANGLE,
				PAD_L1, PAD_R1, 0 , 0 ,
				0, 0, 0, 0,
				0, 0, 0, 0	} , sizeof(unsigned short) * CHEAT_ARRAY_SIZE);

	PadAddCheat(&StackCheckCheat);

	LoadMenuEnd();
}

void MainMenu(void)
{
	MainMenuInit();

	#ifndef NO_INTRO
	PSXSDKIntro();
	#endif //PSXSDK_DEBUG

	while(1)
	{
		MainMenuButtonHandler();
		
		ParallaxSpr.x = 0;
		ParallaxSpr.y = 0;
		
		ParallaxSpr.w = X_SCREEN_RESOLUTION;
		ParallaxSpr.h = Y_SCREEN_RESOLUTION;
		
		ParallaxSpr.tpage = 10;
		
		ParallaxSpr.u = 0;
		ParallaxSpr.v = 0;
		
		ParallaxSpr.r = NORMAL_LUMINANCE;
		ParallaxSpr.g = NORMAL_LUMINANCE;
		ParallaxSpr.b = NORMAL_LUMINANCE;
		
		GfxSetGlobalLuminance(NORMAL_LUMINANCE);

		switch(menuLevel)
		{
			case PLAY_CREDITS_LEVEL:
				GsSortCls(11,170,210);
				
				GfxSortSprite(&ParallaxSpr);
				
				MainMenuDrawButton(&MainMenuBtn[PLAY_BUTTON_INDEX]);
				MainMenuDrawButton(&MainMenuBtn[CREDITS_BUTTON_INDEX]);
				
				GfxSortSprite(&LogoSpr);

				GfxDrawScene_Slow();
			break;

			case CREDITS_LEVEL:
				GsSortCls(11,170,210); 	// RGB (11 170 210) is the colour of background sea
										// Easy way to extend background without sacrificing VRAM
				
				GfxSortSprite(&ParallaxSpr);
				
				CreditsDraw();
				
				GfxSortSprite(&LogoSpr);
				
				GfxDrawScene_Slow();
			break;

			default:
			break;
		}		
	}

}

void MainMenuRestoreInitValues(void)
{
	menuLevel = PLAY_CREDITS_LEVEL;
	MainMenuMinimumBtn = PLAY_BUTTON_INDEX;

	MainMenuBtn[PLAY_BUTTON_INDEX].selected = true;
	MainMenuBtn[PLAY_BUTTON_INDEX].was_selected = false;
	MainMenuBtn[PLAY_BUTTON_INDEX].timer = 0;

	MainMenuBtn[CREDITS_BUTTON_INDEX].selected = false;
	MainMenuBtn[CREDITS_BUTTON_INDEX].was_selected = false;
	MainMenuBtn[CREDITS_BUTTON_INDEX].timer = 0;
}

void MainMenuButtonHandler(void)
{
	static uint8_t btn_selected = PLAY_BUTTON_INDEX;
	static uint8_t previous_btn_selected = 0;
	uint8_t max_buttons;
	bool justExitedCredits = false;

	if(PadOneAnyKeyPressed() == true)
	{
		if(SystemIsRandSeedSet() == false)
		{
			SystemSetRandSeed();
		}
	}

	if(	(PadOneKeyReleased(PAD_CROSS) == true)
				||
		(PadOneKeyReleased(PAD_TRIANGLE) == true)	)
	{
		SfxPlaySound(&AcceptSnd);
	}

	switch(menuLevel)
	{
		case PLAY_CREDITS_LEVEL:
			max_buttons = MAIN_MENU_PLAY_CREDITS_LEVEL_BUTTONS;
		break;

		case CREDITS_LEVEL:
			max_buttons = 0;
			if(PadOneAnyKeyReleased() == true)
			{
				MainMenuRestoreInitValues();
				max_buttons = MAIN_MENU_PLAY_CREDITS_LEVEL_BUTTONS;
				justExitedCredits = true;
				btn_selected = PLAY_BUTTON_INDEX;
			}
		break;

		default:
			max_buttons = 0;
		break;
	}

	MainMenuBtn[previous_btn_selected].was_selected = MainMenuBtn[previous_btn_selected].selected;
	MainMenuBtn[btn_selected].was_selected = MainMenuBtn[btn_selected].selected;

	if(PadOneKeyReleased(PAD_LEFT)	&& (btn_selected > 0) )
	{
		MainMenuBtn[btn_selected].selected = false;
		previous_btn_selected = btn_selected;
		btn_selected--;
		SfxPlaySound(&BellSnd);
	}
	else if(PadOneKeyReleased(PAD_RIGHT)
				&&
			(btn_selected < (max_buttons - 1 + MainMenuMinimumBtn) ) )
	{
		MainMenuBtn[btn_selected].selected = false;
		previous_btn_selected = btn_selected;
		btn_selected++;
		SfxPlaySound(&BellSnd);
	}

	if(btn_selected < MainMenuMinimumBtn)
	{
		btn_selected = MainMenuMinimumBtn;
	}

	if(btn_selected > (max_buttons - 1 + MainMenuMinimumBtn) )
	{
		// Avoid overflow when going back in menu navigation
		btn_selected = (max_buttons - 1 + MainMenuMinimumBtn);
	}

	if(PadOneKeyReleased(PAD_CROSS) && !justExitedCredits)
	{
		if(btn_selected == PLAY_BUTTON_INDEX)
		{
			MainMenuBtn[btn_selected].f();
			// Once gameplay has finished, turn back to first level
			MainMenuRestoreInitValues();
			btn_selected = PLAY_BUTTON_INDEX;
		}
		else
		{
			MainMenuBtn[btn_selected].f();
		}
	}

	MainMenuBtn[btn_selected].selected = true;
}

void CreditsDraw()
{
	SmallFont.spr.r = 0;
	SmallFont.spr.g = 0;
	SmallFont.spr.b = 0;
	
	FontPrintText(	&SmallFont,
					X_SCREEN_RESOLUTION/2 -FONT_DEFAULT_CHAR_SIZE * 5,
					(Y_SCREEN_RESOLUTION/2 + 32) -FONT_DEFAULT_CHAR_SIZE * 2,
					"Video game made by:\n\n -Xavier Del Campo \n -Javier Maldonado\n -Alessio Linares\n -Aria Serra");

	FontPrintText(	&SmallFont,
					X_SCREEN_RESOLUTION/2 -FONT_DEFAULT_CHAR_SIZE * 7,
					Y_SCREEN_RESOLUTION -FONT_DEFAULT_CHAR_SIZE * 2,
					"Press any button to return"	);
					
	SmallFont.spr.r = NORMAL_LUMINANCE;
	SmallFont.spr.g = NORMAL_LUMINANCE;
	SmallFont.spr.b = NORMAL_LUMINANCE;
	
}

void MainMenuDrawButton(TYPE_MMBtn * btn)
{
	MenuSpr.w = BUTTON_SIZE;
	MenuSpr.h = BUTTON_SIZE;

	if(btn->timer < MAIN_MENU_BTN_ANI_SIZE)
	{
		btn->timer++;
	}

	if(btn->selected == true)
	{
		if(btn->was_selected == false)
		{
			btn->timer = 0;
		}

		MenuSpr.r = SELECTED_BUTTON_LUMINANCE;
		MenuSpr.g = SELECTED_BUTTON_LUMINANCE;
		MenuSpr.b = SELECTED_BUTTON_LUMINANCE;
	}
	else
	{
		MenuSpr.r = NORMAL_LUMINANCE;
		MenuSpr.g = NORMAL_LUMINANCE;
		MenuSpr.b = NORMAL_LUMINANCE;
	}

	MenuSpr.u = DEFAULT_BUTTON_U_OFFSET;
	MenuSpr.v = DEFAULT_BUTTON_V_OFFSET;

	switch(btn->i)
	{
		case PLAY_BUTTON_INDEX:
			MenuSpr.x = MAIN_MENU_PLAY_BUTTON_X;
			MenuSpr.y = MAIN_MENU_PLAY_BUTTON_Y - MainMenuBtnAni[btn->timer];
			MenuSpr.u += btn->offset_u;
			MenuSpr.v += btn->offset_v;

			GsSortSprite(&MenuSpr);
		break;

		case CREDITS_BUTTON_INDEX:
			MenuSpr.x = MAIN_MENU_CREDITS_BUTTON_X;
			MenuSpr.y = MAIN_MENU_CREDITS_BUTTON_Y - MainMenuBtnAni[btn->timer];
			MenuSpr.u += btn->offset_u;
			MenuSpr.v += btn->offset_v;

			GsSortSprite(&MenuSpr);
		break;

		default:
		break;
	}

}

void MenuTestCheat(void)
{
	if(MemCardShowMap() == false)
	{
		dprintf("MemCardShowMap() failed!\n");
		return;
	}
}
