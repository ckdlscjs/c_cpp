#pragma once
#include "std.h"
/*
* ���μ�����  1���� ���α׷��� ����ų�� ���� 1���� ���μ����� �ǹ��Ѵ�
* cpu�� �Ϲ������� �Ѱ��� ������ ó���Ҷ� �������� ���α׷��� ���� ������ ������
* ���ؽ�Ʈ����Ī(ContextSwitching)�� �ִ�, ���α׷����� ����Ī�ϸ� �����ϴ°�, ��� ���α׷��� ���������� ���� ���δ�
* �ü���� �����췯(Scheduler)�� �����Ѵ�
* 
* cpu�ھ�� ���α׷��� ���ư��� ������ ������(Thread)�� �θ���, �� cpu�� �ھ� �ϳ����� �ѹ��� �Ѱ��� �������� ����� �����Ų��
* ���μ����� �ּ� �Ѱ��� ������� �����Ǿ������� �������� ������� ������ ���α׷��� ��Ƽ������ ���α׷��̶� �θ���
* ���μ����� ���� �޸𸮰����� �������� ������ �����峢���� ���� �޸𸮸� �����Ѵ�, ���� ���μ��� �ȿ� �����ϱ� ������ ���� �������� ������ ���������°�
* 
* ������ cpu�� �������� �ھ ���������ν� �������� �����带 ���ÿ� ������ �� �ְ� �̶� ������鿡�� ���α׷��� ��Ƽ������� �����ϴ°� �����Ѱ�? �� ����غ����Ѵ�
* 1)����(Parallelizable)������ �۾���
* 1~10000���� ���ϴ� ������ �����Ҷ� �ѹ��� ������ 1�ʰ� �ɸ��ٰ� ���������� 1~1000, 1001~2000... ������ ���Ĵ����� 10���Ǿ����带 �̿��� �����Ѵٸ�
* ������ 10���� 10�� ���ϴ� ������� ���ĵ� 1010�ʿ� ��� ������ ������ �� �ֱ� ������ ���Ͼ������ 10000�ʰ� �ɸ��� ���� �뷫 10�� ������ �ð��� �����Ѱ�����ȴ�
* ��ó�� �ٸ� �����带 �̿��� ���� ������ �����ϴ� ���� ����ȭ(Parallelize)�� �Ѵ�, ������ ��� �۾����� ����ȭ�� �������� �ʴ�
* ������� �Ǻ���ġ����F(n) = F(n-1) + F(n-2)�� ������ ����ȭ �������� �������ǰ�꿡�� �������� ����� �ʼ������� ���� �Ǵµ�
* ����ȭ�Ͽ� ���ü����Ѵٸ� �������� ����� �̹� ���Ǿ����� �ƴϸ� ����� �ȵǾ������� �� �� ���� ������ �̷��� �۾��� ������� �����ϰԵȴٸ� ����ȭ���ǹ̰� ��������
* �� �̿Ͱ��� ��� ����A�� �����ϱ����� ����B�� ����Ǿ�� �Ѵٸ� A�� B�� ������(dependent)�ϴٰ� �� �� �ִ�
* 
* 2)���ð��� �� �۾���
* �Ϲ������� ���ͳ��� ���ۼӵ��� cpu�� ó���ӵ��� ���� �ſ� ������, ���� ���ͳݿ��� ping�� ���� ��û�� �Ҷ� cpu�ھ� ���������� ��û�� �Ѵٸ� ping�� ����ð�������
* cpu�� ���� ó������ �ʰ� ����ϰ� ���� �ð��� ��ԵǾ� ��ȿ�����̴�, �̶� �����带 �̿��Ͽ� ���ؽ�Ʈ����Ī�� ���� �ϳ��� �ھ���� ���������带 ���ÿ� ó����
* ���� ����ó���� �����ϰԵǸ� cpu����ð��� �������� �ʰ� ȿ�������� �۾��� ó�� �� �� �ְԵȴ�
*/
namespace ch15
{ 
	//#include <thread>�� ������ ��������� �߰��ؾ���
	void func1()
	{
		for (int i = 0; i < 10; i++)
		{
			std::cout << "������1�۵���!\n";
		}
	}
	void func2()
	{
		for (int i = 0; i < 10; i++)
		{
			std::cout << "������2�۵���!\n";
		}
	}
	void func3()
	{
		for (int i = 0; i < 10; i++)
		{
			std::cout << "������3�۵���!\n";
		}
	}

	void Exam1()
	{
		std::thread t1(func1); //������ ��ü�� ����, ���ڷ� ���޹��� �Լ��� ���ο� �����忡�� �����ϰԵȴ�, �����尴ü ������ �̹� �Լ��� ȣ����� ���δ�
		std::thread t2(func2);
		std::thread t3(func3);

		t1.join(); //�������� ���Ḧ���
		t2.join();
		t3.join();
		//������ join�� ���� ���ϵǾ� Exam�� ����ִ� �������� ���Ĺ����� ����ȴ�
		//join�� �������� ������� ����������(terminate)Ȯ���� ����
		//�������� ���ڼ��ุ�� �ʿ��ϴٸ� ���Ḧ ��ٸ����ʴ� detach�� ����Ѵ�!
		//t1.detach(); //�ش罺������ ���Ḧ ��ٸ����ʰ� �˾Ƽ� �������Ѵ�
		std::cout << "join�� ���� ����! ���ļ����̳����� ��������ŷ��Ǯ��";
	}

	//1~10'000������ ���� ���� �����带 �̿��� ������ ����ϴ¿���
	void SumWorker(std::vector<int>::iterator start, std::vector<int>::iterator end, int* result)
	{
		for (auto iter = start; iter < end; iter++)
			*result += *iter;
		
		std::thread::id this_id = std::this_thread::get_id();
		//cout�� ���� ������ �Լ�ó��<<�� �������� ������ �Լ�ó�� ȣ��Ǳ⶧���� ������������� ���� �� �� ���� ��� �ϳ����Լ�ȣ���� printf�� �̿��Ѵ�
		//std::cout << "������" << this_id << "�� " << *start << " ~ " << *end - 1 << " ���� ���� ��� : " << *result << '\n'; 
		printf("������ %x ���� %d ���� %d ���� ����� ��� : %d \n", this_id, *start, *(end - 1), *result);
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
		std::cout << "��ü�� : " << total << '\n';
	}

	void workFunc(int& counter)
	{
		for (int i = 0; i < 10'000; i++)
			counter += 1;
	}
	void Exam3()
	{
		//�������(RaceCondition)
		int counter = 0;
		std::vector<std::thread> workers;
		//thread��ü�� counter�� ���޽� std::ref�� ���������� ������ thread�� counter������ �����Ϸ��Ѵ�(call by value), �̶� workFunc��
		//int&, �� �����Ǵ� ������ �䱸�ϱ⶧���� ���� ���� �ʾ� std::ref�� ������ ������ ������Ѵ�
		for (int i = 0; i < 4; i++) workers.push_back(std::thread(workFunc, std::ref(counter)));
		for (int i = 0; i < 4; i++) workers[i].join();
		//�̶� counter�� �����峢�� �������¸� �����ϱ⶧���� �������(RaceCondition)�� �Ǿ� �ǵ��ϴ� �ùٸ� ���� ��� �� �� ����
		std::cout << "counter�� ������ : " << counter << '\n';
	}
	//������¿����� �� �������� ��밪�� �ٸ� �������� ���̴� ������ ��� ������ counter+=1�� ������ �Ǵ��� �� �ʿ伺�� �ִ�
	/*
	mov rax, qword ptr [rbp - 8]
	mov ecx, dword ptr [rax]
	add ecx, 1
	mov dword ptr [rax], ecx
	���Ͱ��� ���·� counter+=1�̶�� �ڵ尡 �������� ���·� ������ �Ǿ��ٰ� �����Ѵ�
	�̶� ù�� rax�������Ϳ� rbp-8�̶�� �ּҰ��� []�������Ѵ�, �̶� �ش��ּҰ��� qword�̹Ƿ� 8����Ʈ�� �ǹ��Ѵ�, �� �̸� c���� Ǯ���ϸ�
	rax = *(int**)(rbp-8)�� �ȴ�
	�̴� workFunc�� �⺻������ ������ ��������(int&)�� �޴µ� threadŬ������ ���ؼ� ������ �� ��� �ϱ� ������ counter�� �״�� �ѱ��
	call by value�� ���·� �Ѿ �����ּҸ� ������ �� �� ���� ���°� �����ʴ´�, ��� std::ref(counter)�� ���� &counter, �� �ּҸ� �ѱ�ԵǹǷ�
	8����Ʈ�� ������(�ּҰ�)�� �Ѱ�����ϴµ� �ش� �ּҰ� rbp-8�� �ִٰ� �����Ѵ�, �Ѱ��ٶ� ��� Ÿ���� �Ѱ��� �� �˼� ���� ������ void* Ÿ������
	�⺻������ �Ѱ��ִµ� ���� �ʿ��� �ּҰ��� int* �� 8����Ʈ��Ʈ�� �ּҰ��̹Ƿ� �̸� �����ϴ� bsp-8�� int*�� �����ؾ��ϱ⶧���� int**�� ĳ���� �� ��
	������ �ڸ��ϱ⿡ (int**)���� ĳ�������ִ°�, ���Ŀ� ����Ǿ��ִ� 8����Ʈ�ּҰ��� ������(*)�ϸ� 8����Ʈ�ּҰ��� rax�� ������ �� �� �ִ°��̴�
	
	���� ecx�� int�� �������Ǹ��̸� ���������ִ����·� �ٽ� �޾ƿ��⶧����
	ecx = *(int*)(rax), �� eax�� counter�� �Ǵ°���� ���δ�, ���� add ecx, 1�� counter�� 1 ������Ų��

	���������� move dword ptr [rax], ecx �� ecx�� ���� rax(&counter)������ �ٽ� �Ű��ش�, �����Ȱ��� �ٽ� �ݴ�οŰ��ִ°��̹Ƿ� ����Ȱ���� 
	rax����� �ּҿ� �����Ѵ�

	�̶� ������Ÿ�̹�(�������)�� ���� ������ �����, ������1, 2���� ���� ��� Ÿ�ֿ̹� �ش� ����� �ڵ尡 ����ǰ� ����Ǵ� ���°� ������� �̷�����
	�ʱ⶧���� �����Ȱ�(ecx)�� �̸� �ٽ� ���� rax(&counter)�� �������ִ°����� ������ ���·� ���� �ߺ����� ������ ���� �ùٸ� ����� ���� �� �� ���°�
	*/
	
	/*
	���Ͱ��� ������� ������ �ذ��ϱ����� �ϳ��� �����常 �ش� counter += 1�� ���������� ���� �����ϴٸ� ���� �� �� ���� �ʴ°� ? �̸� ����
	mutex�� ����Ѵ�
	*/
	void workFuncMutex(int& counter, std::mutex& m)
	{
		for (int i = 0; i < 10'000; i++)
		{
			/*
			m.lock(); //�ѹ��� �� �����常 ���ؽ��� �������� ���´�
			counter += 1; //lock~unlock������ �ڵ�κ��� �Ӱ迵��(CriticalSection)�̶� �θ���
			m.unlock(); //unlock�� ���� ������ lock�� �����嵵 �ٽ� lock�� ȣ���ϰԵǾ� �ƹ������嵵 ������ ����ġ�ų� �������� ���Ѵ�, Deadlock(��������)
			*/
			std::lock_guard<std::mutex> lock(m); //unique_ptró�� intent�� ���������� �ڵ����� ���������ش�
			counter += 1;
		}
			
	}
	void Exam4()
	{
		//�������(RaceCondition)
		int counter = 0;
		std::mutex m; //���ؽ�����
		std::vector<std::thread> workers;

		for (int i = 0; i < 4; i++) workers.push_back(std::thread(workFuncMutex, std::ref(counter), std::ref(m)));
		for (int i = 0; i < 4; i++) workers[i].join();
		//�̶� counter�� �����峢�� �������¸� �����ϱ⶧���� �������(RaceCondition)�� �Ǿ� �ǵ��ϴ� �ùٸ� ���� ��� �� �� ����
		std::cout << "counter�� ������ : " << counter << '\n';
	}

	//�������´� ����������� �̵����� ���ϴ� ��Ȳ�� �ǹ��Ѵ�, �ΰ��� ���ؽ��� �ְ� �����Լ��������� �ΰ��� ���ؽ��� ���� lock�ϴ� ��Ȳ�϶�
	//���ʿ��� �����ǹ��ؽ��� ���� lock�Ϸ��� �ϸ� �ش���ؽ��� unlock�� �Ǿ��־�� ������ �ش��쿡�� ���� lock���ϰ��־� �������¿� ���� �� �ִ�.
	//�̷��� ���¸� ��ƻ���(starvation)�̶� �Ѵ�
	//�ذ���� �ϳ��δ� try_lock���� �Լ��� �̿��� �Լ� ���ʿ� �켱���� �־� �켱������ ���ؽ��� �̿��ϰ� ���־� �����带 �������Ű��
	//���Ŀ� �����ϴ¹������ �����Ѵ�, �̷��� ��������������� ������ C++ Concurrency In Action�̶�� �������� ������� ���ϱ����� ���������� ���̵������ �����Ѵ�
	/*
	* -��ø�� Lock�� �̿��� ���϶�
	* ��� ��������� �ִ� �Ѱ��� Lock�� �̿��Ѵٸ� ������� ���� �� �ִ�, �Ϲ����� �������������δ� �Ѱ��� Lock(Mutex)������ ����ϱ� ������ �������� mutex���ʿ��Ѱ��� ������ʿ�
	* 
	* -Lock�� �����ϰ� ������ �����ڵ带 ȣ���ϴ°��� ���϶�
	* lock�� ����Ʈ, �� lock�� �����Ǳ����� �ٸ��Լ�ȣ���̳� �ݹ���� ȣ���ϴ°�� �ٸ� �������� lock�� ȹ���Ϸ��� �õ��� ������ �ֱ� ������ �̸����ؾ��Ѵ�
	* 
	* -Lock���� ������ ������ ������ ȹ���ض�
	* Mutex���� ������������� lock�ؾ� �ٸ��Լ���� Mutex�� ����ϴ��� ���������� Lock�ؾ� ���������� ������� �߻��� ���� �� �ִ�
	* ���� ���׵��� ����� ��Ƽ������ ���α׷����� ���� ���Ǵ� ������-�Һ���(Producer,Consumer)������ ���Ͽ� ���� �˾ƺ���
	*/
	
	void Producer(std::queue<std::string>* downloaded_pages, std::mutex* m, int index)
	{
		for (int i = 0; i < 5; i++)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100 * index)); //�ٿ�ε�� �ɸ��� �ð��̶� ����, ������ ��ȣ�� Ŭ���� �ð��� �����ɸ���
			std::string content = "������Ʈ : " + std::to_string(i) + " from thread(" + std::to_string(index) + ")\n";

			m->lock();
			//�Ӱ迵��, �����ʹ� ��������̿��� �����Ǿ� �ϹǷ� �Ӱ迵�������� ������ ���Բ� ó���Ѵ�
			downloaded_pages->push(content);
			m->unlock();
		}
	}

	void Consumer(std::queue<std::string>* downloaded_pages, std::mutex* m, int* num_processed)
	{
		//�ٿ�ε尹���� 25���� �ȵƴٸ�
		while (*num_processed < 25)
		{
			m->lock();
			if (downloaded_pages->empty())
			{
				//���� �ٿ�ε�Ϸ�Ȱ� ���ٸ� �ٷ� lock�� �����ϰ� 10�и������嵿�� ����� ��üũ(busy-waiting)
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

	//���� ��޵� �⺻���� �ݺ��ܿ� Ư���� ������ �־� �˸�(notify)�� ���� mutex�� ����ϱ����� ���Ǻ���(condition_value)�� ����Ѵ�
	//std::condition_variable���� Ư�� ���ؽ��� ���ڷ� ������ wait�Լ����� notify�Լ������� ȣ��� ������ ������Ű�� ���ڷγѰܹ��� mutex�� ���ŷ���¿��� ������Ų��.
	void ProducerUsingCV(std::queue<std::string>* downloaded_pages, std::mutex* m, int index, std::condition_variable* cv)
	{
		for (int i = 0; i < 5; i++)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100 * index)); //�ٿ�ε�� �ɸ��� �ð��̶� ����, ������ ��ȣ�� Ŭ���� �ð��� �����ɸ���
			std::string content = "������Ʈ : " + std::to_string(i) + " from thread(" + std::to_string(index) + ")\n";

			m->lock();
			//�Ӱ迵��, �����ʹ� ��������̿��� �����Ǿ� �ϹǷ� �Ӱ迵�������� ������ ���Բ� ó���Ѵ�
			downloaded_pages->push(content);
			m->unlock();
			//Consumer���� Wait���� cv�� �غ�Ǿ����� �˸���
			cv->notify_one();
		}
	}
	void ConsumerUsingCV(std::queue<std::string>* downloaded_pages, std::mutex* m, int* num_processed, std::condition_variable* cv)
	{
		//�ٿ�ε尹���� 25���� �ȵƴٸ�
		while (*num_processed < 25)
		{
			std::unique_lock<std::mutex> lk(*m);
			//������ ���� �ƴҽ� ���ŷ���·δ���Ѵ�
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
		//producer���� ���� ���ϵǸ� ���Ǻ����� ���� ����� consumer���� ���� ���ϵɶ����� ���ŷ���·α�ٸ���
		cv.notify_all();
		for (int i = 0; i < 3; i++) consumers[i].join();
	}
}
