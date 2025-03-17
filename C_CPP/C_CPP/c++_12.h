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
		A a(1); //�Ϲݻ�����
		A b(a); //���������

		A c(A(2)); //������ڰ� ȣ��Ǵ°�?
		//A(2)�� ����c�ʿ� ��ü�� ������ �ѱ�� �������� ����� �Ҹ��� �� �̹Ƿ� �ӽ÷� ������� ��ü�� �ƿ� c�� �ع�����
		//��������� ��ŵ�ϰ� �ӽ÷θ������ ��üA(2)�� ����, �̷� ���������� �������(Copy Elision)�̶� �Ѵ�
		//��쿡���� �����Ϸ��� ��������� �ϰų�, �����ʰų� �� �� �ִ�, �� ���� �Լ������� ��ü�� ����� return�Ұ�� �ݵ�� ��������� �Ͼ��(�������������Ҹ�)
	}

	class MyString
	{
		char* str_content; //���ڿ� �޸���ġ�� ����Ű�� ������
		int str_length; //���ڿ��Ǳ���
		int str_capacity; //���� �Ҵ�� �뷮
	public:
		MyString();
		MyString(const char* str); //���ڿ��κ��� ������ ����
		MyString(const MyString& origin); //���������
		MyString(MyString&& str) noexcept; //�̵�������
		virtual ~MyString(); //�Ҹ���
	public:

		void reserve(int size); //�������
		int length() const; //������̹�ȯ
		const char* c_str() const;
		MyString operator+(const MyString& s); //���ڿ�����, +������������
		MyString& operator=(const MyString& s); //���ڿ�����, =������������
		MyString& operator=(MyString&& s); //���ڿ�����, =������������, �̵����Կ�����
		friend std::ostream& operator<<(std::ostream& out, const MyString& str); //��¿�����������
	};
	MyString::MyString()
	{
		std::cout << "�⺻������ ȣ��1" << '\n';
		str_length = 0;
		str_capacity = 0;
		str_content = nullptr;
	}
	MyString::MyString(const char* str)
	{
		std::cout << "�⺻������ ȣ��2" << '\n';
		str_length = strlen(str);
		str_capacity = str_length + 1;
		str_content = new char[str_capacity];
		for (int i = 0; i < str_length; i++) str_content[i] = str[i];
		str_content[str_length] = '\0';
	}
	MyString::MyString(const MyString& origin)
	{
		std::cout << "��������� ȣ��" << '\n';
		str_length = origin.str_length;
		str_capacity = str_length + 1;
		str_content = new char[str_capacity];
		for (int i = 0; i <= str_length; i++) str_content[i] = origin.str_content[i];
	}
	
	MyString::MyString(MyString&& str) noexcept
	{
		std::cout << "�̵� ������ ȣ�� !" << std::endl;
		str_length = str.str_length;
		str_content = str.str_content;
		str_capacity = str.str_capacity;

		// �ӽ� ��ü �Ҹ� �ÿ� �޸𸮸� ��������
		// ���ϰ� �Ѵ�.
		str.str_content = nullptr;
	}
	
	MyString::~MyString() 
	{
		std::cout << "�Ҹ��� ȣ��" << '\n';
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
		MyString ret; //�ӽð�üret���� (�⺻������ȣ��1)
		ret.reserve(str_length + s.length() + 1);
		for (int i = 0; i < str_length; i++) ret.str_content[i] = str_content[i];
		for (int i = 0; i <= s.length(); i++) ret.str_content[str_length + i] = s.str_content[i]; //str1_cotent + str2_cotent + '\0';
		ret.str_length = str_length + s.length();
		return std::move(ret); //��ȯ�� rvo�� ��������ڸ� ȣ������ ����, std::move�� ����ȣ��, ��������ڸ� �ι� ȣ���ؾ��Ѵ�(str+str�Ҷ��ѹ�, �����Ҷ��ѹ�)
	}
	MyString& MyString::operator=(const MyString& s)
	{
		std::cout << "���Կ���" << std::endl;
		if (s.str_length > str_capacity) {
			delete[] str_content;
			str_capacity = s.str_length + 1;
			str_content = new char[str_capacity];
		}
		str_length = s.str_length;
		for (int i = 0; i <= str_capacity; i++) {
			str_content[i] = s.str_content[i];
		}
		return *this;
	}
	MyString& MyString::operator=(MyString&& s)
	{
		std::cout << "�̵����Կ���" << std::endl;
		str_content = s.str_content;
		str_length = s.str_length;
		str_capacity = s.str_capacity;

		s.str_content = nullptr;
		s.str_length = 0;
		s.str_capacity = 0;
		return *this;
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
		MyString str3 = str1 + str2; //MyString str3(str1.operator+(str2))�� ����
		//operator+�����ڴ� �ӽú�����ürvalue�� �����ϳ� ����������� const ref�̹Ƿ� �������ִ�

		std::cout << str3 << '\n';
		//a�� �ּҰ��� &�����ڵ��� ���� �˾Ƴ� �� �ִ�, ������ �ּҰ��� ���Ҽ� �ִ� 
		//���� �������̶��ϸ� �������� ��ȣ�� ����, ������ �纯�� �� �� �ִ�
		int a = 3; //3�� ���������� �ӽ÷� �ִٰ� ������°�, rvalue�� �Ѵ�
		int& l_a = a; //���������۷���
		//int& r_a = 3; //rvalue�� �ּҸ� ������ �� ����, �ӽð��̱⶧��
		int&& r_a = 3; //���������۷��� rvalue�� ����ų���ִ�, �ӽð�ü rvalue�� �Ҹ�����ʵ��� �Ѵ�

	}

	int& func1(int& a) { return a; }
	int func2(int b) { return b; }
	void Exam3()
	{
		int a = 3;
		func1(a) = 4;
		std::cout << &func1(a) << &a << '\n';

		int b = 2;
		a = func2(b); //�ӽð��� �޾Ƽ� ����
		//func2(b) = 5; //rvalue���� ���� ������ �� ����
	}
	
	void Exam4()
	{
		/*
		* ������ ��쿡�� ���ο� ���� �߰��� �Ҵ��Ѹ޸𸮰� ���ڸ���
		* ���ο� �޸𸮸� �Ҵ��� �Ŀ� �����ǿ��Ҹ� ���ο� �޸𸮷� �ű��
		* �̶� ��������ڸ� �̿��ϹǷ� ���ܰ��߻��Ͽ��� ���� ���Ҵ� ���������⿡ ���������
		* �̵������� ��� ���� �޸��� ���ҵ��� �̵��� ��������Ƿ� �����Ҵ��� �޸𸮸� ���Ҹ� ������ �� ����
		* 
		*/
		MyString s("abc");
		std::vector<MyString> vec;
		vec.resize(0);

		std::cout << "ù ��° �߰� ---" << std::endl;
		vec.push_back(s);
		std::cout << "�� ��° �߰� ---" << std::endl;
		vec.push_back(s);
		std::cout << "�� ��° �߰� ---" << std::endl;
		vec.push_back(s);
	}

	template <typename T>
	void MYSwap(T& a, T& b)
	{
		T tmp(a); //���������ȣ��, ���� �����ص�
		a = std::move(b); //a�� b�� ���� �̵�, ���縸ŭ ������尡�߻���������
		b = std::move(tmp); //b�� tmp�� ���� �̵�, ���縸ŭ ������尡 �߻���������
	}
	void Exam5()
	{
		MyString str1("abc");
		MyString str2("def");
		MYSwap(str1, str2); //�⺻����, ���������->���Կ�����->���Կ����� �� ������ ���簡 �߻��Ѵ�
	}

	class A2
	{
	public:
		A2() { std::cout << "�Ϲݻ�����ȣ��" << '\n'; }
		A2(const A2& a) { std::cout << "���������ȣ��" << '\n'; }
		A2(const A2&& a) { std::cout << "�̵�������ȣ��" << '\n'; }
	};

	void Exam6()
	{
		A2 a; //�Ϲݻ�����ȣ��
		A2 b(a); //��������� ȣ��
		A2 c(std::move(a)); //�̵�������ȣ��, std::move�� rvalue�� ��ȯ��Ų��
	}

	class A3
	{
	public:
		A3() {std::cout << "NormalConstructor\n";}
		A3(const A3& a3) { std::cout << "CopyConstructor\n"; }
		A3(A3&& a3) { std::cout << "MoveConstructor\n"; }
	};
	class B3 
	{
		A3 a3;
	public:
		//��������������� ������带 ���ϱ����� ���������� �޾Ƽ� �ذ��Ϸ��Ѵ�.
		//A3&& _a3�� rvalue�з������Լ�(������)�� ȣ���Ѵ�, �̶� A3Ŭ������ �����ڿ� ���ڸ� �Ѱ��ٶ�
		//_a3�� ��ü�� ��ü�̹Ƿ� move�� �ѹ� ������ �Ѱ����� ������ A3Ŭ�������忡�� �Ϲ�A3��ü(lvaue)�� ���⶧���� move��
		//�ѹ��� ����� rvalue�� ĳ������ ���ڷ� �Ѱܾ� �ùٸ��� �̵��������� ȣ���� ����������
		B3(A3&& _a3) : a3(std::move(_a3)) {}
	};
	
	void Exam7()
	{
		A3 a3;
		B3 B(std::move(a3));
	}

	class A4 {};
	void g(A4& a) { std::cout << "������ ���۷��� ȣ��\n"; }
	void g(const A4& a) { std::cout << "������ ��� ���۷��� ȣ��\n"; }
	void g(A4&& a) { std::cout << "������ ���۷��� ȣ��\n"; };

	template<typename T>
	void Wrapper(T u)
	{
		g(u);
	}

	template<typename T>
	void WrapperRef(T& u)
	{
		g(u);
	}

	template<typename T>
	void Wrapperforward(T&& u)
	{
		g(std::forward<T>(u));
	}
	void Exam8()
	{
		A4 a;
		const A4 ca;
		g(a);
		g(ca);
		g(A4());
		std::cout << "Wrapper(T u), g(u)\n";
		Wrapper(a); //T(A4) u�� ȣ��, g(u)�� lvalue u�� ȣ��, ������ ���۷��� ȣ��
		Wrapper(ca); //T(A4) u�� ȣ��, g(u)�� lvalue u�� ȣ��, ������ ���۷��� ȣ��
		Wrapper(A4()); //T(A4) u�� ȣ��, g(u)�� lvalue u�� ȣ��, ������ ���۷��� ȣ��

		std::cout << "Wrapper(T& u), g(u)\n";
		WrapperRef(a); //T(A4&) �� ����u(call by ref)�� ȣ��, ���簡�ƴϹǷ� �����Ϸ��� A4&�� �߷��Ͽ� ���������۷���ȣ��
		WrapperRef(ca); //T(const A4&) �� ����u(call by ref)�� ȣ��, ���簡�ƴϹǷ� �����Ϸ��� const A4&�� �߷��Ͽ� ������������۷���ȣ��
		//WrapperRef(A4()); //A()�� rvalue�̳� &������ ȣ���ϹǷ� ȣ��Ұ���!

		/*
		* ���c++���� ���۷�����ħ��Ģ(Reference Collapsing Rule)�� ���� T�� Ÿ���� �߷��Ѵ�
		* &(lvaue Ref)�� 1�� ���� &&(rvalue Ref)�� 0���� ���� ���·� OR�������� ���·� Ÿ���� �߷��Ѵ�
		* typedef int& T;
		* T& r1; //int& & ������, lvalue, lvalue �̹Ƿ� r1�� int&�� ����
		* T&& r2; //int& &&�� ����, lvalue, rvalue�̹Ƿ� r2�� int&�� ����
		*
		* typedef int&& U;
		* U& r3; //int&& &�� ����, rvalue, lvalue�̹Ƿ� r3�� int&�� ����
		* U&& r4; //int&& &&�� ���� rvalue, rvalue�̹Ƿ� r4�� int&&�� ����
		*/
		std::cout << "Wrapper(T& u), g(std::forward<T>(u))\n"; //���������۷���
		Wrapperforward(a); //A4&& &�� ���·� �Ѿ�Ƿ� A4&�� ����
		Wrapperforward(ca); //const A4&& &�����·� �Ѿ�Ƿ� const A4&�� ����
		Wrapperforward(A4()); //rvalue(A4)&&���·� ���� �Ѿ��, �ٸ� u�� A4���� �Ű����������̹Ƿ� lvalue���±⶧���� A4Ŭ�������ùٸ��������� MOVE,FORWARD�� ĳ�������ʿ�
	}
}