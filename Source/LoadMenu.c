/* **************************************
 * 	Includes							*	
 * *************************************/

#include "LoadMenu.h"

/* **************************************
 * 	Defines								*	
 * *************************************/

/* **************************************
 * 	Structs and enums					*
 * *************************************/

enum
{
	SMALL_FONT_SIZE = 8
};

enum
{
	BG_BLUE_TARGET_VALUE = 0xC0,
	BG_WHITE_TARGET_VALUE = /*0x40*/ 0,
	BG_INCREASE_STEP = 0x10
};

enum
{
	LOADING_BAR_X = 64,
	LOADING_BAR_Y = 200,
	LOADING_BAR_N_LINES = 4,
	
	LOADING_BAR_WIDTH = 256,
	LOADING_BAR_HEIGHT = 16,
	
	LOADING_BAR_LUMINANCE_TARGET = NORMAL_LUMINANCE,
	LOADING_BAR_LUMINANCE_STEP = 10
};

enum
{
	LOADING_TITLE_CLUT_X = 384,
	LOADING_TITLE_CLUT_Y = 496,
	LOADING_TITLE_X = 128,
	LOADING_TITLE_Y = 32,
	
	LOADING_TITLE_U = 0,
	LOADING_TITLE_V = 0,
	
	LOADING_TITLE_LUMINANCE_STEP = 10,
	LOADING_TITLE_LUMINANCE_TARGET = NORMAL_LUMINANCE
};

enum
{	
	PLANE_START_X = 56,
	PLANE_START_Y = 200,
	
	PLANE_U = 0,
	PLANE_V = 32,
	PLANE_SIZE = 16,
	
	PLANE_LUMINANCE_STEP = 0x10,
	PLANE_LUMINANCE_TARGET_VALUE = NORMAL_LUMINANCE
};

/* *************************************
 * 	Local Prototypes
 * *************************************/

static void LoadMenuInit(void);
static void ISR_LoadMenuVBlank(void);
static bool LoadMenuISRHasEnded(void);
static bool LoadMenuISRHasStarted(void);
static void LoadMenuLoadFileList(	char * fileList[], 	void * dest[], 
									uint8_t szFileList, uint8_t szDestList);

/* *************************************
 * 	Local Variables
 * *************************************/

static GsGPoly4 loadMenuBg;
static GsSprite LoadMenuTitleSpr;
static GsLine LoadMenuBarLines[LOADING_BAR_N_LINES];
static GsRectangle LoadMenuBarRect;

static char * LoadMenuFiles[] = {	"cdrom:\\DATA\\SPRITES\\LOADING.TIM;1",
									"cdrom:\\DATA\\FONTS\\FONT_2.FNT;1"	};

static void * LoadMenuDest[] = {(GsSprite*)&LoadMenuTitleSpr,
								(TYPE_FONT*)&SmallFont		};

static char * strCurrentFile;

// Flags to communicate with ISR state
// 	*	startup_flag: background fades in from black to blue.
// 	*	end_flag: tells the background to fade out to black.
//	*	isr_ended: background has totally faded out to black.
//	*	isr_started: tells the ISR has finished starting up.
static volatile bool startup_flag;
static volatile bool isr_started;
static volatile bool end_flag;
static volatile bool isr_ended;
// Set to true when LoadMenuInit() has been called, and set to false
// once LoadMenuEnd() is called.
// It's used when multiple modules call LoadMenu() at the same time,
// so load menu does not have to be initialised each time;
static bool load_menu_running;

void LoadMenuInit(void)
{
	int i;
	static bool first_load = false;
	
	if(first_load == false)
	{
		first_load = true;
		LoadMenuLoadFileList(	LoadMenuFiles,
								LoadMenuDest,
								sizeof(LoadMenuFiles) / sizeof(char*),
								sizeof(LoadMenuDest)	/ sizeof(void*));
	}
	
	FontSetSize(&SmallFont, SMALL_FONT_SIZE);
	
	// "Loading..." title init
	
	LoadMenuTitleSpr.r = 0;
	LoadMenuTitleSpr.g = 0;
	LoadMenuTitleSpr.b = 0;
	
	LoadMenuTitleSpr.x = LOADING_TITLE_X;
	LoadMenuTitleSpr.y = LOADING_TITLE_Y;
	
	LoadMenuTitleSpr.cx = LOADING_TITLE_CLUT_X;
	LoadMenuTitleSpr.cy = LOADING_TITLE_CLUT_Y;
	LoadMenuTitleSpr.u = LOADING_TITLE_U;
	LoadMenuTitleSpr.v = LOADING_TITLE_V;
	
	startup_flag = true;
	isr_started = false;
	end_flag = false;
	isr_ended = false;
	
	// Background init
	
	loadMenuBg.x[0] = 0;
	loadMenuBg.x[1] = X_SCREEN_RESOLUTION;
	loadMenuBg.x[2] = 0;
	loadMenuBg.x[3] = X_SCREEN_RESOLUTION;
	
	loadMenuBg.y[0] = 0;
	loadMenuBg.y[1] = 0;
	loadMenuBg.y[2] = Y_SCREEN_RESOLUTION;
	loadMenuBg.y[3] = Y_SCREEN_RESOLUTION;
	
	// Colour components adjustment (default to zero)
	for(i = 0; i < 4 ; i++)
	{
		loadMenuBg.r[i] = 0;
		loadMenuBg.g[i] = 0;
		loadMenuBg.b[i] = 0;
	}
	
	// "Loading" bar line 0 (up left - up right)
	
	LoadMenuBarLines[0].x[0] = LOADING_BAR_X;
	LoadMenuBarLines[0].x[1] = LOADING_BAR_X + LOADING_BAR_WIDTH;
	
	LoadMenuBarLines[0].y[0] = LOADING_BAR_Y;
	LoadMenuBarLines[0].y[1] = LOADING_BAR_Y;
	
	// "Loading" bar line 1 (up left - down left)
	
	LoadMenuBarLines[1].x[0] = LOADING_BAR_X;
	LoadMenuBarLines[1].x[1] = LOADING_BAR_X;
	
	LoadMenuBarLines[1].y[0] = LOADING_BAR_Y;
	LoadMenuBarLines[1].y[1] = LOADING_BAR_Y + LOADING_BAR_HEIGHT;
	
	// "Loading" bar line 2 (down left - down right)
	
	LoadMenuBarLines[2].x[0] = LOADING_BAR_X;
	LoadMenuBarLines[2].x[1] = LOADING_BAR_X + LOADING_BAR_WIDTH;
	
	LoadMenuBarLines[2].y[0] = LOADING_BAR_Y + LOADING_BAR_HEIGHT;
	LoadMenuBarLines[2].y[1] = LOADING_BAR_Y + LOADING_BAR_HEIGHT;
	
	// "Loading" bar line 3 (up right - down right)
	
	LoadMenuBarLines[3].x[0] = LOADING_BAR_X + LOADING_BAR_WIDTH;
	LoadMenuBarLines[3].x[1] = LOADING_BAR_X + LOADING_BAR_WIDTH;
	
	LoadMenuBarLines[3].y[0] = LOADING_BAR_Y;
	LoadMenuBarLines[3].y[1] = LOADING_BAR_Y + LOADING_BAR_HEIGHT;
	
	for(i = 0; i < LOADING_BAR_N_LINES ; i++)
	{
		LoadMenuBarLines[i].r = 0;
		LoadMenuBarLines[i].g = 0;
		LoadMenuBarLines[i].b = 0;
	}
	
	LoadMenuBarRect.r = 0;
	LoadMenuBarRect.g = 0;
	LoadMenuBarRect.b = 0;
	
	// LoadMenuBarRect.attribute |= ENABLE_TRANS | TRANS_MODE(0);
	
	LoadMenuBarRect.x = LOADING_BAR_X;
	LoadMenuBarRect.y = LOADING_BAR_Y;
	LoadMenuBarRect.w = 0;
	LoadMenuBarRect.h = LOADING_BAR_HEIGHT;
	
	LoadMenuBarRect.attribute |= ENABLE_TRANS | TRANS_MODE(0);
	
	load_menu_running = true;
	
	SmallFont.spr.r = 0;
	SmallFont.spr.g = 0;
	SmallFont.spr.b = 0;
	
	GfxSetGlobalLuminance(0);
	
	SetVBlankHandler(&ISR_LoadMenuVBlank);
}

void LoadMenuEnd(void)
{
	end_flag = true;
	load_menu_running = false;
	
	while(LoadMenuISRHasEnded() == false);
	dprintf("Set default VBlank handler.\n");
	SetVBlankHandler(&ISR_SystemDefaultVBlank);
	
	GfxSetGlobalLuminance(NORMAL_LUMINANCE);
}

void ISR_LoadMenuVBlank(void)
{
	uint8_t i;
	
	if( (SystemIsBusy() == true) || (GfxIsGPUBusy() == true) )
	{
		return;
	}
	
	if(startup_flag == true)
	{
		// "Loading..." text
		if(LoadMenuTitleSpr.r < LOADING_TITLE_LUMINANCE_TARGET)
		{
			LoadMenuTitleSpr.r += LOADING_TITLE_LUMINANCE_STEP;
			LoadMenuTitleSpr.g += LOADING_TITLE_LUMINANCE_STEP;
			LoadMenuTitleSpr.b += LOADING_TITLE_LUMINANCE_STEP;
		}
		
		if(loadMenuBg.g[0] < BG_WHITE_TARGET_VALUE)
		{
			loadMenuBg.r[0] += BG_INCREASE_STEP;
			loadMenuBg.r[1] += BG_INCREASE_STEP;
			
			loadMenuBg.g[0] += BG_INCREASE_STEP;
			loadMenuBg.g[1] += BG_INCREASE_STEP;
			
			loadMenuBg.b[0] += BG_INCREASE_STEP;
			loadMenuBg.b[1] += BG_INCREASE_STEP;
		}
		// Blue background
		if(loadMenuBg.b[2] < BG_BLUE_TARGET_VALUE)
		{
			loadMenuBg.b[2] += BG_INCREASE_STEP;
			loadMenuBg.b[3] += BG_INCREASE_STEP;
		}
		
		if(LoadMenuBarRect.r < LOADING_BAR_LUMINANCE_TARGET)
		{
			LoadMenuBarRect.r += LOADING_BAR_LUMINANCE_STEP;
			LoadMenuBarRect.g += LOADING_BAR_LUMINANCE_STEP;
			LoadMenuBarRect.b += LOADING_BAR_LUMINANCE_STEP;
		}
		else
		{
			startup_flag = false;
			isr_started = true;
		}
		
		for(i = 0;i < LOADING_BAR_N_LINES ; i++)
		{
			if(LoadMenuBarLines[i].r < LOADING_BAR_LUMINANCE_TARGET)
			{
				LoadMenuBarLines[i].r += LOADING_BAR_LUMINANCE_STEP;
				LoadMenuBarLines[i].g += LOADING_BAR_LUMINANCE_STEP;
				LoadMenuBarLines[i].b += LOADING_BAR_LUMINANCE_STEP;
			}
		}
		
	}
	else if(end_flag == true)
	{
		LoadMenuTitleSpr.r -= LOADING_TITLE_LUMINANCE_STEP;
		LoadMenuTitleSpr.g -= LOADING_TITLE_LUMINANCE_STEP;
		LoadMenuTitleSpr.b -= LOADING_TITLE_LUMINANCE_STEP;
		
		if(loadMenuBg.g[0] > 0)
		{
			loadMenuBg.r[0] -= BG_INCREASE_STEP;
			loadMenuBg.r[1] -= BG_INCREASE_STEP;
			
			loadMenuBg.g[0] -= BG_INCREASE_STEP;
			loadMenuBg.g[1] -= BG_INCREASE_STEP;
			
			loadMenuBg.b[0] -= BG_INCREASE_STEP;
			loadMenuBg.b[1] -= BG_INCREASE_STEP;
		}
		
		if(loadMenuBg.b[2] > 0)
		{
			loadMenuBg.b[2] -= BG_INCREASE_STEP;
			loadMenuBg.b[3] -= BG_INCREASE_STEP;
		}
		
		if(loadMenuBg.b[2] == 0)
		{
			end_flag = false;
			isr_ended = true;
		}
		
		LoadMenuBarRect.w = LOADING_BAR_WIDTH;
		
		if(LoadMenuBarRect.r > 0)
		{
			LoadMenuBarRect.r -= LOADING_BAR_LUMINANCE_STEP;
			LoadMenuBarRect.g -= LOADING_BAR_LUMINANCE_STEP;
			LoadMenuBarRect.b -= LOADING_BAR_LUMINANCE_STEP;
		}
		
		for(i = 0;i < LOADING_BAR_N_LINES ; i++)
		{
			if(LoadMenuBarLines[i].r > 0)
			{
				LoadMenuBarLines[i].r -= LOADING_BAR_LUMINANCE_STEP;
				LoadMenuBarLines[i].g -= LOADING_BAR_LUMINANCE_STEP;
				LoadMenuBarLines[i].b -= LOADING_BAR_LUMINANCE_STEP;
			}
		}
	}
	
	GsSortGPoly4(&loadMenuBg);
	
	GsSortRectangle(&LoadMenuBarRect);
	
	for(i = 0 ; i < LOADING_BAR_N_LINES ; i++)
	{
		GsSortLine(&LoadMenuBarLines[i]);
	}
	
	GsSortSprite(&LoadMenuTitleSpr);
	
	FontSetFlags(&SmallFont, FONT_BLEND_EFFECT);
	
	FontPrintText(	&SmallFont,
					LOADING_BAR_X - 8,
					LOADING_BAR_Y + LOADING_BAR_HEIGHT + 8,
					strCurrentFile	);
					
	FontSetFlags(&SmallFont, FONT_NOFLAGS);
		
	GfxDrawScene_Fast();
}

bool LoadMenuISRHasEnded(void)
{
	return isr_ended;
}

bool LoadMenuISRHasStarted(void)
{
	return isr_started;
}

void LoadMenu(	char *	fileList[], 
				void * dest[],
				uint8_t szFileList	, uint8_t szDestList)
{
	
	if(load_menu_running == false)
	{
		LoadMenuInit();
	
		while(LoadMenuISRHasStarted() == false);
	}
	
	LoadMenuLoadFileList(fileList,dest,szFileList,szDestList);
	
}

void LoadMenuLoadFileList(	char * fileList[], 	void * dest[], 
							uint8_t szFileList, uint8_t szDestList)
{
	char aux_file_name[100];
	char * extension;
	short x_increment;
	uint8_t fileLoadedCount;
	
	if(szFileList != szDestList)
	{
		dprintf("File list size different from dest list size! %d vs %d\n",
				szFileList, szDestList);
		return;
	}
	
	for(fileLoadedCount = 0; fileLoadedCount < szFileList ; fileLoadedCount++)
	{
		if(fileList[fileLoadedCount] == NULL)
		{
			continue;
		}
		
		strCurrentFile = fileList[fileLoadedCount];
		
		x_increment = (short)(LOADING_BAR_WIDTH / szFileList);
		
		LoadMenuBarRect.w = fileLoadedCount* x_increment;
						
		//dprintf("Files %d / %d loaded. New plane X = %d.\n",fileLoadedCount,szFileList,LoadMenuPlaneSpr.x);
		
		// Backup original file path
		strncpy(aux_file_name,fileList[fileLoadedCount],100);
		
		//We want to get file extension, so split into tokens
		strtok(fileList[fileLoadedCount],".;");
		extension = strtok(NULL,".;");
		
		dprintf("File extension: .%s\n",extension);
		//Restore original file path in order to load file
		strncpy(fileList[fileLoadedCount],aux_file_name,100);
		
		if(strncmp(extension,"TIM",3) == 0)
		{
			if(GfxSpriteFromFile(fileList[fileLoadedCount], dest[fileLoadedCount]) == false)
			{
				dprintf("Could not load image file \"%s\"!\n",fileList[fileLoadedCount]);
			}
		}
		else if(strncmp(extension,"CLT",3) == 0)
		{
			if(dest[fileLoadedCount] != NULL)
			{
				dprintf("WARNING: File %s linked to non-NULL destination pointer!\n", dest[fileLoadedCount]);
			}
			
			if(GfxCLUTFromFile(fileList[fileLoadedCount]) == false)
			{
				dprintf("Could not load CLUT file \"%s\"!\n",fileList[fileLoadedCount]);
			}
		}
		else if(strncmp(extension,"VAG",3) == 0)
		{
			if(SfxUploadSound(fileList[fileLoadedCount], dest[fileLoadedCount]) == false)
			{
				dprintf("Could not load sound file \"%s\"!\n",fileList[fileLoadedCount]);
			}
		}
		else if(strncmp(extension,"FNT",3) == 0)
		{
			if(FontLoadImage(fileList[fileLoadedCount], dest[fileLoadedCount]) == false)
			{
				dprintf("Could not load font file \"%s\"!\n",fileList[fileLoadedCount]);
			}
		}
		else
		{
			dprintf("LoadMenu does not recognize following extension: %s\n",extension);
		}
	}
}
