#pragma once

#include "Monster.h"

class �迵�� : public Monster
{
public:
	�迵��();

	// int input = 0�� ����Ʈ�Ű�����
	virtual void Attck(int* _hp, int* _atk, int input = 0);
	virtual void Heal(int* _hp, int* _atk, int input = 0);
};