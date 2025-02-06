#pragma once
#include "std.h"
namespace ch9
{
	template<typename T> //클래스템플릿에 인자를 전달해서 실제 코드를 생성하는것, 클래스템플릿인스턴스화, 실제 객체를 생성하지 않으면 코드로 변환되지않는다.
	class Vector
	{
	private:
		T* data;
		int capacity;
		int length;
		
	public:
		Vector(int _size = 1) : data(new T[_size]), capacity(_size), length(0) {}
		~Vector()
		{
			if(data)
				delete[] data;
		}
		void Push_Back(T t) 
		{
			if (capacity <= length)
			{
				capacity *= 2;//공간2배확장
				T* temp = new T[capacity];
				for (int i = 0; i < length; i++) temp[i] = data[i];
				delete[] data;
				data = temp;
			}
			data[length] = t;
			length++;
		}

		T& operator[](int i)
		{
			return data[i];
		}
		void Remove(int i)
		{
			for (int idx = i + 1; idx < length; idx++) data[idx - 1] = data[idx];
			length--;
		}
		int Size()
		{
			return length;
		}
		void Swap(int i, int j)
		{
			T temp = data[i];
			data[i] = data[j];
			data[j] = temp;
		}
	};
	
	//특정 타입에 대해서 따로 처리를 해주는것을 템플릿특수화 라고 한다, 전달되는 템플릿 인자가 없는 경우라도 템플릿임을 암시하기위해 template<> 은필수
	//bool을 int형(32개)로 표현하여 1비트만을 표현하기위해 1개의 int형변수를 32개짜리 크기로 생각한다. 즉 32개의 비트 안에 몇번째 비트로 기록하고 이를 사용.
	class BoolRef //bool을 반환해주기위한 Proxy패턴(대리객체)
	{
	private:
		unsigned int& ref;
		int pos;
	public:
		BoolRef(unsigned int& _ref, int _pos) : ref(_ref), pos(_pos) {}
		operator bool()
		{
			return (ref & (1 << pos)) ? true : false;
		}
		BoolRef& operator=(bool tf)
		{
			if (tf)
				ref |= (1 << pos);
			else
				ref &= ~(1 << pos);
			return *this;
		}
		friend class Vector<bool>;
	};
	template<>
	class Vector<bool>
	{
	private:
		unsigned int* data;
		int capacity;
		int length;

	public:
		Vector(int _size = 1) : data(new unsigned int[_size/32 + 1]), capacity(_size/32 + 1), length(0) 
		{
			//length는 실제 들어간 비트의 갯수를 기록시킨다.
			for (int i = 0; i < capacity; i++) data[i] = 0; //사이즈/32 만큼 0으로초기화, 32개마다 1개의unsigned int
		}
		~Vector()
		{
			if (data)
				delete[] data;
		}
		void Push_Back(bool b)
		{
			if (capacity*32 <= length)
			{
				unsigned int*  temp = new unsigned int[capacity*2];
				for (int i = 0; i < capacity; i++) temp[i] = data[i];
				for (int i = capacity; i < capacity * 2; i++) temp[i] = 0;
				delete[] data;
				data = temp;
				capacity *= 2;
			}
			if (b)
				data[length / 32] |= (1 << (length % 32));
			length++;
		}
		BoolRef operator[](int i)
		{
			return BoolRef(data[i / 32], i % 32);
		}
		void Remove(int pos)
		{
			for (int i = pos + 1; i < length; i++)
			{
				int prev = i - 1;
				int cur = i;
				
				//현재비트가 1이라면
				if (data[cur / 32] & (1 << (cur % 32)))
					data[prev / 32] |= (1 << (prev % 32));
				else
					data[prev / 32] &= ~(1 << (prev % 32));
			}
			length--;
		}
		int Size() { return length; }

	};
	

	void Exam()
	{
		Vector<int> int_vec;
		int_vec.Push_Back(3);
		int_vec.Push_Back(2);

		std::cout << "-------- int vector ----------" << std::endl;
		std::cout << "첫번째 원소 : " << int_vec[0] << std::endl;
		std::cout << "두번째 원소 : " << int_vec[1] << std::endl;

		Vector<std::string> str_vec;
		str_vec.Push_Back("hello");
		str_vec.Push_Back("world");
		std::cout << "-------- std::string vector -------" << std::endl;
		std::cout << "첫번째 원소 : " << str_vec[0] << std::endl;
		std::cout << "두번째 원소 : " << str_vec[1] << std::endl;

		Vector<bool> bool_vec;
		bool_vec.Push_Back(true);
		bool_vec.Push_Back(true);
		bool_vec.Push_Back(false);
		bool_vec.Push_Back(false);
		bool_vec.Push_Back(false);
		bool_vec.Push_Back(true);
		bool_vec.Push_Back(false);
		bool_vec.Push_Back(true);
		bool_vec.Push_Back(false);
		bool_vec.Push_Back(true);
		bool_vec.Push_Back(false);
		bool_vec.Push_Back(true);
		bool_vec.Push_Back(false);
		bool_vec.Push_Back(true);
		bool_vec.Push_Back(false);
		bool_vec.Push_Back(true);
		bool_vec.Push_Back(false);

		bool_vec[0] = 0; //대리객체이용한 특정비트 On/Off변경
		bool_vec[1] = 0;
		std::cout << "-------- bool vector ---------" << std::endl;
		for (int i = 0; i < bool_vec.Size(); i++) {
			std::cout << bool_vec[i];
		}
		std::cout << std::endl;
		
	}

	//함수템플릿
	template<typename T>
	T Max(const T& a, const T& b) //const 시 상수도 받을 수 있음 rvalue
	{
		return a > b ? a : b;
	}

	//함수객체(Functor, FuntionObject)
	template <typename Cont, typename Comp>
	void bubble_sort(Cont& cont, Comp comp)
	{
		for (int i = 0; i < cont.Size(); i++)
		{
			for (int j = i + 1; j < cont.Size(); j++)
			{
				if (!comp(cont[i], cont[j]))
				{
					cont.Swap(i, j);
				}
			}
		}
	}
	struct Comp1 //오름차순정렬
	{
		bool operator()(int a, int b) { return a < b; }
	};

	//타입이 아닌 템플릿인자(nonType template arguments)
	//typename같은 템플릿인자가 아닌 정수타입, 포인터타입, enum타입, nullptr을 인자로 변수처럼 넣을 수 있다
	//인자의 디폴트값처럼 템플릿인자, 아규먼트도 기본값을 지정 할 수 있다
	template <typename T>
	struct Compare 
	{
		bool operator()(const T& a, const T& b) const { return a < b; }
	};

	template <typename T, typename Comp = Compare<T>> //기본템플릿 인자를 Compare<T>로 지정
	T Min(T a, T b) {
		Comp comp;
		if (comp(a, b)) {
			return a;
		}
		return b;
	}
	void Exam2()
	{
		int a = 10, b = 20;
		std::cout << Max(10, 20) << '\n'; //same Max<int>(a, b), 특정템플릿을 사용할때 해당 템플릿인자에 별도의함수등이 정의되어있지 않다면 컴파일에러가발생한다.

		Vector<int> int_vec;
		int_vec.Push_Back(3);
		int_vec.Push_Back(1);
		int_vec.Push_Back(2);
		int_vec.Push_Back(8);
		int_vec.Push_Back(5);
		int_vec.Push_Back(3);
		bubble_sort(int_vec, Comp1());
		for (int i = 0; i < int_vec.Size(); i++)
			std::cout << int_vec[i] << ' ';
	}
}