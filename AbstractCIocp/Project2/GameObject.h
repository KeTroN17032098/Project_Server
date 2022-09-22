#pragma once
#include"BasicThing.h"
#include"MemPool.h"
#include<map>

enum class GameObjectType
{
	NONE=1,//타입 불명
	PLAYER,//플레이어
	INTERACTABLE,//상호작용 가능
	WALL//장애물 및 벽
};


enum class JobType
{
	NONE=1,
	CITIZEN,
	MAFIA,
	MERCHANARY
};

enum class JobDetail
{
	NONE=0,
	NORMAL_CITIZEN,
	HUNTER_CITIZEN,
	VIGILANTE_CITIZEN,
	STAR_CITIZEN,
	EXOCIST_CITIZEN,
	CANADA_CITIZEN,
	MECHANIST_CITIZEN,
	LOVER_CITIZEN,
	PRETENDER_CITIZEN,
	ETC_CITIZEN,
	NORMAL_MAFIA,
	ASSASSIN_MAFIA,
	PRO_MAFIA,
	LOVER_MAFIA,
	COSPLAY_MAFIA,
	ETC_MAFIA,
	ANGEL,
	FLU,
	SERIALKILLER
};

enum class EWSN
{
	EAST = 1,
	WEST = 1 << 1,
	SOUTH=1<<2,
	NORTH=1<<3
};

class GameObject:public MemPool<GameObject,8192>
{
private:
	int objectid;
	GameObjectType type;
	Vector2 Pos;
	Vector2 LookAt;

public:
	GameObject(int oid,GameObjectType type)
	{
		objectid = oid;
		this->type = type;
		ZeroMemory(&Pos, sizeof(Vector2));
		ZeroMemory(&LookAt, sizeof(Vector2));
	}

	int getOID()
	{
		return objectid;
	}

	GameObjectType getOTYPE()
	{
		return type;
	}

	Vector2 getPosition()
	{
		return Pos;
	}
	Vector2 GetLookAt()
	{
		return LookAt;
	}


	
	void setLookAt(int direction)
	{
		float k = 0;
		if (direction && static_cast<int>(EWSN::EAST) != 0)
		{
			k += 1.0f;
		}
		if (direction && static_cast<int>(EWSN::WEST) != 0)
		{
			k -= 1.0f;
		}
		LookAt.x = k;
	}

	void Moveto(float x, float y)
	{
		Pos.x = x;
		Pos.y = y;
	}


};