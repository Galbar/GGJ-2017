#ifndef __GAME_STRUCTURES__HEADER__
#define __GAME_STRUCTURES__HEADER__

/* *************************************
 * 	Defines
 * *************************************/

#define GAME_MAX_CHARACTERS 8
#define CHEAT_ARRAY_SIZE 16
#define WAVE_RADIUS 32

/* *************************************
 * 	Structs and enums
 * *************************************/

typedef struct t_Camera
{
	int16_t X_Offset;
	int8_t X_Speed;
	uint8_t Speed_Timer;
	short TargetPos;
}TYPE_CAMERA;

typedef struct t_vector
{
	fix16_t x;
	fix16_t y;
}TYPE_VECTOR;

typedef struct t_wave
{
    fix16_t y_speed;
    TYPE_VECTOR position;
    TYPE_VECTOR scale;
}TYPE_WAVE;

typedef struct t_player
{
	bool dead;
	uint8_t wind_slots;
	uint16_t hits;
	fix16_t radius;
	TYPE_VECTOR position;
	TYPE_VECTOR speed;

	bool	(*PadKeyPressed_Callback)(unsigned short);
	bool	(*PadKeyReleased_Callback)(unsigned short);
	bool	(*PadDirectionKeyPressed_Callback)(void);
	TYPE_CAMERA Camera;
	GsSprite * ptrSprite;
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
