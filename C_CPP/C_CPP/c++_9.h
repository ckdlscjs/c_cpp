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

namespace Variadic
{
	//c++에서 템플릿을 이용해 임의의갯수를 받는 함수를 구현 할 수 있다

	//마지막호출시 인자가 한개 뿐 이므로 (5) 로 받아진다
	template<typename T>
	void Print(T t)
	{
		std::cout << t << '\n';
	}
	//첫 호출시 (1, (2,3,4,5))
	//다음 (2, (3,4,5))...재귀적호출
	template<typename T, typename... Types>
	void Print(T t, Types... args)
	{
		std::cout << t << ", ";
		Print(args...);
	}
	void Exam1()
	{
		Print(1, 2, 3, 4, 5); //가변인자를 통해 받는다
	}

	//임의의 갯수의 문자열을 합치는함수
	//std::string str = s1 + s2 + s3.. 는 s1에 s2를 붙일때 메모리할당, 그후 s3를붙일때도 메모리할당, 메모리의 할당/해제는 비용이 크기때문에 한번의 할당으로 처리하는게 효율적이다
	//인자를 합칠 여러개의 문자열을 가변인자를 통해서 받고 메모리할당을 한번으로 해결 할 수 있을것 이므로 효율적으로 해결가능하다
	template <typename String>
	std::string Strcat(const String& str)
	{
		return std::string(str);
	}
	//할당자체는 +연산자를 이용해서 추가로 임시객체를만들고 하기때문에 할당횟수자체는 같으나 const char* 또한 받는게 가능하다
	template<typename String, typename... Strings>
	std::string Strcat(const String& str, Strings... args)
	{
		return std::string(str) + Strcat(args...);
	}
	void Exam2()
	{
		std::cout << Strcat("This", " ", "is", " ", std::string("a"), " ", std::string("sentence"));
	}

	//재할당되는 과정을 한번으로 해결하기위해 미리 크기를 다 계산해서 붙여준다

	//크기를 계산하는 함수, 가변인자템플릿을 이용해 재귀적으로 계산
	size_t GetStringSize(const char* _str) { return std::strlen(_str); }
	size_t GetStringSize(const std::string& _str) { return _str.size(); }
	template<typename String, typename... Strings>
	size_t GetStringSize(String _str, Strings... strs)
	{
		return GetStringSize(_str) + GetStringSize(strs...);
	}

	//목적string에 가변인자로 받는 append_str을 하나씩 붙혀준다, dest_str은 strcat에서 reserve통해 크기를 늘려놓았음.
	void AppendString(std::string& dest_str) { return; }
	template<typename String, typename... Strings>
	void AppendString(std::string& dest_str, const String& append_str, Strings... strs)
	{
		dest_str.append(append_str);
		AppendString(dest_str, strs...);
	}

	template<typename String, typename... Strings>
	std::string Strcat2(String _str, Strings... strs)
	{
		//크기를 미리 체크해놓고 사이즈를 예약
		std::string concat_str;
		concat_str.reserve(GetStringSize(_str, strs...));
		
		//늘려놓은 새 변수에 첫 문장을 받고 해당문장부터 이어붙힌다
		concat_str = _str;
		AppendString(concat_str, strs...);
		return concat_str;
	}

	void Exam3()
	{
		std::cout << Strcat2(std::string("this"), " ", "is", " ", std::string("a"), " ", std::string("sentence"));
	}
	
	//기저조건을위한 인수0일때의 함수
	int SumAll() { return 0; }
	template<typename... Ints>
	int SumAll(int num, Ints... nums)
	{
		return num + SumAll(nums...);
	}

	template<typename... Ints>
	double average(Ints... nums)
	{
		return static_cast<double>(SumAll(nums...)) / sizeof...(nums); //sizeof... 템플릿 가변인자를 이용한 변수로 갯수를 센다
	}
	
	void Exam4()
	{
		std::cout << average(1, 4, 2, 3, 10);
	}

	//fold식 c++17이상, 
	/*
	template<typename... Ints>
	int SumAll(Ints... nums)
	{
		return (... + nums);
	}

	void Exam5()
	{
		std::cout << SumAll(1, 4, 2, 3, 10);
	}
	*/
}

namespace ArrayTest
{
	template<typename T, unsigned int N>
	class Array
	{
	private:
		T data[N]; //템플릿으로 인해 N에들어가는 값이 T와같이 컴파일타임에 코드가생성된다, 즉 Array<int, 5> Array<int, 3>은 서로 다른클래스로 본다
	public:
		Array(T (&arr)[N]) //T (&temp)[N] = arr, 즉 int arr[N]과 동치가된다.
		{
			for (int i = 0; i < N; i++)
				data[i] = arr[i];
		}

		T* GetArray() { return data; }
		unsigned int Size() { return N; }
		void PrintAll()
		{
			for (int i = 0; i < N; i++)
				std::cout << data[i] << ", ";
			std::cout << '\n';
		}
	};
	void Exam()
	{
		int arr[] = { 1, 2, 3 };
		Array<int, 3> Arr(arr);
		Arr.PrintAll();
	}
}

namespace Int
{
	template <int N>
	struct Int 
	{
		static const int num = N; //컴파일타임에 값을 정한다, static 및 const
	};

	template <typename T, typename U>
	struct add
	{
		typedef Int<T::num + U::num> result;
	};

	void Exam()
	{
		typedef Int<2> two;
		typedef Int<5> five;
		std::cout << add<two, five>::result::num; //이미 프로그램 실행전에 컴파일시에 7로 치환되어있다, 즉 컴파일타임에 이미 연산을 끝내버리는 코드자체로 프로그래밍 하는것을 메타프로그래밍 이라한다
	}
}

namespace TMP
{
	template<int N>
	struct Factorial
	{
		static const int result = N * Factorial<N - 1>::result;
	};
	template<>
	struct Factorial<1>
	{
		static const int result = 1;
	};
	void Exam()
	{
		std::cout << Factorial<5>::result; //이미 컴파일하면서 값이 정해져있다, 다만 런타임에서 값을 임의로 변경 할 수는 없다, 이미 컴파일타임에 끝나기때문, 컴파일타임상수
	}

	//재귀를 이용한 유클리드 호제법
	int gcd(int a, int b)
	{
		if (b == 0) return a;
		return gcd(b, a % b);
	}

	//이를 메타프로그래밍으로 구현
	template<int X, int Y>
	struct GCD
	{
		static const int ret = GCD<Y, X%Y>::ret;
	};

	template<int X>
	struct GCD<X, 0> //템플릿특수화, 원하는 템플릿값만 남긴다
	{
		static const int ret = X;
	};
	void Exam2()
	{
		std::cout << GCD<36, 24>::ret;
	}
	
	//분모, 분자를 받아 표현하는 Ratio클래스
	//<N, D>로 컴파일타임에 표기한 값들을 n, d로 정적상수화
	template<int N, int D = 1>
	struct Ratio
	{
		static const int num = N;
		static const int den = D;
	};
	//Ratio를 이용하여 덧셈을 수행하는 템플릿
	template<typename R1, typename R2>
	struct _Ratio_Add
	{
		using type = Ratio<R1::num* R2::den + R2::num * R1::den, R1::den* R2::den>; //정적상수화된 R1, R2의 NUM DEN값을 기반으로 새로운 Ratio클래스타입 +연산된 클래스를 생성, 편의성을위해 using type = 로 지정
	};
	//표현의 통일성을위해 _Ratio_Add를 상속받아사용한다
	template<typename R1, typename R2>
	struct Ratio_Add : _Ratio_Add<R1, R2>::type {}; //표현의 통일성을위해 using으로 표현한 type, 즉 Ratio클래스를 상속하여 새로운Ratio생성(_Ratio_Add의상속이아님, Ratio의상속)

	void Exam3()
	{
		using rat1 = Ratio<2, 3>;
		using rat2 = Ratio<3, 2>;
		using rat3 = Ratio_Add<rat1, rat2>;
		std::cout << rat3::num << '/' << rat3::den;
	}
}

//GCD(최대공약수, 유클리드호제법)을 이용해 유리수의 사칙연산을 수행하는 템플릿을 이용한 메타프로그래밍 코드, 사칙연산 이후 기약분수로 표현을위해 최대공약수를이용한다
namespace TMP2
{
	//이를 메타프로그래밍으로 구현
	template<int X, int Y>
	struct GCD
	{
		static const int value = GCD<Y, X% Y>::value;
	};

	template<int X>
	struct GCD<X, 0> //템플릿특수화, 원하는 템플릿값만 남긴다
	{
		static const int value = X;
	};

	//분모, 분자를 받아 표현하는 Ratio클래스
	//<N, D>로 컴파일타임에 표기한 값들을 n, d로 정적상수화
	template<int N, int D = 1>
	struct Ratio
	{
	private:
		static const int _gcd = GCD<N, D>::value; //유리수의 사칙연산이후 기약분수의 표현을 위한 최대공약수
	public:
		static const int num = N / _gcd; //기약분수표현을위해나눗셈
		static const int den = D / _gcd; //기약분수표현을위해나눗셈
	};
	//Ratio를 이용하여 덧셈을 수행하는 템플릿
	template<typename R1, typename R2>
	struct _Ratio_Add
	{
		using type = Ratio<R1::num* R2::den + R2::num * R1::den, R1::den* R2::den>; //정적상수화된 R1, R2의 NUM DEN값을 기반으로 새로운 Ratio클래스타입 +연산된 클래스를 생성, 편의성을위해 using type = 로 지정
	};
	//표현의 통일성을위해 _Ratio_Add를 상속받아사용한다
	template<typename R1, typename R2>
	struct Ratio_Add : _Ratio_Add<R1, R2>::type {}; //표현의 통일성을위해 using으로 표현한 type, 즉 Ratio클래스를 상속하여 새로운Ratio생성(_Ratio_Add의상속이아님, Ratio의상속)
	
	//뺄셈
	template<typename R1, typename R2>
	struct _Ratio_Subtract
	{
		using type = Ratio<R1::num* R2::den - R2::num * R1::den, R1::den* R2::den>;
	};
	template<typename R1, typename R2>
	struct Ratio_Subtract : _Ratio_Subtract<R1, R2>::type {};

	//곱셈
	template<typename R1, typename R2>
	struct _Ratio_Multiply
	{
		using type = Ratio<R1::num * R2::num, R1::den* R2::den>;
	};
	template<typename R1, typename R2>
	struct Ratio_Multiply : _Ratio_Multiply<R1, R2>::type {};

	//나눗셈
	template<typename R1, typename R2>
	struct _Ratio_Divide
	{
		using type = Ratio<R1::num * R2::den, R1::den * R2::num>;
	};
	template<typename R1, typename R2>
	struct Ratio_Divide : _Ratio_Divide<R1, R2>::type {};

	void Exam()
	{
		using r1 = Ratio<2, 3>;
		using r2 = Ratio<3, 2>;

		using r3 = Ratio_Add<r1, r2>;
		std::cout << "2/3 + 3/2 = " << r3::num << " / " << r3::den << std::endl;

		using r4 = Ratio_Multiply<r1, r3>;
		std::cout << "13 / 6 * 2 /3 = " << r4::num << " / " << r4::den << std::endl;
	}
}