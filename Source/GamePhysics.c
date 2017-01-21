/* **************************************
 * 	Includes							*
 * **************************************/

#include "GamePhysics.h"

/* **************************************
 * 	Defines								*
 * **************************************/

#define BOUNCE_COEF_BALLS 0xCCCC // 0.8
#define BOUNCE_COEF_WAVES 0x4CCC // 0.3
#define GAMEPHYSICS_GRAVITY_ACCELERATION 0x2000
#define GAMEPHYSICS_WIND_DECELERATION 0xA00
#define GAMEPHYSICS_TACKLE_SPEED (0x7C000)
#define GAMEPHYSICS_NORMAL_SPEED (0x4000)
#define GAMEPHYSICS_MAX_NORMAL_SPEED (0x8000) // 8.00
#define GAMEPHYSICS_LAUNCH_SPEED (-0x60000) // 8.00

#define GAMEPHYSICS_INITIAL_WAVE_SPEED 0x20000

/* **************************************
 * 	Local variables						*
 * **************************************/
 
/* **************************************
 * 	Local prototypes					*
 * **************************************/
 
static void GamePhysicsApplyGravity(TYPE_PLAYER * ptrPlayer);
static void GamePhysicsTackleHandler(TYPE_PLAYER * ptrPlayer);

static TYPE_COLLISION collisions[MAX_POSSIBLE_COLLISIONS];
static uint8_t num_collisions;

void GamePhysicsInit(void)
{
	bzero((TYPE_WAVE*)&WaveData, sizeof(TYPE_WAVE));
	
	WaveData[0].speed.y = GAMEPHYSICS_INITIAL_WAVE_SPEED;
}

TYPE_VECTOR GamePhysicsVectorDiff(TYPE_VECTOR * ptrVector1, TYPE_VECTOR * ptrVector2)
{
    TYPE_VECTOR diff;
    
    diff.x = ptrVector1->x - ptrVector2->x;
    diff.y = ptrVector1->y - ptrVector2->y;
    
    return diff;
}

TYPE_VECTOR GamePhysicsVectorMul(TYPE_VECTOR * ptrVector1, TYPE_VECTOR * ptrVector2)
{
    TYPE_VECTOR mul;
    
    mul.x = ptrVector1->x * ptrVector2->x;
    mul.y = ptrVector1->y * ptrVector2->y;
    
    return mul;
}

fix16_t GamePhysicsVectorDist(TYPE_VECTOR * ptrVector1, TYPE_VECTOR * ptrVector2)
{
    TYPE_VECTOR dist = GamePhysicsVectorDiff(ptrVector1, ptrVector2);
    
    return fix16_sqrt(fix16_mul(dist.x, dist.x) + fix16_mul(dist.y, dist.y));
}

TYPE_COLLISION GamePhysicsMakeCollision(	bool Obj1Dynamic,
											TYPE_VECTOR * ptrObj1Position,
											TYPE_VECTOR * ptrObj1Speed,
											bool Obj2Dynamic,
											TYPE_VECTOR * ptrObj2Position,
											TYPE_VECTOR * ptrObj2Speed,
											fix16_t intersectionDistance,
											fix16_t bounceCoeficient	)
{
    TYPE_COLLISION collision;
    
    collision.Obj1Dynamic = Obj1Dynamic;
    collision.Obj2Dynamic = Obj2Dynamic;
    collision.ptrObj1Position = ptrObj1Position;
    collision.ptrObj2Position = ptrObj2Position;
    collision.ptrObj1Speed = ptrObj1Speed;
    collision.ptrObj2Speed = ptrObj2Speed;
    collision.intersectionDistance = intersectionDistance;
    collision.bounceCoeficient = bounceCoeficient;
    
    return collision;
}

void GamePhysicsBallHandler(TYPE_PLAYER * ptrPlayer)
{
	if(ptrPlayer->PadKeyReleased_Callback(PAD_CROSS) == true)
	{
		if(ptrPlayer->StateOnWater == true)
		{
			ptrPlayer->StateOnWater = false;
			ptrPlayer->speed.y = GAMEPHYSICS_LAUNCH_SPEED; // Jump!!
		}
	}
	else if(	(ptrPlayer->PadKeyReleased_Callback(PAD_CIRCLE) == true)
										&&
				(ptrPlayer->StateTackle == false)	)
	{
		if(ptrPlayer->PadKeyPressed_Callback(PAD_LEFT) == true)
		{
			ptrPlayer->StateTackle = true;
			ptrPlayer->speed.x = -GAMEPHYSICS_TACKLE_SPEED;
		}
		else if(ptrPlayer->PadKeyPressed_Callback(PAD_RIGHT) == true)
		{
			ptrPlayer->StateTackle = true;
			ptrPlayer->speed.x = GAMEPHYSICS_TACKLE_SPEED;
		}
		
	}
	
	if(	(ptrPlayer->StateTackle == false)
						&&
		(ptrPlayer->StateOnWater == false) )
	{
		if(ptrPlayer->PadKeyPressed_Callback(PAD_LEFT) == true)
		{
			if(ptrPlayer->speed.x > -GAMEPHYSICS_MAX_NORMAL_SPEED)
			{
				ptrPlayer->speed.x -= GAMEPHYSICS_NORMAL_SPEED;
			}
		}
		else if(ptrPlayer->PadKeyPressed_Callback(PAD_RIGHT) == true)
		{
			if(ptrPlayer->speed.x < GAMEPHYSICS_MAX_NORMAL_SPEED)
			{
				ptrPlayer->speed.x += GAMEPHYSICS_NORMAL_SPEED;
			}
		}
	}
	
	GamePhysicsApplyGravity(ptrPlayer);
	GamePhysicsTackleHandler(ptrPlayer);
}

void GamePhysicsApplyGravity(TYPE_PLAYER * ptrPlayer)
{
	ptrPlayer->speed.y +=GAMEPHYSICS_GRAVITY_ACCELERATION;
	
	if(	(ptrPlayer->position.y + ptrPlayer->speed.y) < (fix16_from_int(240) - ptrPlayer->radius) )
	{
		ptrPlayer->position.y += ptrPlayer->speed.y;
	}
	else
	{
		ptrPlayer->position.y = (fix16_from_int(240) - ptrPlayer->radius);
		ptrPlayer->speed.x = 0;
		
		if(ptrPlayer->StateTackle == true)
		{
			dprintf("Was true, set to false!\n");
		}
		
		ptrPlayer->StateTackle = false;
		ptrPlayer->StateOnWater = true;
	}
}

void GamePhysicsCheckCollisions(TYPE_PLAYER * ptrPlayer1, TYPE_PLAYER * ptrPlayer2)
{
	int i;
    int j;
    TYPE_PLAYER * ptrPlayer;
    TYPE_WAVE * ptrWave;
    TYPE_VECTOR diff; // Unused?
    fix16_t dist = GamePhysicsVectorDist(&ptrPlayer1->position, &ptrPlayer2->position);
    
    num_collisions = 0;
    dist -= ptrPlayer1->radius + ptrPlayer2->radius;
    
    if (dist < 0)
    {
		collisions[num_collisions++] = GamePhysicsMakeCollision(true,
																&ptrPlayer1->position,
																&ptrPlayer1->speed,
																true,
																&ptrPlayer2->position,
																&ptrPlayer2->speed,
																dist,
																BOUNCE_COEF_BALLS);
    }
    
    for (j = 0; j < MAX_PLAYERS; ++j)
    {
		ptrPlayer = &PlayerData[j];
	
		for (i = 0; i < MAX_WAVES; ++i)
		{
			ptrWave = &WaveData[i];
			
			diff = GamePhysicsVectorDiff(&ptrPlayer->position, &ptrWave->position);
			
			dprintf("diff\n", diff);
			
			// TODO: stuff
		}
    }
}

void GamePhysicsWaveHandler(TYPE_WAVE * ptrWave)
{
	fix16_t next_speed;
	fix16_t max_random_point;
	
	bool sign = SystemRand(false, true);
	
	if(sign == true)
	{
		max_random_point = -fix16_from_int(SystemRand(0, 8));
	}
	else
	{
		max_random_point = fix16_from_int(SystemRand(0, 8));
	}
	
	if(ptrWave->decrease == true)
	{
		next_speed = SystemRand(0, 0x800);
	}
	else
	{
		next_speed = -SystemRand(0, 0x800);
	}
	
	ptrWave->speed.y += next_speed;
	
	if(ptrWave->speed.y >= 0x20000)
	{
		ptrWave->decrease = false;
		ptrWave->speed.y -= 0x800;
	}
	else if(ptrWave->speed.y <= -0x20000)
	{
		ptrWave->decrease = true;
		ptrWave->speed.y += 0x800;
	}
	
	if((ptrWave->position.y + ptrWave->speed.y) < fix16_from_int(ptrWave->min_value + max_random_point) )
	{
		ptrWave->decrease = true;
		ptrWave->speed.y = 0;
		return;
	}
	
	if((ptrWave->position.y + ptrWave->speed.y) > fix16_from_int(ptrWave->max_value) )
	{
		ptrWave->speed.y = 0;
		ptrWave->decrease = false;
		return;
	}
	

	ptrWave->position.y += ptrWave->speed.y;
}

void GamePhysicsPerformCollisions(void)
{
	
}

void GamePhysicsTackleHandler(TYPE_PLAYER * ptrPlayer)
{
	if(ptrPlayer->speed.x < 0)
	{
		if( (ptrPlayer->speed.x + GAMEPHYSICS_WIND_DECELERATION) < 0)
		{
			ptrPlayer->speed.x += GAMEPHYSICS_WIND_DECELERATION;
		}
		else
		{
			ptrPlayer->speed.x = 0;
		}
	}
	else if(ptrPlayer->speed.x > 0) // Ignore "== 0" case!!
	{		
		if( (ptrPlayer->speed.x - GAMEPHYSICS_WIND_DECELERATION) > 0)
		{
			ptrPlayer->speed.x -= GAMEPHYSICS_WIND_DECELERATION;

		}
		else
		{
			ptrPlayer->speed.x = 0;
		}
	}
	
	if(	(ptrPlayer->position.x + ptrPlayer->speed.x) < 0
									||
			(ptrPlayer->position.x + ptrPlayer->speed.x) > fix16_from_int(LEVEL_X_SIZE) )
	{
		// Verify out of map
		dprintf("Player dead!\n");
		
		if(ptrPlayer->StateTackle == true)
		{
			dprintf("Was true, now false!\n");
		}
		
		ptrPlayer->StateTackle = false;
		ptrPlayer->speed.x = 0;
		return;
	}
	
	ptrPlayer->position.x += ptrPlayer->speed.x;
}
