#pragma once

//WarningC4316 ����Ʈ ó����, 16����Ʈ�� �ʿ��� XMMatrix��쿡�ʿ�
template<size_t T> //��Ÿ�Ա��(non-type)���ø�, �������(size_t)�� �Ű������� �ްڴ�
class AlignedAllocationPolicy
{
public:
	//����Ʈ�е� new������
	static void* operator new(size_t size)
	{
		return _aligned_malloc(size, T);
	}
	//����Ʈ�е� delete������
	static void operator delete(void* memory)
	{
		_aligned_free(memory);
	}
};