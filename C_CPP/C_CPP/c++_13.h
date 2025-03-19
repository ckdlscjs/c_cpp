#pragma once
#include "std.h"
namespace ch13
{
	/*
	* c++에서 동적할당을 이용해 획득한 자원은 직접 해제 하지 않는이상 프로그램이 종료되기전까지 영원히 남아있는다, 프로그램이 종료될때에는 운영체제가 해제하지만
	* 사용하지않고있는 메모리가 계속해서 남아있기때문에 메모리릭이 발생하고있다
	*/
	class A
	{
		int* data;
	public:
		A()
		{
			std::cout << "메모리공간에 동적할당" << '\n';
			data = new int[100]; //data에 int형 정수 100개짜리 배열을 동적할당함
		}
		virtual ~A()
		{
			std::cout << "소멸자호출, 자원을해제" << '\n';
			delete[] data;
		}
	};

	void thrower()
	{
		A* pa = new A();
		throw 1;
		//예외처리로 인해 delete pa 에 도달하지 못하고 pa에 동적할당한 A객체와 A객체의생성자에서 data에 동적할당시킨 영역또한 해제가 불가능함
		delete pa;
	}

	void Exam()
	{
		try
		{
			thrower();
		}
		catch (int i)
		{
			std::cout << "예외발생!" << '\n';
		}
	}

	/*
	* RAII(Resourcr Acquisition is initialization), 자원의 획득은 초기화다, 자원관리를 스택에 할당한 객체를 통해 수행한다
	* pa의경우 A객체를 가리키는 포인터이기 때문에 소멸자가 호출되지 않는다, pa를 일반적인 포인터가 아닌 포인터 "객체"로 만들어서 객체가 소멸될 때
	* 자신이 가리키는 데이터도 같이 delete하게 만들면 자원관리를 스택의객체를 통해 수행하게 되므로 해결 가능 하다
	* c++11에서는 unique_ptr, shared_ptr로 스마트포인터를 제공한다
	* 일반적인 경우에는 디자인패턴의 일환인 RAII패턴을 사용해 사용이 끝난 메모리를 해제시켜버리면 릭문제를 해결 가능 하나 이미 해제된 메모리를 다시 해제하려하는 더블프리(이중해제)등의
	* 문제를 방지 하기는 힘들다, 고로 만들어진 객체의 소유권이 명확하지 않음이 문제를 야기시키는데 어떠한 포인터에 객체의 유일한 소유권을 지정하여 해당 포인터이외엔 지정객체를
	* 소멸 시킬수 없게하는 제한을 건다면 이중해제같은 문제가 생기지 않는다
	*/
	class A2
	{
		int* data;
	public:
		A2()
		{
			std::cout << "메모리공간에 동적할당" << '\n';
			data = new int[100]; //data에 int형 정수 100개짜리 배열을 동적할당함
		}
		virtual ~A2()
		{
			std::cout << "소멸자호출, 자원을해제" << '\n';
			delete[] data;
		}
		void TestFunc()
		{
			std::cout << "함수" << '\n';
		}
	};
	void Exam2()
	{
		std::unique_ptr<A2> pa(new A2());
		pa->TestFunc();
		//std::unique_ptr<A2> pa2 = pa; //컴파일오류, 삭제된함수를 참조하려한다
	}
	//삭제된함수는 명시적으로 함수의 선언문 옆에 = delete;를 선언함으로써 사용을 금지시킨 함수이다, unique_ptr의 복사생성자는 명시적으로 삭제되어있다 이는
	//컴파일러가 default하게 선언되지않은 복사생성자를 생성하여 참조하는 객체를 복사시키려는 unique_ptr의 복사생성자를 명시적으로 삭제시켜 복사를 불가하게끔 만든것
	/*
	* unique_ptr은 객체의 소유권을 가질뿐 실제 객체에 다른 접근이 불가하게끔 막는 역할이아니다
	* 고로 해당 unique_ptr이 가리키는 객체에 접근하려면 unique_ptr변수.get()으로 실제 객체에 접근하여 필요한 연산등을 하는행위는 가능하다
	*/
	void Exam3()
	{
		std::unique_ptr<A2> pa(new A2());
		pa->TestFunc();

		std::unique_ptr<A2> pb = std::move(pa); //move를 이용해 rvalue로 변환시킨객체를 이동생성자를 이용해 소유권을 이전시킴은 가능하다, 이후pa는 nullptr이되므로 참조시 댕글링포인터현상을발생한다
		A2* tempptr = pb.get();
		tempptr->TestFunc();

		std::unique_ptr<A2> pc = std::make_unique<A2>(); //perfectForward로 &&를 이용해 완벽한전달을 이용, 생성자의 매개변수를 그대로 전달한다

		std::vector<std::unique_ptr<A2>> vec;
		vec.push_back(std::make_unique<A2>()); //push_back최적화, 기본적인경우는 emplace_back이용(push_back은 기본적으로 완벽한전달을 통하지않고 복사를이용해 객체를 집어넣는다)
	}

	/*
	* 때에 따라서 여러개의 스마트포인터가 하나의 객체를 같이 소유해야하는 경우가 있다, 후에 자원을 사용하기위해 객체의소멸등 어느타이밍에 
	* 해제해야할지 추적하면서 그 수가 0이되어야 비로소 해제를 시켜주는 방식의 포인터가 필요하다, 이를위해 shared_ptr을 사용한다
	* 
	*/
}