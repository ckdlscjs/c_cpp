#pragma once
#include "std.h"
void func12_2(void)
{
	int a = 1, b = 0;
	const int* pa = &a; //int형 포인터 pa가 가리키는 주소에 저장된 값을 pa를 통해서는 변경 할 수 없다
	a = 2;
	int* const pb = &b; //int형 포인터 pb가 가리키는 값(변수)은 변경 불가하다, 가리키는 주소에 해당하는 변수의 값에는 접근이 가능하다
	*pb = 3;
	const int* const pc = &a; //가리키는 주소값과 내부의 값 둘다 변경이 불가하다
	std::cout << *pa << *pb << *pc << '\n';
	
	int add_a;
	int* add_pa = &add_a;
	//std::cout에서 포인터의 주소값을 %p형식으로 사용하기위해 std::showbase, std::hex, reinterpret_cast를 이용한다, (자료형)은 c언어 타입 캐스팅
	std::cout << std::showbase << std::hex << reinterpret_cast<uintptr_t>(add_pa + 1) << ", " << (uintptr_t)&add_a << '\n'; //두 값은 같은 주소를 가리킨다, add_pa는 add_a의주소를 가리키고 +1하면 4바이트증감, &add_a+1은 주소+4바이트

	char add_b = 'a';
	char* add_pb = &add_b;
	std::cout << std::showbase << std::hex<< (uintptr_t)add_pb + 1 << ", " << (uintptr_t)&add_b << '\n';

	double add_c = 0.0f;
	double* add_pc = &add_c;
	std::cout << std::showbase << std::hex << (uintptr_t)add_pc + 1 << ", " << (uintptr_t)&add_c << '\n';
	
	int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	int* parr = &arr[0];
	//세가지 다 같은 주소를 가리킨다 결국 &arr[0]은 arr의시작주소인데 이는 arr과 같고 배열 arr의 변수는 arr배열의 시작주소를 가리킨다는걸 알 수 있다.
	std::cout << std::showbase << std::hex << (uintptr_t)parr << ", " << (uintptr_t)arr << ", " << (uintptr_t)&arr[0] <<'\n'; //세가지 다 같은 주소를 가리킨다
	std::cout << std::dec << arr[3] << ", " << *(arr + 3) << '\n';
}