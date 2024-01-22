#pragma once

#include "Monster.h"

class 김영석 : public Monster
{
public:
	김영석();

	// int input = 0는 디폴트매개변수
	virtual void Attck(int* _hp, int* _atk, int input = 0);
	virtual void Heal(int* _hp, int* _atk, int input = 0);
};