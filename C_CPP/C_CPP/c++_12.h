#pragma once
#include "std.h"
namespace ch12
{
	class A 
	{
		int data;
	public:
		A(int _data) : data(_data) { std::cout << "Normal Constructor \n"; }
		A(const A& _a) : data(_a.data) { std::cout << "Copy Constructor \n"; }
	};
	void Exam()
	{
		A a(1); //일반생성자
		A b(a); //복사생성자

		A c(A(2)); //어떤생성자가 호출되는가?
		//A(2)은 변수c쪽에 객체를 생성해 넘기고 스코프를 벗어나면 소멸할 것 이므로 임시로 만들어진 객체를 아예 c로 해버린다
		//복사생성을 스킵하고 임시로만들어진 객체A(2)를 쓴다, 이런 생략과정을 복사생략(Copy Elision)이라 한다
		//경우에따라 컴파일러가 복사생략을 하거나, 하지않거나 할 수 있다, 한 예로 함수내에서 객체를 만들어 return할경우 반드시 복사생략이 일어난다(스코프를벗어나면소멸)
	}

	class MyString
	{
		char* str_content; //문자열 메모리위치를 가리키는 포인터
		int str_length; //문자열의길이
		int str_capacity; //현재 할당된 용량
	public:
		MyString();
		MyString(const char* str); //문자열로부터 복사해 생성
		MyString(const MyString& origin); //복사생성자
		MyString(MyString&& str) noexcept; //이동생성자
		virtual ~MyString(); //소멸자
	public:

		void reserve(int size); //사이즈예약
		int length() const; //현재길이반환
		const char* c_str() const;
		MyString operator+(const MyString& s); //문자열연결, +연산자재정의
		friend std::ostream& operator<<(std::ostream& out, const MyString& str); //출력연산자재정의
	};
	MyString::MyString()
	{
		std::cout << "기본생성자 호출1" << '\n';
		str_length = 0;
		str_capacity = 0;
		str_content = nullptr;
	}
	MyString::MyString(const char* str)
	{
		std::cout << "기본생성자 호출2" << '\n';
		str_length = strlen(str);
		str_capacity = str_length + 1;
		str_content = new char[str_capacity];
		for (int i = 0; i < str_length; i++) str_content[i] = str[i];
		str_content[str_length] = '\0';
	}
	MyString::MyString(const MyString& origin)
	{
		std::cout << "복사생성자 호출" << '\n';
		str_length = origin.str_length;
		str_capacity = str_length + 1;
		str_content = new char[str_capacity];
		for (int i = 0; i <= str_length; i++) str_content[i] = origin.str_content[i];
	}
	
	MyString::MyString(MyString&& str) noexcept
	{
		std::cout << "이동 생성자 호출 !" << std::endl;
		str_length = str.str_length;
		str_content = str.str_content;
		str_capacity = str.str_capacity;

		// 임시 객체 소멸 시에 메모리를 해제하지
		// 못하게 한다.
		str.str_content = nullptr;
	}
	
	MyString::~MyString() 
	{
		std::cout << "소멸자 호출" << '\n';
		delete[] str_content;
	}
	void MyString::reserve(int size)
	{
		if (size <= str_capacity) return;
		const char* content = str_content;
		str_content = new char[size];
		str_capacity = size;
		if (content == nullptr) return;
		for (int i = 0; i <= str_length; i++) str_content[i] = content[i]; //copy 0 ~'\0';
		delete[] content;
	}
	MyString MyString::operator+(const MyString& s)
	{
		MyString ret; //임시객체ret생성 (기본생성자호출1)
		ret.reserve(str_length + s.length() + 1);
		for (int i = 0; i < str_length; i++) ret.str_content[i] = str_content[i];
		for (int i = 0; i <= s.length(); i++) ret.str_content[str_length + i] = s.str_content[i]; //str1_cotent + str2_cotent + '\0';
		ret.str_length = str_length + s.length();
		return std::move(ret); //반환시 rvo로 복사생성자를 호출하지 않음, std::move시 강제호출, 복사생성자를 두번 호출해야한다(str+str할때한번, 복사할때한번)
	}
	
	int MyString::length() const
	{
		return str_length;
	}
	const char* MyString::c_str() const
	{
		return str_content;
	}
	std::ostream& operator<<(std::ostream& out, const MyString& str)
	{
		return out << str.c_str();
	}
	void Exam2()
	{
		MyString str1("abc");
		MyString str2("def");
		std::cout << "-------------" << std::endl;
		MyString str3 = str1 + str2; //MyString str3(str1.operator+(str2))와 같다
		//operator+연산자는 임시변수객체rvalue를 리턴하나 복사생성자의 const ref이므로 받을수있다

		std::cout << str3 << '\n';
		//a의 주소값을 &연산자등을 통해 알아낼 수 있다, 변수등 주소값을 취할수 있는 
		//값을 좌측값이라하며 좌측값은 등호의 왼쪽, 오른쪽 양변에 올 수 있다
		int a = 3; //3은 우측값으로 임시로 있다가 사라지는값, rvalue라 한다
		int& l_a = a; //좌측값레퍼런스
		//int& r_a = 3; //rvalue는 주소를 참조할 수 없다, 임시값이기때문
		int&& r_a = 3; //우측값레퍼런스 rvalue만 가리킬수있다, 임시객체 rvalue가 소멸되지않도록 한다

	}

	int& func1(int& a) { return a; }
	int func2(int b) { return b; }
	void Exam3()
	{
		int a = 3;
		func1(a) = 4;
		std::cout << &func1(a) << &a << '\n';

		int b = 2;
		a = func2(b); //임시값을 받아서 복사
		//func2(b) = 5; //rvalue에는 값을 지정할 수 없다
	}
	
	void Exam4()
	{
		/*
		* 벡터의 경우에는 새로운 원소 추가시 할당한메모리가 모자르면
		* 새로운 메모리를 할당한 후에 기존의원소를 새로운 메모리로 옮긴다
		* 이때 복사생성자를 이용하므로 예외가발생하여도 원본 원소는 문제가없기에 상관없으나
		* 이동생성의 경우 기존 메모리의 원소들이 이동해 사라졌으므로 새로할당한 메모리를 섣불리 해제할 수 없다
		* 
		* 
		*/
		MyString s("abc");
		std::vector<MyString> vec;
		vec.resize(0);

		std::cout << "첫 번째 추가 ---" << std::endl;
		vec.push_back(s);
		std::cout << "두 번째 추가 ---" << std::endl;
		vec.push_back(s);
		std::cout << "세 번째 추가 ---" << std::endl;
		vec.push_back(s);
	}
}