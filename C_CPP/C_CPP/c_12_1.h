#pragma once
#include "std.h"
//�����ʹ� �޸𸮻� ��ġ�� Ư���� �������� �����ּҰ��� �����ϴ� �����̴�
//&�� ����(unary)�����ڷ� �̿��Ͽ� �ǿ������� �ּҰ��� �ҷ����� �뵵�� ����Ѵ�
//�������� Ÿ���� �ش� �����ּҷκ��� �����Ʈ�� ���� �о���°��� ǥ���Ѵ�
void func12_1_1()
{
	int a = 2;
	std::cout << &a << '\n'; //a�� �ּҰ��� 16���� ���·� ����Ѵ�
	int* p = &a;
	std::cout << p << ", " << &a << '\n'; //�� ���� �������� ���δ�,p�� a���ּҸ� ����Ų��, &a�� a���ּҸ� ����Ų��
	std::cout << *p << ", " << a << '\n'; //�����ּҸ� ����Ű�� �� ���� ���� ���� ���δ�

	*p = 3;
	std::cout << *p << ", " << a << '\n'; //�����ּҸ� ����Ű�� �� ���� 3���� �Ѵ� ������� Ȯ�� �Ѵ�
	return;
}