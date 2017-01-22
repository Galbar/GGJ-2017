/* *************************************
 * 	Includes
 * *************************************/

#include "Gfx.h"

/* *************************************
 * 	Defines
 * *************************************/

#define PRIMITIVE_LIST_SIZE 0x800
#define DOUBLE_BUFFERING_SWAP_Y	256
#define UPLOAD_IMAGE_FLAG 1
#define MAX_LUMINANCE 0xFF
#define ROTATE_BIT_SHIFT 12

/* *************************************
 * 	Structs and enums
 * *************************************/

enum
{
	BUTTON_SIZE = 16,
	
	BUTTON_CROSS_U = 48,
	BUTTON_CROSS_V = 0,
	
	BUTTON_SQUARE_U = 0,
	BUTTON_SQUARE_V = 0,
	
	BUTTON_TRIANGLE_U = 32,
	BUTTON_TRIANGLE_V = 0,
	
	BUTTON_CIRCLE_U = 16,
	BUTTON_CIRCLE_V = 0,
};

enum
{
	GFX_SECOND_DISPLAY_X = 384,
	GFX_SECOND_DISPLAY_Y = 256,
	GFX_SECOND_DISPLAY_TPAGE = 22
};

/* *************************************
 * 	Global Variables
 * *************************************/

GsSprite PSXButtons;

/* *************************************
 * 	Local Prototypes
 * *************************************/

static void GfxSwapBuffers(void);

/* *************************************
 * 	Local Variables
 * *************************************/

//Drawing environment
static GsDrawEnv DrawEnv;
//Display environment
static GsDispEnv DispEnv;
//Primitive list (it contains all the graphical data for the GPU)
static unsigned int prim_list[PRIMITIVE_LIST_SIZE];
//Tells other modules whether data is being loaded to GPU
static volatile bool gfx_busy;
//Dictates (R,G,B) brigthness to all sprites silently
static uint8_t global_lum;

void GfxSwapBuffers(void)
{
	if(DrawEnv.h == Y_SCREEN_RESOLUTION)
	{
			if(DispEnv.y == 0)
			{
				DispEnv.y = DOUBLE_BUFFERING_SWAP_Y;
				DrawEnv.y = 0;
			}
			else if(DispEnv.y == DOUBLE_BUFFERING_SWAP_Y)
			{
				DispEnv.y = 0;
				DrawEnv.y = DOUBLE_BUFFERING_SWAP_Y;
			}
			
		GsSetDispEnv(&DispEnv);
		GsSetDrawEnv(&DrawEnv);
	}

}


void GfxInitDrawEnv(void)
{
	DrawEnv.x = 0;
	DrawEnv.y = 0;
	DrawEnv.draw_on_display = false;
	DrawEnv.w = X_SCREEN_RESOLUTION;
	DrawEnv.h = Y_SCREEN_RESOLUTION;
	
	GsSetDrawEnv(&DrawEnv);
}

void GfxInitDispEnv(void)
{
	DispEnv.x = 0;
	DispEnv.y = 0;
	
	GsSetDispEnv(&DispEnv);
}

void GfxSetPrimitiveList(void)
{
	GsSetList(prim_list);
}

void GfxDrawScene_Fast(void)
{	
	GfxSwapBuffers();
	FontCyclic();
	GsDrawList();
}

void GfxDrawScene(void)
{
	while(	(SystemRefreshNeeded() == false) 
				||
			(GsIsDrawing() == true)
				||
			(SystemDMAReady() == false)		);
			
	GfxDrawScene_Fast();
	
	if(UpdatePads() == false)
	{
		SystemSetEmergencyMode(true);
	}
	else
	{
		SystemSetEmergencyMode(false);
	}
	
	SystemRunTimers();
	
	SystemUserTimersHandler();
	
	SystemDisableScreenRefresh();
	
	MemCardHandler();
	
	SystemCheckStack();
}

void GfxDrawScene_Slow(void)
{
	GfxDrawScene();
	while(GfxIsGPUBusy() == true);
}

void GfxSortGsGPoly4(GsGPoly4 * poly)
{
	short w = abs(poly->x[1] - poly->x[0]);
	short h = abs(poly->y[0] - poly->x[2]);
	
	if(GfxIsInsideScreenArea(poly->x[0], poly->y[0], w, h) == false)
	{
		return;
	}
	
	GsSortGPoly4(poly);
}

void GfxSortSprite(GsSprite * spr)
{
	uint8_t aux_r = spr->r;
	uint8_t aux_g = spr->g;
	uint8_t aux_b = spr->b;
	unsigned char aux_tpage = spr->tpage;
	short aux_w = spr->w;
	short aux_x = spr->x;
	bool has_1hz_flash = spr->attribute & GFX_1HZ_FLASH;
	bool has_2hz_flash = spr->attribute & GFX_2HZ_FLASH;
	short max_size_allowed;
	short orig_w = spr->w;
	
	if(spr->attribute & (COLORMODE(COLORMODE_4BPP)))
	{
		max_size_allowed = MAX_SIZE_FOR_GSSPRITE_4BIT;
	}
	else
	{
		max_size_allowed = MAX_SIZE_FOR_GSSPRITE;
	}
	
	if(	(spr->w <= 0) || (spr->h <= 0) )
	{
		// Invalid width or heigth
		return;
	}
	
	if(GfxIsSpriteInsideScreenArea(spr) == false)
	{
		return;
	}
	else if(has_2hz_flash && Gfx2HzFlash() == false)
	{
		return;
	}
	else if(has_1hz_flash && Gfx1HzFlash() == false)
	{
		return;
	}
	
	if(global_lum != NORMAL_LUMINANCE)
	{
		if(spr->r < NORMAL_LUMINANCE - global_lum)
		{
			spr->r = 0;
		}
		else
		{
			spr->r -= NORMAL_LUMINANCE - global_lum;
		}
		
		if(spr->g < NORMAL_LUMINANCE - global_lum)
		{
			spr->g = 0;
		}
		else
		{
			spr->g -= NORMAL_LUMINANCE - global_lum;
		}
		
		if(spr->b < NORMAL_LUMINANCE - global_lum)
		{
			spr->b = 0;
		}
		else
		{
			spr->b -= NORMAL_LUMINANCE - global_lum;
		}
	}
	
	if(has_1hz_flash == true)
	{
		spr->attribute &= ~(GFX_1HZ_FLASH);
	}
	
	if(spr->w > max_size_allowed)
	{
		// GsSprites can't be bigger than 256x256, so since display
		// resolution is 384x240, it must be split into two primitives.
		
		spr->w = max_size_allowed;
		GsSortSprite(spr);

		spr->x += max_size_allowed;
		spr->w = orig_w - max_size_allowed;
		spr->tpage += max_size_allowed / GFX_TPAGE_WIDTH;
		GsSortSprite(spr);
		
		// Restore original values after sorting
		spr->w = aux_w;
		spr->tpage = aux_tpage;
		spr->x = aux_x;
		
		spr->w = orig_w;
	}
	else
	{
		GsSortSprite(spr);
	}
	
	if(has_1hz_flash == true)
	{
		spr->attribute |= GFX_1HZ_FLASH;
	}
	
	spr->r = aux_r;
	spr->g = aux_g;
	spr->b = aux_b;
}

uint8_t GfxGetGlobalLuminance(void)
{
	return global_lum;
}

void GfxSetGlobalLuminance(uint8_t value)
{
	global_lum = value;
}

void GfxIncreaseGlobalLuminance(int8_t step)
{	
	if( (	(global_lum + step) < MAX_LUMINANCE )
			&&
		(	(global_lum + step) > 0	)			)
	{
		global_lum += step;
	}
	else
	{
		global_lum = MAX_LUMINANCE;
	}
}

int GfxRotateFromDegrees(int deg)
{
	return deg << ROTATE_BIT_SHIFT;
}

bool GfxIsGPUBusy(void)
{
	return (GsIsDrawing() || gfx_busy || SystemDMABusy() );
}

bool GfxSpriteFromFile(char * fname, GsSprite * spr)
{
	GsImage gsi;
	
	if(SystemLoadFile(fname) == false)
	{
		return false;
	}
	
	while(GfxIsGPUBusy() == true);
	
	gfx_busy = true;
		
	GsImageFromTim(&gsi,SystemGetBufferAddress() );
	
	GsSpriteFromImage(spr,&gsi,UPLOAD_IMAGE_FLAG);
	gfx_busy = false;
	
	return true;
}

bool GfxCLUTFromFile(char * fname)
{
	GsImage gsi;

	if(SystemLoadFile(fname) == false)
	{
		return false;
	}
	
	while(GfxIsGPUBusy() == true);
	
	gfx_busy = true;
		
	GsImageFromTim(&gsi,SystemGetBufferAddress() );
	
	GsUploadCLUT(&gsi);
	
	gfx_busy = false;
	
	return true;
}

bool GfxIsInsideScreenArea(short x, short y, short w, short h)
{
	if( ( (x + w) >= 0) 
			&&
		(x < X_SCREEN_RESOLUTION)
			&&
		( (y + h) >= 0)
			&&
		(y < Y_SCREEN_RESOLUTION)	)
	{
		return true;
	}
		
	return false;
}

bool GfxIsSpriteInsideScreenArea(GsSprite * spr)
{
	return GfxIsInsideScreenArea(spr->x, spr->y, spr->w, spr->h);
}

void GfxDrawButton(short x, short y, unsigned short btn)
{
	static bool first_entered = true;
	static short orig_u;
	static short orig_v;
	
	if(first_entered == true)
	{
		first_entered = false;
		orig_u = PSXButtons.u;
		orig_v = PSXButtons.v;
	}
	
	PSXButtons.w = BUTTON_SIZE;
	PSXButtons.h = BUTTON_SIZE;
	
	PSXButtons.r = NORMAL_LUMINANCE;
	PSXButtons.g = NORMAL_LUMINANCE;
	PSXButtons.b = NORMAL_LUMINANCE;
	
	PSXButtons.x = x;
	PSXButtons.y = y;
	
	switch(btn)
	{
		case PAD_CROSS:
			PSXButtons.u = BUTTON_CROSS_U;
			PSXButtons.v = BUTTON_CROSS_V;
		break;
		
		case PAD_SQUARE:
			PSXButtons.u = BUTTON_SQUARE_U;
			PSXButtons.v = BUTTON_SQUARE_V;
		break;
		
		case PAD_TRIANGLE:
			PSXButtons.u = BUTTON_TRIANGLE_U;
			PSXButtons.v = BUTTON_TRIANGLE_V;
		break;
		
		case PAD_CIRCLE:
			PSXButtons.u = BUTTON_CIRCLE_U;
			PSXButtons.v = BUTTON_CIRCLE_V;
		break;
		
		case PAD_L1:
		case PAD_L2:
		case PAD_R1:
		case PAD_R2:
		case PAD_SELECT:
		case PAD_START:
		default:
			// Set null width and height so that sprite doesn't get sorted
			PSXButtons.w = 0;
			PSXButtons.h = 0;
		break;
	}
	
	PSXButtons.u += orig_u;
	PSXButtons.v += orig_v;
	
	GfxSortSprite(&PSXButtons);
}

void GfxSaveDisplayData(GsSprite *spr)
{
	MoveImage(	DispEnv.x,
				DispEnv.y,
				GFX_SECOND_DISPLAY_X,
				GFX_SECOND_DISPLAY_Y,
				X_SCREEN_RESOLUTION,
				Y_SCREEN_RESOLUTION);
				
	spr->x = 0;
	spr->y = 0;
	spr->tpage = GFX_SECOND_DISPLAY_TPAGE;
	spr->attribute |= COLORMODE(COLORMODE_16BPP);
	spr->w = X_SCREEN_RESOLUTION;
	spr->h = Y_SCREEN_RESOLUTION;
	spr->u = 0;
	spr->v = 0;
	spr->r = NORMAL_LUMINANCE;
	spr->g = NORMAL_LUMINANCE;
	spr->b = NORMAL_LUMINANCE;
}

bool Gfx1HzFlash(void)
{
	static bool show = false;
	
	if(System1SecondTick() == true)
	{
		show = show? false:true;
	}
	
	return show;
}

bool Gfx2HzFlash(void)
{
	static bool show = false;
	
	if(System500msTick() == true)
	{
		show = show? false:true;
	}
	
	return show;
}

bool GfxTPageOffsetFromVRAMPosition(GsSprite * spr, short x, short y)
{
	if(	(x >= VRAM_W) || (x < 0) || (y >= VRAM_H) || (y < 0) )
	{
		return false;
	}
	
	spr->tpage = x / GFX_TPAGE_WIDTH;
	spr->tpage += (short)(VRAM_W / GFX_TPAGE_WIDTH) * (short)(y / GFX_TPAGE_HEIGHT);
	
	spr->u = (x % GFX_TPAGE_WIDTH);
	
	if(spr->attribute & COLORMODE(COLORMODE_8BPP))
	{
		// On 8bpp images, it looks like U offset needs to be multiplied by 2.
		spr->u <<= 1;
	}
	
	spr->v = (y % GFX_TPAGE_HEIGHT);
	
	//dprintf("Sprite:\n\tTPAGE: %d\n\tU=%d\n\tV=%d\n",spr->tpage,spr->u, spr->v);
	
	return false;
}
