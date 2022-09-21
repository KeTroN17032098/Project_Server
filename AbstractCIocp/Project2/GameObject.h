#pragma once
#include"BasicThing.h"
#include"MemPool.h"

enum class GameObjectType
{
	NONE=0,//타입 불명
	PLAYER,//플레이어
	INTERACTABLE,//상호작용 가능
	WALL//장애물 및 벽
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
	float Collider[4];
public:
	GameObject(int oid)
	{
		objectid = oid;
		type = GameObjectType::NONE;
		ZeroMemory(&Pos, sizeof(Vector2));
		ZeroMemory(&LookAt, sizeof(Vector2));
		ZeroMemory(Collider, sizeof(float)*4);
	}

	void setCollider(float x, float y)
	{
		Collider[0] = Pos.x - x/2;
		Collider[1] = Pos.x + x/2;
		Collider[2] = Pos.y - y / 2;
		Collider[3] = Pos.y + y / 2;
	}
	
	void setLookAt(int direction)
	{
		if(direction.)
	}

	void Moveto(float x, float y)
	{
		Pos.x = x;
		Pos.y = y;
	}

};