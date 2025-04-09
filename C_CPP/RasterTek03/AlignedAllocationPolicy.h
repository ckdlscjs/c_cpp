#pragma once

//WarningC4316 바이트 처리용, 16바이트로 필요한 XMMatrix경우에필요
template<size_t T> //비타입기반(non-type)템플릿, 정수상수(size_t)를 매개변수로 받겠다
class AlignedAllocationPolicy
{
public:
	//바이트패딩 new재정의
	static void* operator new(size_t size)
	{
		return _aligned_malloc(size, T);
	}
	//바이트패딩 delete재정의
	static void operator delete(void* memory)
	{
		_aligned_free(memory);
	}
};