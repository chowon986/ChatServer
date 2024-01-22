#include <iostream>
#include <random>
#include <string>
#include "Monster.h"
#include "Monster4.h"
#include "김영석.h"

using namespace std;

/*
	상태패턴

*/

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

	Monster4* Mon = new Monster4();
}