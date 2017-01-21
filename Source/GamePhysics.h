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
fix16_t GamePhysicsRoundToInt(fix16_t value);
void GamePhysicsVectorRoundToInt(TYPE_VECTOR * vector);
void GamePhysicsInit(void);
TYPE_VECTOR GamePhysicsVectorEscMul(TYPE_VECTOR * ptrVector, fix16_t x);
TYPE_VECTOR GamePhysicsVectorDiff(TYPE_VECTOR * ptrVector1, TYPE_VECTOR * ptrVector2);
TYPE_VECTOR GamePhysicsVectorMul(TYPE_VECTOR * ptrVector1, TYPE_VECTOR * ptrVector2);
void GamePhysicsVectorNormalize(TYPE_VECTOR * ptrVector);
fix16_t GamePhysicsVectorDot(TYPE_VECTOR * ptrVector1, TYPE_VECTOR * ptrVector2);
fix16_t GamePhysicsVectorMagnitude(TYPE_VECTOR * ptrVector1);
fix16_t GamePhysicsVectorDist(TYPE_VECTOR * ptrVector1, TYPE_VECTOR * ptrVector2);
TYPE_POINT_INFO * GamePhysicsMakePointInfo(TYPE_VECTOR * position, TYPE_VECTOR * speed);
fix16_t GamePhysicsAngleBetweenVectors(TYPE_VECTOR * ptrVector1, TYPE_VECTOR * ptrVector2);
void GamePhysicsDistABAndP(	TYPE_VECTOR * ptrA,
								TYPE_VECTOR * ptrB,
								TYPE_VECTOR * ptrP,
								fix16_t * ABPDist,
								TYPE_VECTOR * AX);
TYPE_COLLISION GamePhysicsMakeCollision(	bool Obj1Dynamic,
											TYPE_VECTOR * ptrObj1Position,
											TYPE_VECTOR * ptrObj1Speed,
											fix16_t obj1Radius,
											bool Obj2Dynamic,
											TYPE_VECTOR * ptrObj2Position,
											TYPE_VECTOR * ptrObj2Speed,
											fix16_t obj2Radius,
											fix16_t bounceCoeficient,
											fix16_t frictionCoeficient);

bool GamePhysicsCollidePlayers(	TYPE_PLAYER * ptrPlayer1,
								TYPE_PLAYER * ptrPlayer2,
								TYPE_COLLISION * collision);
bool GamePhysicsCollidePlayerWithWave(	TYPE_PLAYER * ptrPlayer,
										TYPE_WAVE * ptrWaveA,
										TYPE_WAVE * ptrWaveB,
										TYPE_COLLISION * collision);
void GamePhysicsCheckCollisions();
bool GamePhysicsResolveCollision(TYPE_COLLISION * collision);
void GamePhysicsResolveCollisions();
void GamePhysicsWaveHandler(TYPE_WAVE * ptrWave);
void GamePhysicsBallHandler(TYPE_PLAYER * ptrPlayer);

