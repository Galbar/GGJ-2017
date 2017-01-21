/* **************************************
 * 	Includes							*
 * **************************************/
 
#include "Game.h"
#include "GameStructures.h"

/* **************************************
 * 	Defines								*
 * **************************************/
 
#define MAX_POSSIBLE_COLLISIONS 7

/* **************************************
 * 	Global prototypes					*
 * **************************************/
 
 
void GamePhysicsInit(void);
TYPE_VECTOR GamePhysicsVectorDiff(TYPE_VECTOR * ptrVector1, TYPE_VECTOR * ptrVector2);
TYPE_VECTOR GamePhysicsVectorMul(TYPE_VECTOR * ptrVector1, TYPE_VECTOR * ptrVector2);
fix16_t GamePhysicsVectorDist(TYPE_VECTOR * ptrVector1, TYPE_VECTOR * ptrVector2);
TYPE_COLLISION GamePhysicsMakeCollision(	bool Obj1Dynamic,
											TYPE_VECTOR * ptrObj1Position,
											TYPE_VECTOR * ptrObj1Speed,
											bool Obj2Dynamic,
											TYPE_VECTOR * ptrObj2Position,
											TYPE_VECTOR * ptrObj2Speed,
											fix16_t intersectionDistance,
											fix16_t bounceCoeficient	);
											
void GamePhysicsCheckCollisions(TYPE_PLAYER * ptrPlayer1, TYPE_PLAYER * ptrPlayer2);
void GamePhysicsApplyGravity(TYPE_PLAYER * ptrPlayer);
void GamePhysicsWaveHandler(TYPE_WAVE * ptrWave);
