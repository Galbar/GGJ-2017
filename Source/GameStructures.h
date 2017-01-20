#ifndef __GAME_STRUCTURES__HEADER__
#define __GAME_STRUCTURES__HEADER__

/* *************************************
 * 	Defines
 * *************************************/

#define GAME_MAX_CHARACTERS 8
#define CHEAT_ARRAY_SIZE 16

/* *************************************
 * 	Structs and enums
 * *************************************/

typedef struct t_Camera
{
	int32_t X_Offset;
	int32_t Y_Offset;
	int8_t X_Speed;
	int8_t Y_Speed;
	uint8_t Speed_Timer;
}TYPE_CAMERA;

typedef enum t_flstate
{
	STATE_IDLE = 0,
	STATE_PARKED,
	STATE_TAXIING,
	STATE_TAKEOFF,
	STATE_APPROACH,
	STATE_FINAL,
	STATE_LANDED
}FL_STATE;

typedef struct t_isopos
{
	short x;
	short y;
	short z;
}TYPE_ISOMETRIC_POS;

typedef struct t_isofix16pos
{
	fix16_t x;
	fix16_t y;
	fix16_t z;
}TYPE_ISOMETRIC_FIX16_POS;

typedef struct t_cartpos
{
	short x;
	short y;
}TYPE_CARTESIAN_POS;

typedef struct
{
	// ## State flags ##
		// Player is on the game
		bool Active;
		// Player has locked the camera at a determined aircraft
		bool LockTarget;
		
	uint8_t ID;
	
	bool	(*PadKeyPressed_Callback)(unsigned short);
	bool	(*PadKeyReleased_Callback)(unsigned short);
	bool	(*PadDirectionKeyPressed_Callback)(void);
	TYPE_CAMERA Camera;
}TYPE_PLAYER;

typedef enum t_fontflags
{
	FONT_NOFLAGS		= 0,
	FONT_CENTERED		= 0x01,
	FONT_WRAP_LINE		= 0x02,
	FONT_BLEND_EFFECT	= 0x04,
	FONT_1HZ_FLASH		= 0x08,
	FONT_2HZ_FLASH		= 0x10
}FONT_FLAGS;

typedef struct t_Font
{
	GsSprite spr;
	short char_w;
	short char_h;
	char init_ch;
	uint8_t char_per_row;
	uint8_t max_ch_wrap;
	FONT_FLAGS flags;
	short spr_w;
	short spr_h;
	short spr_u;
	short spr_v;
}TYPE_FONT;

typedef struct t_Timer
{
	uint32_t time;
	uint32_t orig_time;
	bool repeat_flag;
	bool busy;
	void (*Timeout_Callback)(void);
}TYPE_TIMER;

typedef struct t_Cheat
{
	unsigned short Combination[CHEAT_ARRAY_SIZE];
	void (*Callback)(void);
}TYPE_CHEAT;

#endif // __GAME_STRUCTURES__HEADER__
