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

TYPE_VECTOR GamePhysicsVectorEscMul(TYPE_VECTOR * ptrVector, fix16_t x);
TYPE_VECTOR GamePhysicsVectorDiff(TYPE_VECTOR * ptrVector1, TYPE_VECTOR * ptrVector2);
TYPE_VECTOR GamePhysicsVectorMul(TYPE_VECTOR * ptrVector1, TYPE_VECTOR * ptrVector2);
void GamePhysicsVectorNormalize(TYPE_VECTOR * ptrVector);
fix16_t GamePhysicsVectorDot(TYPE_VECTOR * ptrVector1, TYPE_VECTOR * ptrVector2);
fix16_t GamePhysicsVectorMagnitude(TYPE_VECTOR * ptrVector1);
fix16_t GamePhysicsVectorDist(TYPE_VECTOR * ptrVector1, TYPE_VECTOR * ptrVector2);
fix16_t GamePhysicsAngleBetweenVectors(TYPE_VECTOR * ptrVector1, TYPE_VECTOR * ptrVector2);
void GamePhysicsDistABAndP(	TYPE_VECTOR * ptrA,
								TYPE_VECTOR * ptrB,
								TYPE_VECTOR * ptrP,
								fix16_t * ABPDist,
								TYPE_VECTOR * AX);
TYPE_COLLISION GamePhysicsMakeCollision(	bool Obj1Dynamic,
											TYPE_VECTOR ptrObj1Position,
											TYPE_VECTOR ptrObj1Speed,
											bool Obj2Dynamic,
											TYPE_VECTOR ptrObj2Position,
											TYPE_VECTOR ptrObj2Speed,
											fix16_t intersectionDistance,
											fix16_t bounceCoeficient	);
bool GamePhysicsCollidePlayerWithWave(	TYPE_PLAYER * ptrPlayer,
										TYPE_WAVE * ptrWaveA,
										TYPE_WAVE * ptrWaveB,
										TYPE_COLLISION * collision);
void GamePhysicsCheckCollisions(TYPE_PLAYER * ptrPlayer1, TYPE_PLAYER * ptrPlayer2);
void GamePhysicsApplyGravity(TYPE_PLAYER * ptrPlayer);
