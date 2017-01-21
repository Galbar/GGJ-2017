/* **************************************
 * 	Includes							*
 * **************************************/

#include "GamePhysics.h"

/* **************************************
 * 	Defines								*
 * **************************************/

#define BOUNCE_COEF_BALLS 0xCCCC // 0.8
#define BOUNCE_COEF_WAVES 0x4CCC // 0.3
#define FRICT_COEF_BALLS 0xCCCC // 0.8
#define FRICT_COEF_WAVES 0x4CCC // 0.3
#define GAMEPHYSICS_GRAVITY_ACCELERATION 0x20000 // 2.00
#define GAMEPHYSICS_LAUNCH_SPEED 0x80000 // 8.00

#define GAMEPHYSICS_INITIAL_WAVE_SPEED 0x20000

/* **************************************
 * 	Local variables						*
 * **************************************/

static TYPE_COLLISION collisions[MAX_POSSIBLE_COLLISIONS];
static TYPE_POINT_INFO point_infos[MAX_POSSIBLE_COLLISIONS - 1]; // -1 because collision between balls don't need it
static uint8_t num_collisions;
static uint8_t num_point_infos;

void GamePhysicsInit(void)
{
	bzero((TYPE_WAVE*)&WaveData, sizeof(TYPE_WAVE));

	WaveData[0].speed.y = GAMEPHYSICS_INITIAL_WAVE_SPEED;
}

void GamePhysicsApplyGravity(TYPE_PLAYER * ptrPlayer)
{
	ptrPlayer->speed.y += GAMEPHYSICS_GRAVITY_ACCELERATION;
}

TYPE_VECTOR GamePhysicsVectorEscMul(TYPE_VECTOR * ptrVector, fix16_t x)
{
	TYPE_VECTOR mul;

	mul.x = fix16_mul(ptrVector->x, x);
	mul.y = fix16_mul(ptrVector->y, x);

	return mul;
}

TYPE_VECTOR GamePhysicsVectorAdd(TYPE_VECTOR * ptrVector1, TYPE_VECTOR * ptrVector2)
{
	TYPE_VECTOR sum;

	sum.x = ptrVector1->x + ptrVector2->x;
	sum.y = ptrVector1->y + ptrVector2->y;

	return sum;
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

	mul.x = fix16_mul(ptrVector1->x, ptrVector2->x);
	mul.y = fix16_mul(ptrVector1->y, ptrVector2->y);

	return mul;
}

void GamePhysicsVectorNormalize(TYPE_VECTOR * ptrVector)
{
	fix16_t mag = GamePhysicsVectorMagnitude(ptrVector);
	ptrVector->x = fix16_div(ptrVector->x, mag);
	ptrVector->y = fix16_div(ptrVector->y, mag);
}

fix16_t GamePhysicsVectorDot(TYPE_VECTOR * ptrVector1, TYPE_VECTOR * ptrVector2)
{
	return fix16_mul(ptrVector1->x, ptrVector2->x) + fix16_mul(ptrVector1->y, ptrVector2->y);
}

fix16_t GamePhysicsVectorMagnitude(TYPE_VECTOR * ptrVector)
{
	return fix16_sqrt(fix16_mul(ptrVector->x, ptrVector->x) + fix16_mul(ptrVector->y, ptrVector->y));
}

fix16_t GamePhysicsVectorDist(TYPE_VECTOR * ptrVector1, TYPE_VECTOR * ptrVector2)
{
	TYPE_VECTOR d = GamePhysicsVectorDiff(ptrVector2, ptrVector1);
	return GamePhysicsVectorMagnitude(&d);
}

fix16_t GamePhysicsAngleBetweenVectors(TYPE_VECTOR * ptrA, TYPE_VECTOR * ptrB)
{
	fix16_t angle;
	angle = fix16_atan2(ptrB->y, ptrB->x) - fix16_atan2(ptrA->y, ptrA->x);
	if (angle < 0)
	{
		angle += fix16_pi << 1;
	}
	return angle;
}

TYPE_POINT_INFO * GamePhysicsMakePointInfo(TYPE_VECTOR * position, TYPE_VECTOR * speed)
{
	TYPE_POINT_INFO * info = &point_infos[num_point_infos++];
	info->position = *position;
	info->speed = *speed;
	return info;
}

void GamePhysicsDistABAndP(	TYPE_VECTOR * ptrA,
							TYPE_VECTOR * ptrB,
							TYPE_VECTOR * ptrP,
							fix16_t * ABPDist,
							TYPE_VECTOR * AX)
{
	TYPE_VECTOR AP = GamePhysicsVectorDiff(ptrP, ptrA);
	TYPE_VECTOR AB = GamePhysicsVectorDiff(ptrB, ptrA);
	fix16_t alpha = GamePhysicsAngleBetweenVectors(&AP, &AB);
	fix16_t x = fix16_mul(fix16_cos(alpha), GamePhysicsVectorMagnitude(&AP));
	*ABPDist = fix16_mul(fix16_sin(alpha), GamePhysicsVectorMagnitude(&AP));
	GamePhysicsVectorNormalize(&AB);
	AX->x = fix16_mul(AB.x, x);
	AX->y = fix16_mul(AB.y, x);
}

TYPE_COLLISION GamePhysicsMakeCollision(	bool Obj1Dynamic,
											TYPE_VECTOR * ptrObj1Position,
											TYPE_VECTOR * ptrObj1Speed,
											fix16_t obj1Radius,
											bool Obj2Dynamic,
											TYPE_VECTOR * ptrObj2Position,
											TYPE_VECTOR * ptrObj2Speed,
											fix16_t obj2Radius,
											fix16_t bounceCoeficient,
											fix16_t frictionCoeficient)
{
	TYPE_COLLISION collision;

	collision.Obj1Dynamic = Obj1Dynamic;
	collision.Obj2Dynamic = Obj2Dynamic;
	collision.ptrObj1Position = ptrObj1Position;
	collision.ptrObj2Position = ptrObj2Position;
	collision.ptrObj1Speed = ptrObj1Speed;
	collision.ptrObj2Speed = ptrObj2Speed;
	collision.obj1Radius = obj1Radius;
	collision.obj2Radius = obj2Radius;
	collision.bounceCoeficient = bounceCoeficient;
	collision.frictionCoeficient = frictionCoeficient;

	return collision;
}

bool GamePhysicsCollidePlayers(	TYPE_PLAYER * ptrPlayer1,
								TYPE_PLAYER * ptrPlayer2,
								TYPE_COLLISION * collision)
{
	fix16_t dist = GamePhysicsVectorDist(&ptrPlayer1->position, &ptrPlayer2->position);

	if (dist < 0)
	{
		*collision = GamePhysicsMakeCollision(	true,
												&ptrPlayer1->position,
												&ptrPlayer1->speed,
												ptrPlayer1->radius,
												true,
												&ptrPlayer2->position,
												&ptrPlayer2->speed,
												ptrPlayer2->radius,
												BOUNCE_COEF_BALLS,
												FRICT_COEF_BALLS);
		return true;
	}
	return false;
}

bool GamePhysicsCollidePlayerWithWave(	TYPE_PLAYER * ptrPlayer,
										TYPE_WAVE * ptrWaveA,
										TYPE_WAVE * ptrWaveB,
										TYPE_COLLISION * collision)
{
	TYPE_VECTOR AX;
	TYPE_VECTOR R;
	TYPE_VECTOR AB;
	TYPE_VECTOR nA;
	TYPE_VECTOR nB;
	TYPE_VECTOR nAB;
	TYPE_VECTOR nX;
	TYPE_VECTOR XnX;
	fix16_t distABAndP;

	GamePhysicsDistABAndP(	&ptrWaveA->position,
							&ptrWaveB->position,
							&ptrPlayer->position,
							&distABAndP,
							&AX);
	AB = GamePhysicsVectorDiff(	&ptrWaveB->position,
								&ptrWaveA->position);
	R = GamePhysicsVectorMul(&AX, &AB);
	// si X no esta sobre AB
	if (!((R.x < 0 || R.y < 0)
				||
				(GamePhysicsVectorMagnitude(&AX) > GamePhysicsVectorMagnitude(&AB))))
	{
		// si hi ha interseccio
		if (distABAndP < ptrPlayer->radius)
		{
			R = GamePhysicsVectorAdd(&ptrWaveA->position, &AX);

			nA = GamePhysicsVectorAdd(&ptrWaveA->position, &ptrWaveA->speed);
			nB = GamePhysicsVectorAdd(&ptrWaveB->position, &ptrWaveB->speed);
			nAB = GamePhysicsVectorDiff(&nB, &nA);
			GamePhysicsVectorNormalize(&nAB);
			nX = GamePhysicsVectorEscMul(&nAB, GamePhysicsVectorMagnitude(&AX));
			XnX = GamePhysicsVectorDiff(&nX, &R);
			TYPE_POINT_INFO * point_info = GamePhysicsMakePointInfo(&R, &XnX);
			*collision = GamePhysicsMakeCollision(	true,
													&ptrPlayer->position,
													&ptrPlayer->speed,
													ptrPlayer->radius,
													false,
													&point_info->position,
													&point_info->speed,
													0,
													BOUNCE_COEF_WAVES,
													FRICT_COEF_WAVES);
			return true;
		}
	}
	return false;
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

void GamePhysicsCheckCollisions()
{
	int i;
	int j;
	TYPE_PLAYER * ptrPlayer;
	TYPE_WAVE * ptrWaveA;
	TYPE_WAVE * ptrWaveB;
	num_collisions = 0;
	num_point_infos = 0;

	if(GamePhysicsCollidePlayers(&PlayerData[PLAYER_ONE], &PlayerData[PLAYER_TWO], &collisions[num_collisions]))
	{
		++num_collisions;
	}

	for (j = 0; j < MAX_PLAYERS && num_collisions < MAX_POSSIBLE_COLLISIONS; ++j)
	{
		ptrPlayer = &PlayerData[j];
		for (i = 0; i < MAX_WAVES && num_collisions < MAX_POSSIBLE_COLLISIONS; ++i)
		{
			ptrWaveA = &WaveData[i];
			ptrWaveB = &WaveData[i + 1];
			if (GamePhysicsCollidePlayerWithWave(	ptrPlayer,
													ptrWaveA,
													ptrWaveB,
													&collisions[num_collisions]))
			{
				++num_collisions;
			}
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

bool GamePhysicsResolveCollision(TYPE_COLLISION * collision)
{
	TYPE_VECTOR N = GamePhysicsVectorDiff(collision->ptrObj2Position, collision->ptrObj1Position);
	TYPE_VECTOR vNormal;
	TYPE_VECTOR vTangent;
	TYPE_VECTOR tmp;
	fix16_t dist = GamePhysicsVectorMagnitude(&N) - collision->obj1Radius + collision->obj2Radius;
	fix16_t angle;

	if (dist < 0)
	{
		if (collision->Obj1Dynamic && collision->Obj2Dynamic)
		{
			// tots dos son pilotes
		}
		else if (collision->Obj2Dynamic)
		{
			angle = GamePhysicsAngleBetweenVectors(&N, collision->ptrObj1Speed) - fix16_pi;

			vNormal = GamePhysicsVectorEscMul(collision->ptrObj1Speed, fix16_cos(angle));

			tmp = GamePhysicsVectorEscMul(collision->ptrObj2Speed, fix16_cos(angle));
			tmp = GamePhysicsVectorAdd(&vNormal, &tmp);

			vNormal = GamePhysicsVectorEscMul(&tmp, -collision->bounceCoeficient);

			vTangent = GamePhysicsVectorEscMul(collision->ptrObj1Speed, fix16_sin(angle));

			tmp = GamePhysicsVectorEscMul(collision->ptrObj2Speed, fix16_sin(angle));
			tmp = GamePhysicsVectorAdd(&vTangent, &tmp);

			vTangent = GamePhysicsVectorEscMul(&tmp, collision->frictionCoeficient);

			*collision->ptrObj1Position = GamePhysicsVectorAdd(collision->ptrObj1Position, &N);
			*collision->ptrObj1Speed = GamePhysicsVectorAdd(&vNormal, &vTangent);
		}
		return false;
	}
	return true;
}

void GamePhysicsResolveCollisions()
{
	int i;
	bool all_solved;

	do
	{
		all_solved = true;
		for (i = 0; i < num_collisions; ++i)
		{
			TYPE_COLLISION * collision = &collisions[i];
			all_solved = all_solved && GamePhysicsResolveCollision(collision);
		}
	} while(!all_solved);
}
