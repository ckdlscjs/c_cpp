#pragma once
#include "std.h"
void func4Print(int x)
{
	std::cout << "int : " << x << '\n';
}
void func4Print(char x)
{
	std::cout << "char : " << x << '\n';
}
void func4Print(double x)
{
	std::cout << "double : " << x << '\n';
}

class Func4Check
{
public:
	Func4Check()
	{
		std::cout << "Func4생성자" << '\n';
	}
};
namespace funcPointGeo
{
	class Point {
		int x, y;

	public:
		Point(int pos_x, int pos_y);
		Point(const Point& point);
	};
	funcPointGeo::Point::Point(int pos_x, int pos_y)
	{
		x = pos_x;
		y = pos_y;
	}
	funcPointGeo::Point::Point(const Point& point)
	{
		x = point.x;
		y = point.y;
	}


	class Geometry {
	public:
		Geometry() {
			num_points = 0;
		}

		void AddPoint(const Point& point) {
			point_array[num_points++] = new Point(point);
		}

		// 모든 점들 간의 거리를 출력하는 함수 입니다.
		void PrintDistance();

		// 모든 점들을 잇는 직선들 간의 교점의 수를 출력해주는 함수 입니다.
		// 참고적으로 임의의 두 점을 잇는 직선의 방정식을 f(x,y) = ax+by+c = 0
		// 이라고 할 때 임의의 다른 두 점 (x1, y1) 과 (x2, y2) 가 f(x,y)=0 을 기준으로
		// 서로 다른 부분에 있을 조건은 f(x1, y1) * f(x2, y2) <= 0 이면 됩니다.
		void PrintNumMeets();

	private:
		// 점 100 개를 보관하는 배열.
		Point* point_array[100];
		int num_points;
	};
}

void func4()
{
	int a = 1;
	char b = 'c';
	double c = 3.2f;
	func4Print(a);
	func4Print(b);
	func4Print(c);
	/*
	* 오버로딩의 형변환 과정
	* 1.적합한 인자가 맞는 함수를 찾는다
	* 
	* 2.정확히 일치하는 타입이 없을경우 형변환을 통해 일치하는 함수를 찾는다
	* 2_1. char unsigned char, short 는 int로변환된다
	* 2_2. unsigned short 는 int의 크기에 따라 int 혹은 unsigned int로 변환된다
	* 2_3. float는 double로 변환된다
	* 2_4. Enum은 int로 변환된다
	* 
	* 3.해당사항이 없다면 추가로 좀더 포괄적인 형변환을 통해 일치하는 함수를 찾는다
	* 3_1. 임의의숫자(Numeric)타입은 다른 숫자타입으로 변환된다 (ex. float -> int)
	* 3_2. Enum또한 임의의 숫자 타입으로 변환된다 (ex. Enum -> double)
	* 3_3. 0은 포인터타입<->숫자타입 양측을 오가며 체크
	* 3_4. 포인터는 void포인터로 변환된다
	* 
	* 4.직접 정의한 타입변환으로 일치하는것을 찾는다.
	* 
	* 위의과정들을 통해서 일치하는 함수를 찾을 수 없거나 같은단계에서 두개이상 겹치는 경우 모호(ambiguous)에러를 발생시킨다
	*/

	Func4Check func; //생성자호출, ()->함수호출이되버림, 모호한구문을 함수선언으로 우석 해석하는 좌측우선파싱(left-to-right parsing)때문, 함수선언(정의)가되어버림
	Func4Check func2 = Func4Check();
}

