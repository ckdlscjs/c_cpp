#pragma once
#include "std.h"
namespace MyString5
{
	class MyString
	{
	private:
		char* str;
		int len;
	public:
		//문자하나로부터생성
		MyString(const char c);
		//문자열로부터생성
		MyString(const char* _str);
		//복사생성자
		MyString(const MyString& _str);
		//소멸자
		~MyString();

	public:
		//const로호출
		int Size() const;
		void Print() const;
		MyString& Assign(const char* _str);
		MyString& Assign(const MyString& _str);
		char At(int idx) const;
		MyString& Insert(int loc, const MyString& _str);
		MyString& Insert(int loc, const char* _str);
		MyString& Insert(int loc, char c);
		MyString& Erase(int loc, int num);
		int Find(const MyString& _str) const;
		int Find(const char* _str) const;
		int Find(char _c) const;
		bool operator==(const MyString& _str);
		char& operator[](const int idx) const;
		friend ostream& operator<<(std::ostream& out, const MyString& _str)
		{
			return out << _str.str;
		}
	};
	MyString::MyString::MyString(const char c)
	{
		str = new char[2];
		str[0] = c;
		str[1] = '\0';
		len = 1;
	}
	MyString::MyString::MyString(const char* _str)
	{
		len = 0;
		while (*(_str + len) != '\0') len++;
		str = new char[len + 1];
		for (int i = 0; i < len; i++)
			str[i] = _str[i];
		str[len] = '\0';
	}
	MyString::MyString::MyString(const MyString& _str)
	{
		len = _str.Size();
		str = new char[len + 1];
		for (int i = 0; i < len; i++)
			str[i] = _str.str[i];
		str[len] = '\0';
	}
	MyString::MyString::~MyString()
	{
		std::cout << str << " 의 소멸자호출\n";
		delete[] str;
	}
	int MyString::MyString::Size() const
	{
		return len;
	}
	void MyString::MyString::Print() const
	{
		std::cout << str << '\n';
	}
	MyString& MyString::MyString::Assign(const char* _str)
	{
		delete[] str;
		len = 0;
		while (_str[len] != '\0') len++;
		str = new char[len + 1];
		for (int i = 0; i < len; i++)
			str[i] = _str[i];
		str[len] = '\0';
		return *this;
	}
	MyString& MyString::MyString::Assign(const MyString& _str)
	{
		delete[] str;
		len = _str.Size();
		str = new char[len + 1];
		for (int i = 0; i < len; i++)
			str[i] = _str.str[i];
		str[len] = '\0';
		return *this;
	}
	char MyString::MyString::At(int idx) const
	{
		if (idx < 0 || idx >= len) return NULL;
		return str[idx];
	}
	MyString& MyString::MyString::Insert(int loc, const MyString& _str)
	{
		if (loc < 0) return *this;
		if (len < loc) loc = len;
		int _len = len + _str.Size();
		char* temp = new char[_len + 1];
		int idx = 0;
		for (idx = 0; idx < loc; idx++)
			temp[idx] = str[idx];
		for (int i = 0; i < _str.Size(); i++)
			temp[idx++] = _str.str[i];
		for (int i = loc; i < len; i++)
			temp[idx++] = str[i];
		temp[_len] = '\0';
		delete[] str;
		str = temp;
		len = _len;
		return *this;
	}
	MyString& MyString::MyString::Insert(int loc, const char* _str)
	{
		return this->Insert(loc, MyString(_str));
	}
	MyString& MyString::MyString::Insert(int loc, char c)
	{
		return this->Insert(loc, MyString(c));
	}
	MyString& MyString::MyString::Erase(int loc, int num)
	{
		if (num < 0 || loc < 0 || loc > len || num > len) return *this;
		for (int i = loc + num; i < len; i++)
			str[i - num] = str[i];
		len -= num;
		str[len] = '\0';
		return *this;
	}
	int MyString::MyString::Find(const MyString& _str) const
	{
		if (_str.Size() <= 0) return -1;
		if (_str.Size() > len) return -1;
		for (int i = 0; i <= len - _str.Size(); i++)
		{
			int j;
			for (j = 0; j < _str.Size(); j++)
				if (str[i + j] != _str.str[j]) break;
			if (j == _str.Size()) return i;
		}
		return -1;
	}
	int MyString::MyString::Find(const char* _str) const
	{
		return Find(MyString(_str));
	}
	int MyString::MyString::Find(char _c) const
	{
		return Find(MyString(_c));
	}
	bool MyString::MyString::operator==(const MyString& _str)
	{
		if (len != _str.Size()) return false;
		for (int i = 0; i < len; i++)
			if (str[i] != _str.At(i)) return false;
		return true;
	}
	char& MyString::MyString::operator[](const int idx) const
	{
		return str[idx];
	}
	void Exam()
	{
		MyString str1("Check1");
		MyString str2 = str1;
		str2.Assign("Check2");
		std::cout << (str1 == str2 ? 1 : 0) << '\n';
		MyString str3("abcdef");
		str3[3] = 'c';
		std::cout << str3 << '\n';
	}
}

namespace Complex
{
	//허수 클래스 z = a + bi;
	class Complex
	{
	private:
		double real, img; //실수부, 허수부

	private:
		double GetNumber(const char* _str, int begin, int end)
		{
			double ret = 0.0f;
			double decimal_after = 1.0f;
			int minus = 1;
			bool is_int = true;
			for (begin; begin <= end; begin++)
			{
				if (_str[begin] == ' ' || _str[begin] == '+')
					continue;
				else if (_str[begin] == '-')
				{
					minus = -1;
					continue;
				}
				else if (_str[begin] == '.')
				{
					is_int = false;
					continue;
				}
				else if (is_int)
				{
					ret *= 10.0f;
					ret += (_str[begin] - '0');
				}
				else
				{
					decimal_after *= 0.1f;
					ret += (_str[begin] - '0') * decimal_after;
				}
			}
			return ret * minus;
		}
	public:
		Complex(double _real, double _img) : real(_real), img(_img) {}
		Complex(const char* _str)
		{
			//_str -> 부호, 실수부, 부호, 허수부 i
			int begin = 0, end = std::strlen(_str), pos_oper = -1;
			bool is_i = _str[end - 1] == 'i';
			for (int i = end - 1; i >= begin; i--)
			{
				if (_str[i] == '+' || _str[i] == '-')
				{
					pos_oper = i;
					break;
				}
			}
			if (!is_i)
			{
				real = GetNumber(_str, begin, end - 1);
			}
			else
			{
				if (pos_oper > 0)
				{
					real = GetNumber(_str, begin, pos_oper - 1);
					img = GetNumber(_str, pos_oper, end - 2);
				}
				else
				{
					img = GetNumber(_str, pos_oper, end - 2);
				}
			}
		}
		Complex(const Complex& c) : real(c.real), img(c.img) {} //복사생성자
		//내부의 값을 변경할 필요 없이 새로운 피연산자를 생성해 리턴하므로 const로 선언한다
		/*
		* 자기자신을 반환하는 이항대입관련된 연산자들은 멤버함수로, 일반 연산자는 friend를 이용한다.
		Complex operator+(const Complex& c) const
		{
			return Complex(real + c.real, img + c.img);
		}
		Complex operator-(const Complex& c) const
		{
			return Complex(real - c.real, img - c.img);
		}
		Complex operator*(const Complex& c) const
		{
			return Complex(real*c.real - img*c.img, real * c.img + img * c.real); // (a1a2 - b1b2) + (a1b2 + a2b1)i
		}
		Complex operator/(const Complex& c) const // ((a1a2 + b1b2) + (b1a2 - a1b2)i)  / (a2^2 + b2^2)
		{
			return Complex((real * c.real + img * c.img)  / (c.real*c.real + c.img * c.img), (img * c.real- real * c.img) / (c.real * c.real + c.img * c.img));
		}
		*/
		Complex& operator=(const Complex& c)
		{
			real = c.real;
			img = c.img;
			return *this;
		}
		Complex& operator+=(const Complex& c) 
		{
			*this = *this + c;
			return *this;
		}
		Complex& operator-=(const Complex& c) 
		{
			*this = *this - c;
			return *this;
		}
		Complex& operator*=(const Complex& c)
		{
			*this = *this * c;
			return *this;
		}
		Complex& operator/=(const Complex& c)
		{
			*this = *this / c;
			return *this;
		}
		/*
		* //컴파일러에서 생성자를 이용해서 알아서 형변환해준다 즉
		* a = a.operator+(Complex("-1.1 + 3.923i")); 로 처리된다
		Complex operator+(const char* _str)
		{
			return *this + Complex(_str);
		}
		Complex operator-(const char* _str)
		{
			return *this - Complex(_str);
		}
		Complex operator*(const char* _str)
		{
			return *this * Complex(_str);
		}
		Complex operator/(const char* _str)
		{
			return *this / Complex(_str);
		}
		*/
		friend ostream& operator<<(std::ostream& out, const Complex& c) //출력연산자 오버로딩
		{
			out << '(' << c.real << ", " << c.img << ')';
			return out;
		}
		friend Complex operator+(const Complex& a, const Complex& b)
		{
			return Complex(a.real + b.real, a.img + b.img);
		}
		friend Complex operator-(const Complex& a, const Complex& b)
		{
			return Complex(a.real - b.real, a.img - b.img);
		}
		friend Complex operator*(const Complex& a, const Complex& b)
		{
			return Complex(a.real * b.real - a.img * b.img, a.real * b.img + a.img * b.real); // (a1a2 - b1b2) + (a1b2 + a2b1)i
		}
		friend Complex operator/(const Complex& a, const Complex& b)
		{
			return Complex((a.real * b.real + a.img * b.img) / (b.real * b.real + b.img * b.img), (a.img * b.real - a.real * b.img) / (b.real * b.real + b.img * b.img));
		}
	};
	/*
	Complex operator+(const Complex& a, const Complex& b)
	{
		
	}
	Complex operator+(const Complex& a, const Complex& b)
	{
		return Complex(a.real + b.real, a.img + b.img);
	}
	Complex operator+(const Complex& a, const Complex& b)
	{
		return Complex(a.real + b.real, a.img + b.img);
	}
	Complex operator+(const Complex& a, const Complex& b)
	{
		return Complex(a.real + b.real, a.img + b.img);
	}
	*/
	class A
	{
	private:
		void private_func() {}
		int private_num;

		friend class B; //B -> A에접근 가능, A -> B 불가능
		friend void func(); //외부함수 func에서 A클래스의 인자에 접근을 허용한다
	};
	class B
	{
	public:
		void b()
		{
			A a;
			a.private_func();
			a.private_num = 2;
		}
	};
	void func()
	{
		A a;
		a.private_func();
		a.private_num = 3;
	}
	void Exam()
	{
		/*
		Complex a(1.0f, 2.0f);
		Complex b(3.0f, -2.0f);
		
		Complex c(0.0f, 0.0f); //c = 시에 복사생성자가 호출됨
		c = a * b + a / b + a + b; //대입연산자가 호출됨
		
		a += b;
		std::cout << a << '\n';
		std::cout << b << '\n';
		std::cout << b + "-4-3.2i" << '\n';
		Complex c(0, 0);
		std::cout << Complex("-0.5-3.7i") << '\n';
		std::cout << c + "-1.1 + 3.923i";
		*/
		Complex a(0, 0);
		a = "-1.1 + 3.923i" + a;
		a = a + a;
		std::cout << a;
	}
}

namespace Wrapper
{
	class Int
	{
		int data;
		// some other data

	public:
		Int(int data) : data(data) {} //기본생성자
		Int(const Int& i) : data(i.data) {} //복사생성자
		operator int() //int형 형변환, 
		{ 
			return data; 
		} 
	};
	class Test
	{
		int data;
		// some other data

	public:
		Test(int data) : data(data) {} //기본생성자
		Test(const Test& i) : data(i.data) {} //복사생성자
		Test& operator++() //전위증감연산자는 인자가 없음
		{
			data++;
			std::cout << "전위증감연산자!\n";
			return *this;
		}
		Test& operator++(int) //후위증감연산자는 인자가 있음
		{
			data++;
			std::cout << "후위증감연산자!\n";
			return *this;
		}
	};
	void Exam()
	{
		Int x = 3;
		Int x2 = x + 4;
		x2 = x * 2 + x + 4;
		std::cout << x2;
		Test tes = 3;
		++tes;
	}
}

namespace NDimension
{
	class Array
	{
		struct Address
		{
			//0레벨~n-1레벨, n-1레벨은 실제 데이터배열을 가리킨다
			int level;
			//다음레벨의 시작주소를 가리키는 포인터, 맨마지막은 실제 데이터를 가리킨다
			void* next;
		};
		//몇개의 층을 가지는가
		const int dim;
		//해당 층에 몇개의 배열이 존재하는가
		int* size;
		//top주소를 가리키는 포인터(루트)
		Address* root;
	public:
		Array(int _dim, int* _sizes) : dim(_dim)
		{
			size = new int[dim];
			std::memcpy(size, _sizes, sizeof(size));
		}
	};
}