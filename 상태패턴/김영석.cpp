#include "김영석.h"

using namespace std;

김영석::김영석()
{
	name = "김영석";
	m_HP = 50;
	m_Atk = 2;
}

void 김영석::Attck(int* _hp, int* _atk, int input)
{
	cout << m_Atk << "만큼의 피해를 주었다." << endl;
	*_hp -= m_Atk;
}

void 김영석::Heal(int* _hp, int* _atk, int input)
{
	cout << name << "은 " << m_Atk << "만큼 회복했다!" << endl;
	m_HP += m_Atk;
}
