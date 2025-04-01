#pragma once
#include "std.h"
//본 챕터에서는 c++에서 호출 가능한 모든것을 포괄해서 나타내는 Callable에 대해 알아본다, 쓰레드를 배우기전 짧게 거치는역할
namespace ch14
{
	//Callable이란 호출 할 수 있는것을 의미한다, C++에서는 ()로 호출 할 수 있는 모든것을 Callable이라 일컫는다
	struct S
	{
		void operator()(const int& a, const int& b) { std::cout << "a + b = " << a + b << '\n'; }
	};
	void Exam1()
	{
		S s;
		s(3, 5); //same s.operator()(3, 5);
		
		//또다른 예시로 람다표현식이 있다
		void (*func)(const int& a, const int& b) = [](const int& a, const int& b)->void {std::cout << "a + b = " << a + b << '\n'; };
		func(4, 6);
	}
	//C++에서는 이러한 Callable들을 객체의 형태로 보관 할 수 있는 std::function이라는 클래스를 제공한다 일반적인 함수포인터는 함수들만 지정이 가능하지만
	//function의 경우 함수뿐만이 아닌 모든 Callable들을 보관 할 수 있는 객체
	//function의 여러가지 사용예는 다음과같다

	int some_func1(const std::string& a) { std::cout << "func1호출!" << a << '\n';  return 0; }
	struct struct_func2 
	{
		void operator()(const char& c) { std::cout << "func2호출!" << c << '\n'; }
	};
	void Exam2()
	{
		//템플릿인자로 Callable할 타입을 받는다
		std::function<int(const std::string& a)> f1 = some_func1;//함수포인터처럼 인자를저장한다
		f1("Hello!");
		std::function<void(const char& c)> f2 = struct_func2();//객체를저장한다
		f2('H');
		std::function<void()> f3 = []() {std::cout << "func3호출!" << '\n'; };//람다를 함수포인터처럼 인자로 저장한다
		f3();
	}

	//function의 사용에서 멤버함수의 호출의 경우 조건이 있다, 위의 operator()의 경우 this형태로 자기자신의 operator를 호출하므로 알 수 있으나
	//일반적인 멤버함수는 해당클래스를 사용하는 모든 객체가 공유하기 때문에 정확한 지정이 되지 않는다면 어떠한 객체의 멤버함수를 호출하는지 알 수 없는 문제가 생긴다
	class A
	{
	public:
		A(int _c) : c(_c) {}
		int some_func() { std::cout << "비상수함수호출:" << c << '\n'; return 0; }
		int some_func_const() const { std::cout << "상수함수호출:" << c << '\n'; return 0; }
	private:
		const int c;
	};
	void Exam3()
	{
		A a(5);
		//std::function<int()> func = a.some_func; //컴파일에러발생
		//이를 해결하기위한 기본적인 방법으로 &클래스명::함수명 으로 멤버함수의 포인터주소를 변수값으로 넘겨받는다
		//이때 어떠한 객체에서 해당멤버함수를 호출해야할지 알아야 하기때문에 템플릿인자로 해당클래스의 객체를 넘겨받아 사용한다, 즉 함수반환형(객체인자) 형태로 템플릿인자를 구성한다
		//참조로 객체를 넘기기때문에 원본객체의 멤버함수가 호출되며 복사등으로 호출될수도 있으니 이에염두해 템플릿인자를 구성시킨다

		std::function<int(A&)> func1 = &A::some_func; //멤버함수의경우는 일반적인 함수의 이름과 다르게 묵시적인 변환이 일어나지 않기때문에 &연산자를 이용해 주소값을 명시적으로 전달해야한다
		func1(a); //저장된 function변수에 객체를 전달함으로써 저장된 멤버함수의 포인터를 이용해 해당함수를 호출시킨다

		std::function<int(const A&)> func2 = &A::some_func_const;
		func2(a);
	}
	
	//std::vector<std::vector<int>>형태가 있을때 해당 원소들의 사이즈를 기반으로한 std::vector<int>를 만들어주는 경우를생각해보자
	void Exam4()
	{
		std::vector<int> a(1);
		std::vector<int> b(4);
		std::vector<int> c(3);
		std::vector<int> d(2);
		std::vector<std::vector<int>> container; //1,4,3,2의 std::vector<int> 들을 인자로 갖는다
		container.push_back(a);
		container.push_back(b);
		container.push_back(c);
		container.push_back(d);

		std::vector<int> size_vec(4); //해당벡터를 위의 컨테이너의 사이즈들을 집어넣으려한다(1,4,3,2 를원소로)

		//std::transform(container.begin(), container.end(), size_vec.begin(), &std::vector<int>::size); //에러가발생!
		
		//이때 마지막 인자로 넘겨지는 함수가 일반함수일 경우에는 상관없으나 멤버함수일경우 컴파일에러가발생한다
		//_Func(*_UFirst); 형태로 func템플릿인자가 넘어가는데 이는 &std::vector<int>(*_UFirst); 의 형태로 변환된다
		//그러나 멤버함수의경우엔 (*_UFirst).(*&std::vector<int>::size)가 되거나 _UFirst->(*&std::vector<int>::size)로 호출해야하므로 오류가 생긴다
		std::function<size_t(const std::vector<int>&)> sz_func = &std::vector<int>::size; //function을 이용해 Callable한 멤버함수 size를 받아 이를 넘긴다
		std::transform(container.begin(), container.end(), size_vec.begin(), sz_func);

		//이를 좀더 스마트하게 해결하기 위해 function객체를 리턴해버리는 함수 std::mem_fn을 이용한다, 람다함수로 대체또한 가능하다, 결국 특정한 객체를 함수의 매개변수 인자로
		//받고 해당객체의 멤버함수를 함수포인터로 리턴해주면 이를 사용하는 형태이기 때문
		std::transform(container.begin(), container.end(), size_vec.begin(), std::mem_fn(&std::vector<int>::size));
		for (std::vector<int>::const_iterator iter = size_vec.begin(); iter != size_vec.end(); iter++)
			std::cout << *iter << ' ';
	}

	//함수객체의 생성시 매개변수의 인자를 지정이 가능하다, 이를위해 std::bind를 사용한다
	void Exam5()
	{
		std::function<void(int x, int y)> func = [](int x, int y)->void {std::cout << x << " + " << y << " = " << x + y << '\n'; };
		auto add_with_2 = std::bind(func, std::placeholders::_1, 2);
		add_with_2(5, 10, 5); //뒤의것들은 무시되고 첫번째 인자만 받아진다
		auto add_with = std::bind(func, std::placeholders::_1, std::placeholders::_2);
		add_with(1, 3); //순서를반대로

		std::function<void(int& x, int y)> func2 = [](int& x, int y)->void {x += y; }; 
		int a = 1, b = 3;
		std::cout << a << '\n';
		auto addfunc = std::bind(func2, std::ref(a), std::placeholders::_1); //인수가 참조로 받을때 std::ref로 넘겨준다, const ref의 경우 cref로호출
		addfunc(b);
		std::cout << a << '\n';
	}
}