#pragma once
#include "std.h"
/*
* c++ stl의 컨테이너는 두가지 종류가있다, 원소를 순차적으로 저장하는 시퀀스컨테이너(SequenceContainer), key에따라 저장하는 연관컨테이너(AssociateContainer) 
* 시퀀스컨테이너등 원소를 맨 뒤에 추가하는 작업은 amortized O(1) 이라 한다, 원소가 다 차게되는 경우 더 큰 공간을 재할당하고 기존원소의 복사를 수행해야하므로 실제O(1)보다는 크지만 평균적인값인것
*/

namespace stl_vec
{
	
	void Exam()
	{
		void (*pf)(const std::vector<int>&) = [](const std::vector<int>& vec) 
			{
			for (std::vector<int>::const_iterator iter = vec.begin(); iter != vec.end(); iter++)
				std::cout << *iter << ' ';
			std::cout << '\n'; 
			};
		std::vector<int> vec;
		vec.push_back(0);
		vec.push_back(10); //뒤에삽입
		vec.push_back(20);
		vec.push_back(30);
		//반복자(iterator)
		/*
		* begin() 첫원소
		* end() 마지막원소+1
		* 
		*/
		
		std::cout << *(vec.begin() + 2) << '\n';//0, 1, 2(vec[2] ->20)
		
		pf(vec);

		vec.insert(vec.begin() + 2, 15); //20자리에 15삽입
		pf(vec);
		
		*(vec.begin() + 3) = 25; //기존20을 25로변경

		//iterator을 이용해 erase를 수행할때 반복문내에서 iterator를 위치지정을 이용해 수행시 반환값을 iterator에 삽입하여 iterator를 재지정해줘야함
		for (std::vector<int>::const_iterator iter = vec.begin(); iter != vec.end();)
		{
			if (*iter == 25)
				iter = vec.erase(iter);
			iter++;
		}
		pf(vec);
		//begin()0을가리킴 end()끝+1을가리킴
		//rend()첫번째원소보다 한칸앞을가리킴(end와동일), rbegin() 마지막원소를가리킴
	}
}

namespace stl_list
{
	/*
	* 연결리스트, vector와 달리 연결리스트구조로 되어있기에 원소의 임의접근은 불가하다, 
	* ++, -- 같은 증감, 감산 연산만 가능하고 begin() + 5등 임의접근이 불가하다(연속적메모리가 아니기때문)
	*/
	void Exam()
	{
		std::list<int> lst;
		lst.push_back(0);
		lst.push_back(10); //뒤에삽입
		lst.push_back(20);
		lst.push_back(30);
		for (std::list<int>::const_iterator iter = lst.begin(); iter != lst.end();)
		{
			if (*iter == 20)
			{
				iter = lst.erase(iter);
				continue;
			}
			std::cout << *iter++ << ' ';
		}
	}
}

namespace stl_deque
{
	/*
	* 덱은 벡터의 대부분의 연산을 수행하며 속도또한 빠르게 가능하나 원소의 위치를 기록하기위한 추가메모리공간을 잡아먹기때문에 메모리공간을 소비함으로 시간복잡도측면의 이득을 가져간다
	* 메모리상에 연속으로 존재하지않고 연결리스트를 이용하여 다음위치를 가리키는 포인터로 구성하여 노드들을 구성시킨다
	* 노드형태로 원소집합을 구성시키기때문에 벡터처럼 새공간을 할당하면서 기존의 원소들을 복사할 필요성이 없기때문에 벡터보다 빠르게 동작 할 수있다
	*/
	void Exam()
	{
		
	}
}

/*
* 10_2
* 연관컨테이너(AssociateContainer)에 대해서 다룬다
* key<->value 쌍의 컨테이너
* 데이터가 있을시 여부만체크 set, unordered_set
* 데이터쌍을 체크 map, unordered_map
* set, map은 레드블랙트리기반의 자가균형이진트리를 이용
* unordered계열은 해시를 이용
* multi계열은 중복되는 key값을 허용한다, 리턴은 임의의값을 뱉으나 해당 키에 대응하는
* 구간을 뱉게끔 할 수 있다 이를 위해 var.equal_range(원소)를 사용한다
* set, map의 경우 < 혹은 > 연산자 비교를 하는 operator를 재정의하여 임의정의클래스를
* 삽입 가능하다, unordered계열은 hash를 만들어서사용해야 하므로 비효율적
* 
*/

/*
* 10_3
* sort는 랜덤반복지정자에서만 사용가능하다, vector, deque(원소임의접근이가능한), list같은 순차적으로 접근해야하는 컨테이너에서는 사용이불가능
* 일반적으로 algorithm의 함수들은 Pred즉 서술자에 함수객체를 집어넣어 사용한다(리턴 bool)
* erase의경우 erase(iter)의경우 하나의원소를 지우고 erase(iter first, iter last)를 통해 first~last사이의원소를지운다
* 특정한 조건을 만족시키는 조건을 사용할경우 remove, 혹은 remove_if를 이용한다
* 
* 람다함수
* [캡처절] (인자) -> 리턴타입 {함수본체}
* 리턴타입이없으면 ->생략가능 [캡쳐절] (인자) {함수본체}, 리턴타입을 컴파일러가추론, 클래스내부에서 인자를 체크위해 사용시 캡쳐절에 this를사용
* [&]참조로가져옴
* [=]복사로가져옴
* [this] 객체가져옴
* 
* transform(시작반복자, 끝반복자, 결과를저장할컨테이너의시작반복자, 서술자(Pred))-> 시작~끝 까지 시작반복자쪽에 해당갯수만큼복사, 즉 복사될컨테이너가 이를 받을만큼크기가되야함
*/
namespace ch10_3
{
	struct is_odd
	{
		bool operator()(const int& i) { return i % 2 == 1; };
	};
	struct is_odd2
	{
		int* num_delete;
		is_odd2(int* num_del_ptr) : num_delete(num_del_ptr) {} //외부변수를 사용, 객체함수의 인스턴스변수는 객체마다다르므로,
		bool operator()(const int& i) 
		{ 
			if (*num_delete < 2)
			{
				if (i % 2 == 1)
				{
					(*num_delete)++;
					return true;
				}
			}
			return false;
		};
	};
	void Exam()
	{
		int del_cnt = 0;
		std::vector<int> vec = { 5, 3, 1, 2, 3, 4 };
		/*
		auto iterator = std::remove_if(vec.begin(), vec.end(), is_odd2(&del_cnt)); // 2, 4, 1, 2, 3, 4, is_odd에 해당하는 홀수들을 뒤로보낸다
		for (const auto& iter : vec)
			std::cout << iter << ' ';
		std::cout << '\n';
		*/
		vec.erase(std::remove_if(vec.begin(), vec.end(), is_odd2(&del_cnt)), vec.end());
		for (const auto& iter : vec)
			std::cout << iter << ' ';
	}
}