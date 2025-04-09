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

	void workFunc(int& counter)
	{
		for (int i = 0; i < 10'000; i++)
			counter += 1;
	}
	void Exam3()
	{
		//경쟁상태(RaceCondition)
		int counter = 0;
		std::vector<std::thread> workers;
		//thread객체에 counter를 전달시 std::ref로 전달해주지 않으면 thread는 counter변수를 복사하려한다(call by value), 이때 workFunc는
		//int&, 즉 참조되는 변수를 요구하기때문에 형이 맞지 않아 std::ref로 참조로 전달을 해줘야한다
		for (int i = 0; i < 4; i++) workers.push_back(std::thread(workFunc, std::ref(counter)));
		for (int i = 0; i < 4; i++) workers[i].join();
		//이때 counter는 쓰레드끼리 변수상태를 공유하기때문에 경쟁상태(RaceCondition)가 되어 의도하는 올바른 값을 기대 할 수 없다
		std::cout << "counter의 최종값 : " << counter << '\n';
	}
	//경쟁상태에서의 값 증가에서 기대값과 다른 연산결과를 보이는 이유는 어떠한 식으로 counter+=1이 컴파일 되는지 알 필요성이 있다
	/*
	mov rax, qword ptr [rbp - 8]
	mov ecx, dword ptr [rax]
	add ecx, 1
	mov dword ptr [rax], ecx
	위와같은 형태로 counter+=1이라는 코드가 어셈블리언어 형태로 컴파일 되었다고 가정한다
	이때 첫줄 rax레지스터에 rbp-8이라는 주소값을 []역참조한다, 이때 해당주소값은 qword이므로 8바이트를 의미한다, 즉 이를 c언어로 풀이하면
	rax = *(int**)(rbp-8)이 된다
	이는 workFunc는 기본적으로 변수를 참조형태(int&)로 받는데 thread클래스를 통해서 전달을 해 줘야 하기 때문에 counter를 그대로 넘기면
	call by value의 형태로 넘어가 원래주소를 전달해 줄 수 없어 형태가 맞지않는다, 고로 std::ref(counter)를 통해 &counter, 즉 주소를 넘기게되므로
	8바이트형 정수값(주소값)을 넘겨줘야하는데 해당 주소가 rbp-8에 있다고 가정한다, 넘겨줄때 어떠한 타입을 넘겨줄 지 알수 없기 때문에 void* 타입으로
	기본형으로 넘겨주는데 실제 필요한 주소값은 int* 즉 8바이트인트형 주소값이므로 이를 포함하는 bsp-8은 int*을 내포해야하기때문에 int**로 캐스팅 할 수
	있음이 자명하기에 (int**)으로 캐스팅해주는것, 이후에 저장되어있는 8바이트주소값을 역참조(*)하면 8바이트주소값을 rax에 대입해 줄 수 있는것이다
	
	이후 ecx는 int형 변수가되며이를 역참조해주는형태로 다시 받아오기때문에
	ecx = *(int*)(rax), 즉 eax는 counter가 되는결과를 보인다, 이후 add ecx, 1로 counter를 1 증감시킨다

	마지막으로 move dword ptr [rax], ecx 즉 ecx의 값을 rax(&counter)쪽으로 다시 옮겨준다, 증감된값을 다시 반대로옮겨주는것이므로 변경된결과를 
	rax저장된 주소에 복사한다

	이때 쓰레드타이밍(경쟁상태)로 인한 문제가 생긴다, 쓰레드1, 2에서 각각 어떠한 타이밍에 해당 어셈블리 코드가 실행되고 복사되는 형태가 순서대로 이뤄지지
	않기때문에 증감된값(ecx)와 이를 다시 원래 rax(&counter)로 복사해주는과정에 덮어씌우기 형태로 값의 중복적인 간섭이 생겨 올바른 결과를 보장 할 수 없는것
	*/
	
	/*
	위와같은 경쟁상태 문제를 해결하기위해 하나의 쓰레드만 해당 counter += 1에 순차적으로 접근 가능하다면 방지 할 수 있지 않는가 ? 이를 위해
	mutex를 사용한다
	*/
	void workFuncMutex(int& counter, std::mutex& m)
	{
		for (int i = 0; i < 10'000; i++)
		{
			/*
			m.lock(); //한번에 한 쓰레드만 뮤텍스의 사용권한을 갖는다
			counter += 1; //lock~unlock사이의 코드부분을 임계영역(CriticalSection)이라 부른다
			m.unlock(); //unlock을 하지 않으면 lock한 쓰레드도 다시 lock을 호출하게되어 아무쓰레드도 연산을 끝마치거나 시작하지 못한다, Deadlock(교착상태)
			*/
			std::lock_guard<std::mutex> lock(m); //unique_ptr처럼 intent를 빠져나가면 자동으로 해제를해준다
			counter += 1;
		}
			
	}
	void Exam4()
	{
		//경쟁상태(RaceCondition)
		int counter = 0;
		std::mutex m; //뮤텍스변수
		std::vector<std::thread> workers;

		for (int i = 0; i < 4; i++) workers.push_back(std::thread(workFuncMutex, std::ref(counter), std::ref(m)));
		for (int i = 0; i < 4; i++) workers[i].join();
		//이때 counter는 쓰레드끼리 변수상태를 공유하기때문에 경쟁상태(RaceCondition)가 되어 의도하는 올바른 값을 기대 할 수 없다
		std::cout << "counter의 최종값 : " << counter << '\n';
	}

	//교착상태는 데드락에의해 이도저도 못하는 상황을 의미한다, 두개의 뮤텍스가 있고 실행함수각각에서 두개의 뮤텍스를 서로 lock하는 상황일때
	//양쪽에서 각각의뮤텍스를 서로 lock하려고 하면 해당뮤텍스는 unlock이 되어있어야 하지만 해당경우에는 서로 lock을하고있어 교착상태에 빠질 수 있다.
	//이러한 상태를 기아상태(starvation)이라 한다
	//해결법중 하나로는 try_lock같은 함수를 이용해 함수 한쪽에 우선권을 주어 우선적으로 뮤텍스를 이용하게 해주어 스레드를 선수행시키고
	//이후에 수행하는방법또한 존재한다, 이러한 여러가지방법등을 설명한 C++ Concurrency In Action이라는 서적에서 데드락을 피하기위해 다음과같은 가이드라인을 제시한다
	/*
	* -중첩된 Lock의 이용을 피하라
	* 모든 쓰레드들이 최대 한개의 Lock을 이용한다면 데드락을 피할 수 있다, 일반적인 구조디자인으로는 한개의 Lock(Mutex)만으로 충분하기 때문에 여러개의 mutex가필요한가를 고려가필요
	* 
	* -Lock을 소유하고 있을때 유저코드를 호출하는것을 피하라
	* lock의 인텐트, 즉 lock이 해제되기전에 다른함수호출이나 콜백등을 호출하는경우 다른 구조에서 lock을 획득하려는 시도가 있을수 있기 때문에 이를피해야한다
	* 
	* -Lock들을 언제나 정해진 순서로 획득해라
	* Mutex들을 정해진순서대로 lock해야 다른함수등에서 Mutex를 사용하더라도 같은순서로 Lock해야 순서로인한 데드락의 발생을 막을 수 있다
	* 위의 사항들을 고려해 멀티쓰레드 프로그램에서 자주 사용되는 생산자-소비자(Producer,Consumer)디자인 패턴에 대해 알아본다
	*/
	
	void Producer(std::queue<std::string>* downloaded_pages, std::mutex* m, int index)
	{
		for (int i = 0; i < 5; i++)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100 * index)); //다운로드시 걸리는 시간이라 가정, 쓰레드 번호가 클수록 시간이 오래걸린다
			std::string content = "웹사이트 : " + std::to_string(i) + " from thread(" + std::to_string(index) + ")\n";

			m->lock();
			//임계영역, 데이터는 쓰레드사이에서 공유되야 하므로 임계영역내에서 문제가 없게끔 처리한다
			downloaded_pages->push(content);
			m->unlock();
		}
	}

	void Consumer(std::queue<std::string>* downloaded_pages, std::mutex* m, int* num_processed)
	{
		//다운로드갯수가 25개가 안됐다면
		while (*num_processed < 25)
		{
			m->lock();
			if (downloaded_pages->empty())
			{
				//만약 다운로드완료된게 없다면 바로 lock을 해제하고 10밀리세컨드동안 대기후 재체크(busy-waiting)
				m->unlock();
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
				continue;
			}
			std::string content = downloaded_pages->front();
			downloaded_pages->pop();
			(*num_processed)++;
			m->unlock();

			std::cout << content;
			std::this_thread::sleep_for(std::chrono::milliseconds(80));
		}
	}

	void Exam5()
	{
		std::queue<std::string> downloaded_pages;
		std::mutex m;

		std::vector<std::thread> producers;
		for (int i = 0; i < 5; i++) producers.push_back(std::thread(Producer, &downloaded_pages, &m, i + 1));

		int num_complete = 0;
		std::vector<std::thread> consumers;
		for (int i = 0; i < 3; i++) consumers.push_back(std::thread(Consumer, &downloaded_pages, &m, &num_complete));

		for (int i = 0; i < 5; i++) producers[i].join();
		for (int i = 0; i < 3; i++) consumers[i].join();
	}

	//위에 언급된 기본적인 반복외에 특정한 조건을 주어 알림(notify)을 통해 mutex를 사용하기위해 조건변수(condition_value)를 사용한다
	//std::condition_variable에서 특정 뮤텍스를 인자로 삽입해 wait함수에서 notify함수등으로 호출시 조건을 충족시키면 인자로넘겨받은 mutex를 블로킹상태에서 해제시킨다.
	void ProducerUsingCV(std::queue<std::string>* downloaded_pages, std::mutex* m, int index, std::condition_variable* cv)
	{
		for (int i = 0; i < 5; i++)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100 * index)); //다운로드시 걸리는 시간이라 가정, 쓰레드 번호가 클수록 시간이 오래걸린다
			std::string content = "웹사이트 : " + std::to_string(i) + " from thread(" + std::to_string(index) + ")\n";

			m->lock();
			//임계영역, 데이터는 쓰레드사이에서 공유되야 하므로 임계영역내에서 문제가 없게끔 처리한다
			downloaded_pages->push(content);
			m->unlock();
			//Consumer에서 Wait중인 cv에 준비되었음을 알린다
			cv->notify_one();
		}
	}
	void ConsumerUsingCV(std::queue<std::string>* downloaded_pages, std::mutex* m, int* num_processed, std::condition_variable* cv)
	{
		//다운로드갯수가 25개가 안됐다면
		while (*num_processed < 25)
		{
			std::unique_lock<std::mutex> lk(*m);
			//조건이 참이 아닐시 블로킹상태로대기한다
			cv->wait(lk, [&]() {return !downloaded_pages->empty() || *num_processed >= 25; });
			if (*num_processed >= 25)
			{
				lk.unlock();
				return;
			}

			std::string content = downloaded_pages->front();
			downloaded_pages->pop();
			(*num_processed)++;
			m->unlock();

			std::cout << content;
			std::this_thread::sleep_for(std::chrono::milliseconds(80));
		}
	}

	void Exam6()
	{
		std::queue<std::string> downloaded_pages;
		std::mutex m;
		std::condition_variable cv;

		std::vector<std::thread> producers;
		for (int i = 0; i < 5; i++) producers.push_back(std::thread(ProducerUsingCV, &downloaded_pages, &m, i + 1, &cv));

		int num_complete = 0;
		std::vector<std::thread> consumers;
		for (int i = 0; i < 3; i++) consumers.push_back(std::thread(ConsumerUsingCV, &downloaded_pages, &m, &num_complete, &cv));

		for (int i = 0; i < 5; i++) producers[i].join();
		//producer들이 전부 리턴되면 조건변수를 전부 깨우고 consumer들이 전부 리턴될때까지 블로킹상태로기다린다
		cv.notify_all();
		for (int i = 0; i < 3; i++) consumers[i].join();
	}
}
