#pragma once
#include "std.h"
void changeVal(int* p)
{
	*p = 3;
}
void changeVal(int& p)
{
	p = 7;
}
int& retVal(int& ref)
{
	return ref;
}
//참조자로 상수값리터럴을 리턴
const int& retConstval() 
{
	int constVal = 456;
	return constVal;
}
void func2_1()
{
	int num = 5; //int형변수 num
	std::cout << num << '\n';

	changeVal(&num); //int형변수 num을 int* p에 주소를 넘겨서 changeVal함수를 이용해 값을 변경함
	std::cout << num << '\n'; //num의값이 변경됨을 확인

	int& ref_num = num; //int형 참조자 ref_num에 num을 지정
	ref_num = 6;
	std::cout << num << '\n';

	changeVal(ref_num); //int형 참조자 p에 ref_num(num)을 지정
	std::cout << num << '\n';

	const int& ref = 123;
	std::cout << ref << '\n'; //상수리터럴 ref

	int arr[] = { 1, 4, 3 };
	int& refarr = *arr;
	int(&refarr2)[3] = arr;
	std::cout << &refarr << ' ' << refarr2+1 << ' ' << arr+1 << '\n'; //arr[0]의주소, 배열arr의 주소+1, 배열arr의주소+1
	std::cout << refarr2 + 1 << ' ' << &arr + 1 << '\n'; //arr의주소+1, 배열arr전체의주소+1(+4*3)
	std::cout << refarr + 1 << ' ' << refarr2[1] << ' ' << *(arr + 1) << '\n'; //배열arr[0]의값+1, arr[1], arr[1]

	int& reffunc = retVal(arr[0]); //참조자로 arr[0]을 건네줌, reffunc에 반환되는 참조자형 변수로 인해 reffunc참조자가 arr[0]을 가리킴
	reffunc = 5; //arr[0]의값을변경
	std::cout << arr[0] << '\n';

	const int& constVal = retConstval(); //상수형 참조자 리터럴을 상수형 참조자로 받음, 수정불가
	std::cout << constVal;
}