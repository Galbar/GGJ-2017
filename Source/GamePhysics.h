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

#define MAX_POSSIBLE_COLLISIONS 7
static TYPE_COLLISION collisions[MAX_POSSIBLE_COLLISIONS]

fix16_t PhysicsVectorDist(TYPE_VECTOR * ptrVector1, TYPE_VECTOR * ptrVector2)
{
    TYPE_VECTOR dist;
    dist.x = ptrVector1->x - ptrVector2->x;
    dist.y = ptrVector1->y - ptrVector2->y;
    return fix16_sqrt(fix16_mul(dist.x, dist.x) + fix16_mul(dist.y, dist.y));
}

bool PhysicsCheckCollisions(TYPE_PLAYER * ptrPlayer1, TYPE_PLAYER * ptrPlayer2)
{
    fix16_t dist = PhysicsVectorDist(ptrPlayer1->position, ptrPlayer2->position);
    if (dist < 0)
    {
	return true;
    }
    return false;
}

void PhysicsPerformCollisionPlayers()
