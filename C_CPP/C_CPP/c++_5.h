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
	void Exam()
	{
		MyString str1("Check1");
		MyString str2 = str1;
		str2.Assign("Check2");
		std::cout << (str1 == str2 ? 1 : 0) << '\n';
	}
}

namespace Complex
{
	//허수 클래스 z = a + bi;
	class Complex
	{
	private:
		double real, img; //실수부, 허수부
	public:
		Complex(double _real, double _img) : real(_real), img(_img) {}
		Complex(const Complex& c) : real(c.real), img(c.img) {} //복사생성자
		
		//내부의 값을 변경할 필요 없이 새로운 피연산자를 생성해 리턴하므로 const로 선언한다
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
		friend ostream& operator<<(std::ostream& out, const Complex& c) //출력연산자 오버로딩
		{
			out << '(' << c.real << ", " << c.img << ')';
			return out;
		}
	};
	
	void Exam()
	{
		Complex a(1.0f, 2.0f);
		Complex b(3.0f, -2.0f);
		/*
		Complex c(0.0f, 0.0f); //c = 시에 복사생성자가 호출됨
		c = a * b + a / b + a + b; //대입연산자가 호출됨
		*/
		a += b;
		std::cout << a << '\n';
		std::cout << b << '\n';
	}
}
