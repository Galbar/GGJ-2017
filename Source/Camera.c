/* *************************************
 * 	Includes
 * *************************************/

#include "Camera.h"

/* *************************************
 * 	Defines
 * *************************************/

#define SPEED_CALCULATION_TIME 3
#define MAX_CAMERA_SPEED 4
#define MIN_CAMERA_SPEED 1
#define CAMERA_INITIAL_X_OFFSET (X_SCREEN_RESOLUTION >> 1)
#define CAMERA_FAST_MOVEMENT_THRESHOLD 16
#define CAMERA_FAST_MOVEMENT_THRESHOLD_HYSTERESIS MAX_CAMERA_SPEED

/* *************************************
 * 	Local Prototypes
 * *************************************/

static void CameraUpdateSpeed(TYPE_PLAYER * ptrPlayer1, TYPE_PLAYER * ptrPlayer2);
static bool CameraSpecialConditions(void);

/* *************************************
 * 	Local Variables
 * *************************************/

static TYPE_CAMERA Camera;
static GsRectangle TargetRect;

void CameraInit(void)
{
	bzero((TYPE_CAMERA*)&Camera, sizeof(TYPE_CAMERA));
	TargetRect.w = 8;
	TargetRect.h = 8;
	TargetRect.r = 255;
	TargetRect.g = 255;
	TargetRect.b = 255;
}

void CameraApplyCoordinatesToSprite(GsSprite * spr)
{
	spr->x += (short)Camera.X_Offset;
}

void CameraApplyCoordinatesToRectangle(GsRectangle * rect)
{
	rect->x += (short)Camera.X_Offset;
}

void CameraApplyCoordinatesToGsGPoly4(GsGPoly4 * poly4)
{
	uint8_t i;
	
	for(i = 0; i < 4; i++)
	{
		poly4->x[i] += (short)Camera.X_Offset;
	}
}

void CameraUpdateSpeed(TYPE_PLAYER * ptrPlayer1, TYPE_PLAYER * ptrPlayer2)
{
	short plOne_middle = fix16_to_int(ptrPlayer1->position.x);
	short plTwo_middle = fix16_to_int(ptrPlayer2->position.x);
	
	short middle_point = abs(plOne_middle - plTwo_middle) >> 1;
	
	static bool hysteresis = false;
	short ref_value;
	
	short camera_diff;
	
	if(	plOne_middle < plTwo_middle	)
	{
		// Player 1 left, player 2 right
		Camera.TargetPos = plOne_middle + middle_point;
	}
	else if( plOne_middle > plTwo_middle)
	{
		// Player 2 left, player 1 right
		// Take into account that the following condition:
		// (ptrPlayer1->position.x) == (ptrPlayer2-> position.y)
		// is not of interest! (Should we update camera if balls are actually colliding?)
		Camera.TargetPos = plTwo_middle + middle_point;
	}
	
	camera_diff = Camera.TargetPos - ((-Camera.X_Offset) + (X_SCREEN_RESOLUTION >> 1) );
	
	dprintf("Target = {%d}\n", Camera.TargetPos);
	
	dprintf("Middle = {%d}\n", middle_point);
	
	if(((-Camera.X_Offset) + camera_diff) < 0)
	{
		dprintf("Left edge exceeded! %d\n", (Camera.X_Offset + camera_diff));
		// Do not go too much left!
		return;
	}
	
	// LEVEL_SIZE : 640 x 240
	
	if((Camera.X_Offset + camera_diff) >= (LEVEL_X_SIZE - (X_SCREEN_RESOLUTION >> 1) ) )
	{
		dprintf("Right edge exceeded!\n");
		return;
	}
	
	// ptrPlayer->PadKeyPressed_Callback(PAD_LEFT)
	// target_X < x_offset
	
	dprintf("CAMERA DIFF = %d\n",camera_diff);
	
	ref_value = hysteresis ? CAMERA_FAST_MOVEMENT_THRESHOLD_HYSTERESIS: CAMERA_FAST_MOVEMENT_THRESHOLD;
	
	if(camera_diff < -ref_value)
	{
		hysteresis = true;
		
		if(Camera.X_Speed < 0)
		{
			Camera.X_Speed += 2;
		}
		else if(Camera.X_Speed < MAX_CAMERA_SPEED)
		{
			Camera.X_Speed++;
		}
	}
	else if(camera_diff > ref_value)
	{
		hysteresis = true;
		
		if(Camera.X_Speed > 0)
		{
			Camera.X_Speed --;
		}
		else if(Camera.X_Speed > -MAX_CAMERA_SPEED)
		{
			Camera.X_Speed--;
		}
	}
	else if(abs(camera_diff) <= ref_value)
	{
		hysteresis = false;
		// Slow movement
		if(Camera.X_Speed > 0)
		{
			Camera.X_Speed--;
		}
		else if(Camera.X_Speed < 0)
		{
			Camera.X_Speed++;
		}
		else
		{
			// Camera has no speed
			if(camera_diff > 0)
			{
				Camera.X_Offset--;
			}
			else if(camera_diff < 0)
			{
				Camera.X_Offset++;
			}
		}
	}
}

void CameraHandler(TYPE_PLAYER * ptrPlayer1, TYPE_PLAYER * ptrPlayer2)
{
	if(CameraSpecialConditions() == true)
	{
		Camera.X_Speed = 0;
		return;
	}
	
	TargetRect.x = Camera.TargetPos;
	TargetRect.y = 128;
	
	if(Camera.Speed_Timer < SPEED_CALCULATION_TIME)
	{
		Camera.Speed_Timer++;
	}
	else
	{
		Camera.Speed_Timer = 0;
		CameraUpdateSpeed(ptrPlayer1, ptrPlayer2);
	}
	
	Camera.X_Offset += Camera.X_Speed;
}

bool CameraSpecialConditions(void)
{
	return false;
}

void CameraDrawTarget(void)
{
	CameraApplyCoordinatesToRectangle(&TargetRect);
	GsSortRectangle(&TargetRect);
}
