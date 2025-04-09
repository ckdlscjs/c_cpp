#pragma once
#include "std.h"
/*
* 프로세스는  1개의 프로그램을 가리킬때 보통 1개의 프로세스를 의미한다
* cpu가 일반적으로 한개의 연산을 처리할때 여러가지 프로그램을 실행 가능한 이유는
* 컨텍스트스위칭(ContextSwitching)에 있다, 프로그램들을 스위칭하며 실행하는것, 어떠한 프로그램을 실행할지에 대한 여부는
* 운영체제의 스케쥴러(Scheduler)가 결정한다
* 
* cpu코어에서 프로그램이 돌아가는 단위를 쓰레드(Thread)라 부른다, 즉 cpu의 코어 하나에는 한번에 한개의 쓰레드의 명령을 실행시킨다
* 프로세스는 최소 한개의 쓰레드로 구성되어있으며 여러개의 쓰레드로 구성된 프로그램을 멀티쓰레드 프로그램이라 부른다
* 프로세스는 서로 메모리공간을 공유하지 않으나 쓰레드끼리는 서로 메모리를 공유한다, 같은 프로세스 안에 존재하기 때문에 같은 변수값에 접근이 가능해지는것
* 
* 현대의 cpu가 여러개의 코어를 지원함으로써 여러개의 쓰레드를 동시에 수행할 수 있고 이때 어느측면에서 프로그램을 멀티쓰레드로 구성하는게 유익한가? 를 고려해봐야한다
* 1)병렬(Parallelizable)가능한 작업들
* 1~10000까지 더하는 연산을 수행할때 한번의 덧셈당 1초가 걸린다고 가정했을때 1~1000, 1001~2000... 식으로 병렬단위로 10개의쓰레드를 이용해 구성한다면
* 최종합 10개를 10번 더하는 연산까지 합쳐도 1010초에 모든 연산을 수행할 수 있기 때문에 단일쓰레드로 10000초가 걸리는 일을 대략 10배 가까이 시간을 단축한결과가된다
* 이처럼 다른 쓰레드를 이용해 좀더 빠르게 수행하는 것을 병렬화(Parallelize)라 한다, 하지만 모든 작업들이 병렬화가 가능하진 않다
* 예를들어 피보나치수열F(n) = F(n-1) + F(n-2)의 연산을 병렬화 시켰을때 다음항의계산에는 이전항의 결과가 필수적으로 들어가게 되는데
* 병렬화하여 동시수행한다면 이전값의 결과가 이미 계산되었을지 아니면 계산이 안되어있을지 알 수 없기 때문에 이러한 작업을 순서대로 진행하게된다면 병렬화의의미가 없어진다
* 즉 이와같이 어떠한 연산A를 수행하기위해 연산B가 선행되어야 한다면 A가 B에 의존적(dependent)하다고 볼 수 있다
* 
* 2)대기시간이 긴 작업들
* 일반적으로 인터넷의 전송속도는 cpu의 처리속도에 비해 매우 느리다, 무언가 인터넷에서 ping을 통해 요청을 할때 cpu코어 단일적으로 요청을 한다면 ping의 응답시간에비해
* cpu가 일을 처리하지 않고 대기하고 있을 시간이 비게되어 비효율적이다, 이때 쓰레드를 이용하여 컨텍스트스위칭을 통해 하나의 코어내에서 여러쓰레드를 동시에 처리해
* 여러 응답처리를 수행하게되면 cpu수행시간을 낭비하지 않고 효율적으로 작업을 처리 할 수 있게된다
*/
namespace ch15
{ 
	//#include <thread>로 쓰레드 헤더파일을 추가해야함
	void func1()
	{
		for (int i = 0; i < 10; i++)
		{
			std::cout << "쓰레드1작동중!\n";
		}
	}
	void func2()
	{
		for (int i = 0; i < 10; i++)
		{
			std::cout << "쓰레드2작동중!\n";
		}
	}
	void func3()
	{
		for (int i = 0; i < 10; i++)
		{
			std::cout << "쓰레드3작동중!\n";
		}
	}

	void Exam1()
	{
		std::thread t1(func1); //쓰레드 객체를 생성, 인자로 전달받은 함수를 새로운 쓰레드에서 수행하게된다, 쓰레드객체 생성시 이미 함수가 호출됨을 보인다
		std::thread t2(func2);
		std::thread t3(func3);

		t1.join(); //스레드의 종료를대기
		t2.join();
		t3.join();
		//세개의 join이 전부 리턴되야 Exam을 잡고있는 스레드의 이후문장이 수행된다
		//join을 수행하지 않을경우 비정상종료(terminate)확률이 높다
		//스레드의 각자수행만이 필요하다면 종료를 기다리지않는 detach를 사용한다!
		//t1.detach(); //해당스레드의 종료를 기다리지않고 알아서 끝나게한다
		std::cout << "join이 전부 끝남! 병렬수행이끝나서 스레드블로킹이풀림";
	}

	//1~10'000까지의 합을 여러 쓰레드를 이용해 빠르게 계산하는예제
	void SumWorker(std::vector<int>::iterator start, std::vector<int>::iterator end, int* result)
	{
		for (auto iter = start; iter < end; iter++)
			*result += *iter;
		
		std::thread::id this_id = std::this_thread::get_id();
		//cout은 각각 별도의 함수처럼<<를 기준으로 별도의 함수처럼 호출되기때문에 스레드안정성을 보장 할 수 없다 고로 하나의함수호출인 printf를 이용한다
		//std::cout << "쓰레드" << this_id << "가 " << *start << " ~ " << *end - 1 << " 까지 더한 결과 : " << *result << '\n'; 
		printf("쓰레드 %x 에서 %d 부터 %d 까지 계산한 결과 : %d \n", this_id, *start, *(end - 1), *result);
	}
	void Exam2()
	{
		std::vector<int> nums(10000);
		for (int i = 0; i < 10000; i++) nums[i] = i;

		int results[4] = { 0, 0, 0, 0 };
		std::vector<std::thread> workers;
		for (int i = 0; i < 4; i++) workers.push_back(std::thread(SumWorker, nums.begin() + i * 2500, nums.begin() + (i + 1) * 2500, &results[i]));
		for (int i = 0; i < 4; i++) workers[i].join();
		int total = 0;
		for (int i = 0; i < 4; i++) total += results[i];
		std::cout << "전체합 : " << total << '\n';
	}
}
