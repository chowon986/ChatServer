#include "Monster.h"

Monster::Monster()
{
	m_HP = 100;
	m_Atk = 5;
}

void Monster::Attck(int* _hp, int* _atk, int input)
{
	*_hp -= m_Atk;
}

void Monster::Heal(int input)
{
	m_HP += m_Atk;
}
