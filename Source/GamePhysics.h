#include "Game.h"
#include "GameStructures.h"

typedef struct t_collision
{
    bool Obj1Dynamic;
    bool Obj2Dynamic;
    TYPE_VECTOR * ptrObj1Position;
    TYPE_VECTOR * ptrObj2Position;
    TYPE_VECTOR * ptrObj1Speed;
    TYPE_VECTOR * ptrObj2Speed;
    fix16_t intersectionDistance;
    fix16_t bounceCoeficient;
}TYPE_COLLISION;

#define BOUNCE_COEF_BALLS 0xCCCC // 0.8
#define BOUNCE_COEF_WAVES 0x4CCC // 0.3
#define MAX_POSSIBLE_COLLISIONS 7
static TYPE_COLLISION collisions[MAX_POSSIBLE_COLLISIONS]
static uint8_t num_collisions;

TYPE_VECTOR PhysicsVectorDiff(TYPE_VECTOR * ptrVector1, TYPE_VECTOR * ptrVector2)
{
    TYPE_VECTOR diff;
    diff.x = ptrVector1->x - ptrVector2->x;
    diff.y = ptrVector1->y - ptrVector2->y;
    return diff;
}

TYPE_VECTOR PhysicsVectorMul(TYPE_VECTOR * ptrVector1, TYPE_VECTOR * ptrVector2)
{
    TYPE_VECTOR mul;
    mul.x = ptrVector1->x * ptrVector2->x;
    mul.y = ptrVector1->y * ptrVector2->y;
    return mul;
}

fix16_t PhysicsVectorDist(TYPE_VECTOR * ptrVector1, TYPE_VECTOR * ptrVector2)
{
    TYPE_VECTOR dist = PhysicsVectorDiff(ptrVector1, ptrPlayer2);
    return fix16_sqrt(fix16_mul(dist.x, dist.x) + fix16_mul(dist.y, dist.y));
}

TYPE_COLLISION PhysicsMakeCollision(bool Obj1Dynamic, TYPE_VECTOR * ptrObj1Position, TYPE_VECTOR * ptrObj1Speed,
				    bool Obj2Dynamic, TYPE_VECTOR * ptrObj2Position, TYPE_VECTOR * ptrObj2Speed,
				    fix16_t intersectionDistance, fix16_t bounceCoeficient)
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

void PhysicsCheckCollisions(TYPE_PLAYER * ptrPlayer1, TYPE_PLAYER * ptrPlayer2)
{
    num_collisions = 0;
    fix16_t dist = PhysicsVectorDist(ptrPlayer1->position, ptrPlayer2->position);
    dist -= ptrPlayer1->radius + ptrPlayer2->radius;
    if (dist < 0)
    {
	collisions[num_collisions++] = PhysicsMakeCollision(true, ptrPlayer1->position, ptrPlayer1->speed,
							    true, ptrPlayer2->position, ptrPlayer2->speed,
							    dist, BOUNCE_COEF_BALLS);
    }

    int i;
    int j;
    for (j = 0; j < MAX_PLAYERS; ++j)
    {
	TYPE_PLAYER * ptrPlayer = &PlayerData[j];
	for (i = 0; i < MAX_WAVES; ++i)
	{
	    TYPE_WAVE * ptrWave = &WaveData[i];
	    TYPE_VECTOR diff = PhysicsVectorDiff(ptrPlayer->position, ptrWave->position);
// TODO: stuff
	}
    }
}

void PhysicsPerformCollisions()
