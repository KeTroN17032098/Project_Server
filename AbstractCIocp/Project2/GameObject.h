#pragma once
#include"BasicThing.h"
#include"MemPool.h"

class GameObject:public MemPool<GameObject,1024>
{
private:
	int objectid;
	Vector2 Pos;
	Vector2 LookAt;
	
};