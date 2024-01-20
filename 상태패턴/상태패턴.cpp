#include <iostream>
#include <random>
#include <string>
using namespace std;

/*
	상태패턴

*/

class Monster
{
public:
	string name;
	int m_HP;
	int m_Atk;

	Monster()
	{
		m_HP = 100;
		m_Atk = 5;
	}

	// int input = 0는 디폴트매개변수
	virtual void Attck(int* _hp, int* _atk, int input = 0)
	{
		*_hp -= m_Atk;
	}

	virtual void Heal(int input = 0)
	{
		m_HP += m_Atk;
	}
};

class 김영석 : public Monster
{
public:
	김영석()
	{
		name = "김영석";
		m_HP = 50;
		m_Atk = 2;
	}

	// int input = 0는 디폴트매개변수
	virtual void Attck(int* _hp, int* _atk, int input = 0)
	{
		cout << m_Atk << "만큼의 피해를 주었다." << endl;
		*_hp -= m_Atk;
	}

	virtual void Heal(int* _hp, int* _atk, int input = 0)
	{
		cout << name << "은 " << m_Atk << "만큼 회복했다!" << endl;
		m_HP += m_Atk;
	}
};

int main()
{
	srand(time(nullptr));

	int nHP = 100;
	int nAtk = 10;

	// 몬스터 만남
	Monster dummy;
	김영석* monster1 = new 김영석();

	int nInput{};
	int nMonsterInput{};

	cout << "야생의 " << monster1->name << "가 나타났다." << endl;

	while (true)
	{
		cout << "HP : " << nHP << '\n';
		cout << "Monster HP : " << monster1->m_HP << '\n';

		if (nHP <= 0)
		{
			cout << "패배" << '\n';
			break;
		}
		else if (monster1->m_HP <= 0)
		{
			cout << "승리" << '\n';
			break;
		}

		cout << "1. 공격 2. 회복" << '\n';
		cout << "입력 : ";

		int nInput{};
		cin >> nInput;

		switch (nInput)
		{
		case 1:
			cout << "플레이어가 공격합니다." << '\n';
			monster1->m_HP -= nAtk;
			break;
		case 2:
			cout << "플레이어가 회복합니다." << '\n';
			nHP += nAtk;
			break;
		}

		nMonsterInput = rand() % 2 + 1;

		switch (nMonsterInput)
		{
		case 1:
			monster1->Attck(&nHP, &nAtk);
			break;
		case 2:
			monster1->Heal(&nHP, &nAtk);
			break;
		}

		system("pause");
		system("cls");
	}


}