#include <iostream>

using namespace std;

/*

- CRTP는 가상 함수 호출이 발생하지 않기 때문에 런타임 오버헤드가 없다.

- 컴파일 타임에 다형성을 제공하고, 가상 함수 호출 대신 컴파일러 코드를 생성하므로 호출되는 함수가 미리 결정된다.

  (런타임에 다형성으로 인한 오버헤드 제거)

- 가상 함수 테이블(vtable)이 필요하지 않아서 프로그램 코드 크기가 감소한다.
*/
template <typename T>
class Base
{
public:
	void interface() { static_cast<T*>(this)->implementation(); }	//구현을 필수로 해야하는 순수가상함수같은 형태가 된다
	void commonOperation() { cout << "Common Operation in Base class" << endl; }
};

class Derived1 : public Base<Derived1>
{
public:
	void implementation() { cout << "Implementation in Derived1" << endl; }
};

class Derived2 : public Base<Derived2>
{
public:
	void implementation() { cout << "Implementation in Derived2" << endl; }
};

int main()
{
	Derived1* d1 = new Derived1();
	Derived2* d2 = new Derived2();

	d1->interface();
	d1->commonOperation();

	d2->interface();
	d2->commonOperation();

	return 0;
}