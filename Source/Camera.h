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
void CameraHandler(TYPE_PLAYER * ptrPlayer1, TYPE_PLAYER * ptrPlayer2);
void CameraApplyCoordinatesToSprite(GsSprite * spr);
void CameraApplyCoordinatesToRectangle(GsRectangle * rect);
void CameraApplyCoordinatesToGsGPoly4(GsGPoly4 * poly4);
void CameraDrawTarget(void);

#endif //__CAM_HEADER__
