/* *************************************
 * 	Includes
 * *************************************/

#include "Camera.h"

/* *************************************
 * 	Defines
 * *************************************/

#define SPEED_CALCULATION_TIME 3
#define MAX_CAMERA_SPEED 5
#define MIN_CAMERA_SPEED 1
#define CAMERA_INITIAL_X_OFFSET (X_SCREEN_RESOLUTION >> 1)

/* *************************************
 * 	Local Prototypes
 * *************************************/

static void CameraUpdateSpeed(void);
static bool CameraSpecialConditions(void);

/* *************************************
 * 	Local Variables
 * *************************************/

static TYPE_CAMERA Camera;

void CameraInit(void)
{
	
}

void CameraApplyCoordinatesToSprite(GsSprite * spr)
{
	spr->x += (short)Camera.X_Offset;
	spr->y += (short)Camera.Y_Offset;
}

void CameraApplyCoordinatesToRectangle(GsRectangle * rect)
{
	dprintf("Rectangle {%d, %d}\n",
			rect->x,
			rect->y	);
			
	rect->x += (short)Camera.X_Offset;
	rect->y += (short)Camera.Y_Offset;
}

void CameraUpdateSpeed(void)
{
	/*if(ptrPlayer->PadDirectionKeyPressed_Callback() == true)
	{
		if(ptrPlayer->PadKeyPressed_Callback(PAD_LEFT) == true)
		{
			if(Camera.X_Speed < 0)
			{
				Camera.X_Speed += 2;
			}
			else if(Camera.X_Speed < MAX_CAMERA_SPEED)
			{
				Camera.X_Speed++;
			}
		}
			
		if(ptrPlayer->PadKeyPressed_Callback(PAD_UP) == true)
		{
			if(Camera.Y_Speed < 0)
			{
				Camera.Y_Speed += 2;
			}
			else if(Camera.Y_Speed < MAX_CAMERA_SPEED)
			{
				Camera.Y_Speed++;
			}
		}
			
		if(ptrPlayer->PadKeyPressed_Callback(PAD_DOWN) == true)
		{
			if(Camera.Y_Speed > 0)
			{
				Camera.Y_Speed -= 2;
			}
			else if(Camera.Y_Speed > -MAX_CAMERA_SPEED)
			{
				Camera.Y_Speed--;
			}
		}
		
		if(ptrPlayer->PadKeyPressed_Callback(PAD_RIGHT) == true)
		{
			if(Camera.X_Speed > 0)
			{
				Camera.X_Speed -= 2;
			}
			else if(Camera.X_Speed > -MAX_CAMERA_SPEED)
			{
				Camera.X_Speed--;
			}
		}
	}

	if(	(ptrPlayer->PadKeyPressed_Callback(PAD_LEFT) == false)
			&&
		(ptrPlayer->PadKeyPressed_Callback(PAD_RIGHT) == false)	)
	{
		if(Camera.X_Speed > 0)
		{
			Camera.X_Speed--;
		}
		else if(Camera.X_Speed < 0)
		{
			Camera.X_Speed++;
		}
	}
	
	if(	(ptrPlayer->PadKeyPressed_Callback(PAD_UP) == false)
			&&
		(ptrPlayer->PadKeyPressed_Callback(PAD_DOWN) == false)	)
	{		
		if(Camera.Y_Speed > 0)
		{
			Camera.Y_Speed--;
		}
		else if(Camera.Y_Speed < 0)
		{
			Camera.Y_Speed++;
		}
	}*/
}

void CameraHandler(void)
{
	if(CameraSpecialConditions() == true)
	{
		Camera.X_Speed = 0;
		Camera.Y_Speed = 0;
		return;
	}
	
	if(Camera.Speed_Timer < SPEED_CALCULATION_TIME)
	{
		Camera.Speed_Timer++;
	}
	else
	{
		Camera.Speed_Timer = 0;
		CameraUpdateSpeed();
	}
	
	Camera.X_Offset += Camera.X_Speed;
	Camera.Y_Offset += Camera.Y_Speed;
}

bool CameraSpecialConditions(void)
{
	return false;
}
