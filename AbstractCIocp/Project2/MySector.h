#pragma once
#include"GameObject.h"
#include<deque>
#include<vector>

class MySector:public MemPool<MySector,1024>
{
private:
	std::vector<Vector2> Poly;
	std::deque<GameObject*> ObjectList;
public:
	bool isObjectInside(GameObject* obj);

	bool isObjectStillInside(GameObject* obj);
};