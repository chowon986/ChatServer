#pragma once
#include <string>
using namespace std;

class Monster
{
public:
	string name;
	int m_HP;
	int m_Atk;

	Monster();

	// int input = 0는 디폴트매개변수
	virtual void Attck(int* _hp, int* _atk, int input = 0);

	virtual void Heal(int input = 0);
};