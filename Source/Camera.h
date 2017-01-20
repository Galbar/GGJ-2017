#ifndef __CAM_HEADER__
#define __CAM_HEADER__

/* *************************************
 * 	Includes
 * *************************************/

#include "Global_Inc.h"
#include "System.h"
#include "Pad.h"
#include "GameStructures.h"

/* *************************************
 * 	Defines
 * *************************************/

/* *************************************
 * 	Global prototypes
 * *************************************/

void CameraInit(void);
void CameraHandler(void);
void CameraApplyCoordinatesToSprite(GsSprite * spr);
void CameraApplyCoordinatesToRectangle(GsRectangle * rect);

#endif //__CAM_HEADER__
