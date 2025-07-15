#include <iostream>

using namespace std;

/*

- CRTP�� ���� �Լ� ȣ���� �߻����� �ʱ� ������ ��Ÿ�� ������尡 ����.

- ������ Ÿ�ӿ� �������� �����ϰ�, ���� �Լ� ȣ�� ��� �����Ϸ� �ڵ带 �����ϹǷ� ȣ��Ǵ� �Լ��� �̸� �����ȴ�.

  (��Ÿ�ӿ� ���������� ���� ������� ����)

- ���� �Լ� ���̺�(vtable)�� �ʿ����� �ʾƼ� ���α׷� �ڵ� ũ�Ⱑ �����Ѵ�.
*/
template <typename T>
class Base
{
public:
	void interface() { static_cast<T*>(this)->implementation(); }	//������ �ʼ��� �ؾ��ϴ� ���������Լ����� ���°� �ȴ�
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