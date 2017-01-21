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

/* **************************************
 * 	Local variables						*
 * **************************************/

static TYPE_COLLISION collisions[MAX_POSSIBLE_COLLISIONS];
static uint8_t num_collisions;

void GamePhysicsApplyGravity(TYPE_PLAYER * ptrPlayer)
{
	ptrPlayer->speed.y += GAMEPHYSICS_GRAVITY_ACCELERATION;
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

void GamePhysicsCheckCollisions(TYPE_PLAYER * ptrPlayer1, TYPE_PLAYER * ptrPlayer2)
{
	int i;
    int j;
    TYPE_PLAYER * ptrPlayer;
    TYPE_WAVE * ptrWave;
    //TYPE_VECTOR diff; // Unused?
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
			
			// TODO: stuff
		}
    }
}

void GamePhysicsPerformCollisions(void)
{
	
}
