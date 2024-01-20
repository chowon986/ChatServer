// class.cpp

#include <iostream>
using namespace std;

class Parent
{
public:
	Parent() 
	{
		num = 0;
		arr = nullptr;
	}
	~Parent() {}

	virtual void Update()
	{
		cout << "부모의 업데이트 함수" << endl;
	}

	virtual void Update2() = 0;

	int num;
	int* arr;
};

class Child : public Parent
{
public:
	Child() {}
	~Child() {}

	void Update()
	{
		cout << "자식1의 업데이트 함수" << endl;
	}
	
	void Update2() {}

};

class Child2 : public Parent
{
public:
	Child2() {}
	~Child2() {}

	void Update()
	{
		cout << "자식2의 업데이트 함수" << endl;
	}

	void Update2() {}

};

class Child3 : public Parent
{
public:
	Child3() {}
	~Child3() {}

	void Update()
	{
		cout << "자식3의 업데이트 함수" << endl;
	}

	void Update2() {}
};

int main()
{
	Child obj;
	Child* p = &obj;

	//Parent objParent;
	//Parent* p2 = &objParent;

	Parent* p3 = &obj; 
	// 부모 포인터에 자식 클래스의 값을 저장할 수 있음
	// 왜 사용할까?
	
	Child a;
	Child2 b;
	Child3 c;

	Parent* list[3];
	list[0] = &a;
	list[1] = &b;
	list[2] = &c;

	for (int i = 0; i < 3; i++)
	{
		list[i]->Update();
	}
}

// 가상 함수
// 함수의 호출 위치를 런타임시에 결정하는 함수
// 가상함수테이블이 생기면 객체가 생성될 때 실행해야 하는 위치를 저장함
// (update는 자식꺼 호출해야해~)
// 다형성을 구성하는 부모클래스의 함수에 적용해야 함

// 부모 클래스의 포인터로 가상함수를 호출하면
// 객체에 있는 가상함수테이블을 확인해서 함수를 호출함

// abstract 구현부가 조금 있으면 추상 클래스
// interface 순수가상함수들만 있으면