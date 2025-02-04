#pragma once
#include "std.h"
namespace ch9
{
	template<typename T>
	class Vector
	{
	private:
		T* data;
		int capacity;
		int length;
		
	public:
		Vector(int _size = 1) : data(new T[_size]), capacity(_size), lenght(0) {}
		void Push_Back(T t) 
		{

		}
	};
}