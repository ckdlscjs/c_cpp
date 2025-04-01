#pragma once
#include "std.h"
//�� é�Ϳ����� c++���� ȣ�� ������ ������ �����ؼ� ��Ÿ���� Callable�� ���� �˾ƺ���, �����带 ������ ª�� ��ġ�¿���
namespace ch14
{
	//Callable�̶� ȣ�� �� �� �ִ°��� �ǹ��Ѵ�, C++������ ()�� ȣ�� �� �� �ִ� ������ Callable�̶� ���´´�
	struct S
	{
		void operator()(const int& a, const int& b) { std::cout << "a + b = " << a + b << '\n'; }
	};
	void Exam1()
	{
		S s;
		s(3, 5); //same s.operator()(3, 5);
		
		//�Ǵٸ� ���÷� ����ǥ������ �ִ�
		void (*func)(const int& a, const int& b) = [](const int& a, const int& b)->void {std::cout << "a + b = " << a + b << '\n'; };
		func(4, 6);
	}
	//C++������ �̷��� Callable���� ��ü�� ���·� ���� �� �� �ִ� std::function�̶�� Ŭ������ �����Ѵ� �Ϲ����� �Լ������ʹ� �Լ��鸸 ������ ����������
	//function�� ��� �Լ��Ӹ��� �ƴ� ��� Callable���� ���� �� �� �ִ� ��ü
	//function�� �������� ��뿹�� ����������

	int some_func1(const std::string& a) { std::cout << "func1ȣ��!" << a << '\n';  return 0; }
	struct struct_func2 
	{
		void operator()(const char& c) { std::cout << "func2ȣ��!" << c << '\n'; }
	};
	void Exam2()
	{
		//���ø����ڷ� Callable�� Ÿ���� �޴´�
		std::function<int(const std::string& a)> f1 = some_func1;//�Լ�������ó�� ���ڸ������Ѵ�
		f1("Hello!");
		std::function<void(const char& c)> f2 = struct_func2();//��ü�������Ѵ�
		f2('H');
		std::function<void()> f3 = []() {std::cout << "func3ȣ��!" << '\n'; };//���ٸ� �Լ�������ó�� ���ڷ� �����Ѵ�
		f3();
	}

	//function�� ��뿡�� ����Լ��� ȣ���� ��� ������ �ִ�, ���� operator()�� ��� this���·� �ڱ��ڽ��� operator�� ȣ���ϹǷ� �� �� ������
	//�Ϲ����� ����Լ��� �ش�Ŭ������ ����ϴ� ��� ��ü�� �����ϱ� ������ ��Ȯ�� ������ ���� �ʴ´ٸ� ��� ��ü�� ����Լ��� ȣ���ϴ��� �� �� ���� ������ �����
	class A
	{
	public:
		A(int _c) : c(_c) {}
		int some_func() { std::cout << "�����Լ�ȣ��:" << c << '\n'; return 0; }
		int some_func_const() const { std::cout << "����Լ�ȣ��:" << c << '\n'; return 0; }
	private:
		const int c;
	};
	void Exam3()
	{
		A a(5);
		//std::function<int()> func = a.some_func; //�����Ͽ����߻�
		//�̸� �ذ��ϱ����� �⺻���� ������� &Ŭ������::�Լ��� ���� ����Լ��� �������ּҸ� ���������� �Ѱܹ޴´�
		//�̶� ��� ��ü���� �ش����Լ��� ȣ���ؾ����� �˾ƾ� �ϱ⶧���� ���ø����ڷ� �ش�Ŭ������ ��ü�� �Ѱܹ޾� ����Ѵ�, �� �Լ���ȯ��(��ü����) ���·� ���ø����ڸ� �����Ѵ�
		//������ ��ü�� �ѱ�⶧���� ������ü�� ����Լ��� ȣ��Ǹ� ��������� ȣ��ɼ��� ������ �̿������� ���ø����ڸ� ������Ų��

		std::function<int(A&)> func1 = &A::some_func; //����Լ��ǰ��� �Ϲ����� �Լ��� �̸��� �ٸ��� �������� ��ȯ�� �Ͼ�� �ʱ⶧���� &�����ڸ� �̿��� �ּҰ��� ��������� �����ؾ��Ѵ�
		func1(a); //����� function������ ��ü�� ���������ν� ����� ����Լ��� �����͸� �̿��� �ش��Լ��� ȣ���Ų��

		std::function<int(const A&)> func2 = &A::some_func_const;
		func2(a);
	}
	
	//std::vector<std::vector<int>>���°� ������ �ش� ���ҵ��� ����� ��������� std::vector<int>�� ������ִ� ��츦�����غ���
	void Exam4()
	{
		std::vector<int> a(1);
		std::vector<int> b(4);
		std::vector<int> c(3);
		std::vector<int> d(2);
		std::vector<std::vector<int>> container; //1,4,3,2�� std::vector<int> ���� ���ڷ� ���´�
		container.push_back(a);
		container.push_back(b);
		container.push_back(c);
		container.push_back(d);

		std::vector<int> size_vec(4); //�ش纤�͸� ���� �����̳��� ��������� ����������Ѵ�(1,4,3,2 �����ҷ�)

		//std::transform(container.begin(), container.end(), size_vec.begin(), &std::vector<int>::size); //�������߻�!
		
		//�̶� ������ ���ڷ� �Ѱ����� �Լ��� �Ϲ��Լ��� ��쿡�� ��������� ����Լ��ϰ�� �����Ͽ������߻��Ѵ�
		//_Func(*_UFirst); ���·� func���ø����ڰ� �Ѿ�µ� �̴� &std::vector<int>(*_UFirst); �� ���·� ��ȯ�ȴ�
		//�׷��� ����Լ��ǰ�쿣 (*_UFirst).(*&std::vector<int>::size)�� �ǰų� _UFirst->(*&std::vector<int>::size)�� ȣ���ؾ��ϹǷ� ������ �����
		std::function<size_t(const std::vector<int>&)> sz_func = &std::vector<int>::size; //function�� �̿��� Callable�� ����Լ� size�� �޾� �̸� �ѱ��
		std::transform(container.begin(), container.end(), size_vec.begin(), sz_func);

		//�̸� ���� ����Ʈ�ϰ� �ذ��ϱ� ���� function��ü�� �����ع����� �Լ� std::mem_fn�� �̿��Ѵ�, �����Լ��� ��ü���� �����ϴ�, �ᱹ Ư���� ��ü�� �Լ��� �Ű����� ���ڷ�
		//�ް� �ش簴ü�� ����Լ��� �Լ������ͷ� �������ָ� �̸� ����ϴ� �����̱� ����
		std::transform(container.begin(), container.end(), size_vec.begin(), std::mem_fn(&std::vector<int>::size));
		for (std::vector<int>::const_iterator iter = size_vec.begin(); iter != size_vec.end(); iter++)
			std::cout << *iter << ' ';
	}

	//�Լ���ü�� ������ �Ű������� ���ڸ� ������ �����ϴ�, �̸����� std::bind�� ����Ѵ�
	void Exam5()
	{
		std::function<void(int x, int y)> func = [](int x, int y)->void {std::cout << x << " + " << y << " = " << x + y << '\n'; };
		auto add_with_2 = std::bind(func, std::placeholders::_1, 2);
		add_with_2(5, 10, 5); //���ǰ͵��� ���õǰ� ù��° ���ڸ� �޾�����
		auto add_with = std::bind(func, std::placeholders::_1, std::placeholders::_2);
		add_with(1, 3); //�������ݴ��

		std::function<void(int& x, int y)> func2 = [](int& x, int y)->void {x += y; }; 
		int a = 1, b = 3;
		std::cout << a << '\n';
		auto addfunc = std::bind(func2, std::ref(a), std::placeholders::_1); //�μ��� ������ ������ std::ref�� �Ѱ��ش�, const ref�� ��� cref��ȣ��
		addfunc(b);
		std::cout << a << '\n';
	}
}