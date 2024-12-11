#pragma once
#include "std.h"
struct Animal
{
	char name[30];
	int age;
};
void CreateAnimal(Animal* animal)
{
	std::cout << "이름 : "; std::cin >> animal->name;
	std::cout << "나이 : "; std::cin >> animal->age;
}
void ShowAnimal(Animal* animal)
{
	std::cout << "이름 : " << animal->name << '\n';
	std::cout << "나이 : " << animal->age << '\n';
}
void func3()
{
	int* p = new int; //int형포인터 p에 동적할당
	*p = 10;
	std::cout << *p << '\n';
	delete p; //힙영역에 동적할당된 int형 사이즈를 가리키는 p가 할당하는 주소를 해제한다

	int arr_size;
	std::cin >> arr_size;
	int* arr = new int[arr_size]; //동적배열생성(힙영역)
	for (int i = 0; i < arr_size; i++) std::cin >> arr[i];
	for (int i = 0; i < arr_size; i++) std::cout << arr[i] << ' ';
	std::cout << '\n';
	delete[] arr; //해제, new[]는 배열 전체를 위한 단일 메모리 블록을 생성하는데 delete는 단일객체를 해제하므로 UB를 야기시킨다
}
//확장이 필요한 기본적인 객체생성코드
void funcAnimal() 
{
	Animal* list[10];
	int idx = 0;
	int input = 0;
	while (input != -1)
	{
		std::cout << "1.추가" << '\n';
		std::cout << "2.상태" << '\n';
		std::cin >> input;
		switch (input)
		{
		case 1 :
		{
			Animal* newAnimal = new Animal;
			CreateAnimal(newAnimal);
			list[idx++] = newAnimal;
		}break;
		case 2:
		{
			for (int i = 0; i < idx; i++)
				ShowAnimal(list[i]);
		}break;
		default:
		{
			std::cout << "종료";
		}break;
		}
	}
	for (int i = 0; i < idx; i++)
		delete list[i];
}