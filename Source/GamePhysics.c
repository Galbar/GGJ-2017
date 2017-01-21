/* **************************************
 * 	Includes							*
 * **************************************/

#include "GamePhysics.h"

/* **************************************
 * 	Defines								*
 * **************************************/

#define BOUNCE_COEF_BALLS 0xCCCC // 0.8
#define BOUNCE_COEF_WAVES 0x4CCC // 0.3
#define GAMEPHYSICS_GRAVITY_ACCELERATION 0x20000 // 2.00
#define GAMEPHYSICS_LAUNCH_SPEED 0x80000 // 8.00

#define GAMEPHYSICS_INITIAL_WAVE_SPEED 0x20000

/* **************************************
 * 	Local variables						*
 * **************************************/

static TYPE_COLLISION collisions[MAX_POSSIBLE_COLLISIONS];
static uint8_t num_collisions;

void GamePhysicsApplyGravity(TYPE_PLAYER * ptrPlayer)
{
	ptrPlayer->speed.y += GAMEPHYSICS_GRAVITY_ACCELERATION;
}

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

void GamePhysicsLaunchBall(TYPE_PLAYER * ptrPlayer)
{
	if(ptrPlayer->PadKeyReleased_Callback(PAD_CROSS) == true)
	{
		if(ptrPlayer->StateOnWater == true)
		{
			ptrPlayer->StateOnWater = false;
			ptrPlayer->speed.y = GAMEPHYSICS_LAUNCH_SPEED; // Jump!!
		}
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
	uint8_t sign = SystemRand(0, 100);
	fix16_t next_speed;
	
	if(sign > 50)
	{
		next_speed = SystemRand(0, 0x20000);
	}
	else
	{
		next_speed = -SystemRand(0, 0x20000);
	}
	
	ptrWave->speed.y += next_speed;
	
	if(ptrWave->speed.y > 131072)
	{
		ptrWave->speed.y -= 0x20000;
	}
	else if(ptrWave->speed.y < -131072)
	{
		ptrWave->speed.y += 0x20000;
	}
	
	dprintf("next_speed = %d\n", fix16_to_int(next_speed));
	
	if((ptrWave->position.y + ptrWave->speed.y) < fix16_from_int(240-64) )
	{
		return;
	}
	
	if((ptrWave->position.y + ptrWave->speed.y) > fix16_from_int(240-16) )
	{
		return;
	}
	

	ptrWave->position.y += ptrWave->speed.y;
}

void GamePhysicsPerformCollisions(void)
{
	
}
