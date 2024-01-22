// main.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>

int main()
{
    std::cout << "Hello World!\n";
}

// 프로그램 실행: <Ctrl+F5> 또는 [디버그] > [디버깅하지 않고 시작] 메뉴
// 프로그램 디버그: <F5> 키 또는 [디버그] > [디버깅 시작] 메뉴

// 시작을 위한 팁: 
//   1. [솔루션 탐색기] 창을 사용하여 파일을 추가/관리합니다.
//   2. [팀 탐색기] 창을 사용하여 소스 제어에 연결합니다.
//   3. [출력] 창을 사용하여 빌드 출력 및 기타 메시지를 확인합니다.
//   4. [오류 목록] 창을 사용하여 오류를 봅니다.
//   5. [프로젝트] > [새 항목 추가]로 이동하여 새 코드 파일을 만들거나, [프로젝트] > [기존 항목 추가]로 이동하여 기존 코드 파일을 프로젝트에 추가합니다.
//   6. 나중에 이 프로젝트를 다시 열려면 [파일] > [열기] > [프로젝트]로 이동하고 .sln 파일을 선택합니다.
#include <iostream>
#include <random>
#include <string>
#include "Monster.h"
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


}