#include "�迵��.h"

using namespace std;

�迵��::�迵��()
{
	name = "�迵��";
	m_HP = 50;
	m_Atk = 2;
}

void �迵��::Attck(int* _hp, int* _atk, int input)
{
	cout << m_Atk << "��ŭ�� ���ظ� �־���." << endl;
	*_hp -= m_Atk;
}

void �迵��::Heal(int* _hp, int* _atk, int input)
{
	cout << name << "�� " << m_Atk << "��ŭ ȸ���ߴ�!" << endl;
	m_HP += m_Atk;
}
