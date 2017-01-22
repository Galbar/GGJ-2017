#ifndef __PAD_HEADER__
#define __PAD_HEADER__

/* *************************************
 * 	Includes
 * *************************************/

#include "Global_Inc.h"
#include "System.h"
#include "GameStructures.h"

/* *************************************
 * 	Defines
 * *************************************/

#define PAD_ALWAYS_REPEAT 1

/* *************************************
 * 	Global prototypes
 * *************************************/
 
void PadInit(void);
void PadClearData(void);

bool PadOneConnected(void);

bool PadOneAnyKeyPressed(void);
bool PadTwoAnyKeyPressed(void);

bool PadOneAnyKeyReleased(void);

bool PadOneKeyPressed(unsigned short key);
bool PadTwoKeyPressed(unsigned short key);

bool PadOneKeyRepeat(unsigned short key, uint8_t time);
bool PadTwoKeyRepeat(unsigned short key, uint8_t time);

bool PadOneKeyReleased(unsigned short key);
bool PadTwoKeyReleased(unsigned short key);

bool PadOneDirectionKeyPressed(void);
bool PadTwoDirectionKeyPressed(void);

bool PadOneDirectionKeyReleased(void);
bool PadTwoDirectionKeyReleased(void);

bool PadOneKeySinglePressed(unsigned short key);
bool PadTwoKeySinglePressed(unsigned short key);

bool UpdatePads(void);
bool PadOneIsVibrationEnabled(void);
bool PadTwoIsVibrationEnabled(void);

bool PadAddCheat(TYPE_CHEAT * cheat);

// Experimental (to be removed)
unsigned short * PadOneGetAddress(void);

#endif //__PAD_HEADER__
