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
		std::weak_ptr<A> other;
	public:
		A()
		{
			std::cout << "메모리공간에 동적할당" << '\n';
			data = new int[100]; //data에 int형 정수 100개짜리 배열을 동적할당함
		}
		void TestFunc()
		{
			std::cout << "함수" << '\n';
		}
		void SetOther(std::weak_ptr<A> _other) { other = _other; }
		void AccessOther()
		{
			std::shared_ptr<A> shared_other = other.lock(); //lock함수는 weak_ptr이 가리키는 객체가 아직 존재한다면 해당객체를 가리키는 shared_ptr을 반환해공유한다!
			if (shared_other)
			{
				std::cout << "객체에 접근됨" << '\n';
			}
			else
			{
				std::cout << "이미 소멸된 객체!" << '\n';
			}
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
	void Exam2()
	{
		std::unique_ptr<A> pa(new A());
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
		std::unique_ptr<A> pa(new A());
		pa->TestFunc();

		std::unique_ptr<A> pb = std::move(pa); //move를 이용해 rvalue로 변환시킨객체를 이동생성자를 이용해 소유권을 이전시킴은 가능하다, 이후pa는 nullptr이되므로 참조시 댕글링포인터현상을발생한다
		A* tempptr = pb.get();
		tempptr->TestFunc();

		std::unique_ptr<A> pc = std::make_unique<A>(); //perfectForward로 &&를 이용해 완벽한전달을 이용, 생성자의 매개변수를 그대로 전달한다

		std::vector<std::unique_ptr<A>> vec;
		vec.push_back(std::make_unique<A>()); //push_back최적화, 기본적인경우는 emplace_back이용(push_back은 기본적으로 완벽한전달을 통하지않고 복사를이용해 객체를 집어넣는다)
	}

	/*
	* 때에 따라서 여러개의 스마트포인터가 하나의 객체를 같이 소유해야하는 경우가 있다, 후에 자원을 사용하기위해 객체의소멸등 어느타이밍에 
	* 해제해야할지 추적하면서 그 수가 0이되어야 비로소 해제를 시켜주는 방식의 포인터가 필요하다, 이를위해 shared_ptr을 사용한다
	* shared_ptr은 기존에 유일하게 객체를 소유하는 unique_ptr과 다르게 특정한 객체를 가리킬때 다른 shared_ptr또한 해당객체를 가리킬 수 있다
	* 앞서 언급된대로 몇개의 shared_ptr이 해당 객체를 가리키는지 알 수 있어야하는데 이를 참조개수(Reference_count)로 관리한다, 0이되면 해당 객체를 해제하는것
	*/

	void Exam4()
	{
		std::vector<std::shared_ptr<A>> vec;
		vec.push_back(std::shared_ptr<A>(new A()));
		vec.push_back(std::shared_ptr<A>(vec[0]));
		vec.push_back(std::shared_ptr<A>(vec[1]));

		std::cout << vec.begin()->use_count() << ' '; //refcount
		std::cout << "원소0소멸" << '\n';
		vec.erase(vec.begin());
		std::cout << vec.begin()->use_count() << ' ';
		std::cout << "원소1소멸" << '\n';
		vec.erase(vec.begin());
		std::cout << vec.begin()->use_count() << ' ';
		std::cout << "원소2소멸" << '\n';
		vec.erase(vec.begin());
		std::cout << "main종료" << '\n';
	}
	/*
	* shared_ptr은 처음으로 실제객체를 가리키게된 shared_ptr(원소0)이 제어블록(ControlBlock)을 동적할당 한 후 나머지들이 해당 제어블록에 필요한 정보를
	* 공유하는 방식으로 구현된다
	* 위의 예제의 제어블록할당/가리키는객체할당 을 한번에 합친크기로 하기위해 make_shared<T>(객체) 를 지정해주는 형태로 할당함이 훨씬 빠르다
	* shared_ptr은 인자로 shared_ptr이 아닌 원본객체의 주소가 전달되면 자기 자신이 첫 shared_ptr로 별도의 shared_ptr형태를 구성하기때문에 이에대한주의가필요하다
	* 별도의 두개의 제어블록이 생성되는것, 즉 이중해제등의 문제가 발생할수도있다, 이러한 문제를 방지하기위해 원본객체의 주소를 이용하는 shared_ptr의생성을 지양해야함
	* 별도의 예외로 자기자신을 지정하는 shared_ptr을 반환할경우 std::shared_from_this()를 통하면 자기자신을 가리키는 shared_ptr을 반환 가능하다, 별도의 shared_ptr을 생성x
	* 이를 위해선 해당 클래스가 std::enable_shared_from_this<클래스>를 상속받아야한다, 이를위해선 해당객체를 가리키는 shared_ptr이 반드시 먼저 생성되어있어야한다,
	* shared_from_this()함수는 위의클래스를 상속해 만든 객체에서 해당객체를 가리키는 제어블록의여부만을 확인 할 뿐, 없는제어블록(shared_ptr)을 만들지 않는다
	*/

	/*순환참조
	* 만약 어떠한 두 객체가 있는데 객체의 멤버변수가 shared_ptr로 다른객체를 서로 가리키고있다면 두객체는 영원히 소멸 될수 없는 상태가 되고 서로 참조하여 refcount가 0이 되지않는
	* 경우를 순환참조라고한다, 이러한 문제를 해결하기위해 weak_ptr을 이용한다
	* weak_ptr은 일반포인터, shared_ptr사이에 위치한 스마트포인터로 객체를 참조 하나 참조카운팅을 늘리지않는다
	* 그말인즉슨 weak_ptr이 참조하고있는 객체가 있다고하더라도 다른 shared_ptr이 가리키고 있지 않다면 이미 메모리에서 소멸되었음을 의미한다
	* 이를 위해 weak_ptr은 자체로는 객체자체를 가리키는것이 불가하며 shared_ptr로 변환되어 사용된다, 이때 가리키는 객체가 이미 소멸되어있다면 빈 shared_ptr로 변환되고
	* 아닐경우 올바른 shared_ptr로써 사용된다
	*/
	void Exam5()
	{
		std::vector<std::shared_ptr<A>> vec;
		vec.push_back(std::make_shared<A>());
		vec.push_back(std::make_shared<A>());
		vec[0]->SetOther(vec[1]);
		vec[1]->SetOther(vec[0]);

		std::cout << vec[0].use_count() << ' ' << vec[1].use_count() << '\n'; //refcount!
		vec[0]->AccessOther();
		vec.pop_back(); //1번원소를 제거!
		vec[0]->AccessOther();
		//weak_ptr자체도 shared_ptr객체의 제어블록에서 약한참조개수(weak count)로 관리가되고 해당 개수마저 0될시 그때 메모리에서 제어블록마저 해제가된다!
	}
}