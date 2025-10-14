//#pragma once
//#include "CommonHeader.h"
//
//struct ComponentChunk
//{
//	void* data = nullptr;
//	size_t size = 0;
//	size_t capacity = 0;
//	size_t count = 0;
//	std::function<void(void*)> destructor;
//	/*
//	std::vector, string 같은 단순데이터가 아닌 동적할당을 별도에서 수행하는 내부형컨테이너 std기반의 라이브러리를 
//	Component요소로 사용시 이를 소멸자를 명시적으로 호출해 올바른 정리가 필요하다
//	*/
//	std::function<void(void*)> destruct_only;
//	std::function<void(void*, void*)> swap_and_pop;
//};
//
//class Archetype
//{
//public:
//	Archetype();
//	~Archetype();
//	Archetype(const Archetype&) = delete;
//	Archetype& operator=(const Archetype&) = delete;
//	Archetype(Archetype&&) = delete;
//	Archetype& operator=(Archetype&&) = delete;
//public:
//	template<typename T>
//	void RegisterComponent();
//	template<typename T>
//	void AddComponent(T&& component);
//	template<typename T>
//	T* GetComponents();
//	void DeleteComponent(size_t chunkIdx);
//
//private:
//	std::unordered_map<std::type_index, ComponentChunk> m_Components;
//};
//
//
//inline Archetype::Archetype()
//{
//
//}
//inline Archetype::~Archetype()
//{
//	for (auto& iter : m_Components)
//		iter.second.destructor(iter.second.data);
//}
//
//template<typename T>
//inline void Archetype::RegisterComponent()
//{
//	std::type_index type = typeid(T);
//	_ASEERTION_NULCHK(m_Components.find(type) == m_Components.end(), "Already Chunk exist");
//	m_Components[type] = ComponentChunk();
//	ComponentChunk& chunk = m_Components[type];
//	size_t size = sizeof(T);
//	if (chunk.data == nullptr)
//	{
//		chunk.capacity = 16;
//		chunk.data = new T[chunk.capacity];
//		chunk.size = size;
//		chunk.count = 0;
//	}
//	chunk.destructor = [](void* data)
//		{
//			delete[] static_cast<T*>(data);
//		};
//
//	chunk.destruct_only = [](void* ptr)
//		{
//			// **힙 메모리 릭 방지:** 객체의 소멸자만 명시적으로 호출합니다.
//			static_cast<T*>(ptr)->~T();
//		};
//
//	chunk.swap_and_pop = [](void* dest, void* src)
//		{
//			T* dest_ptr = static_cast<T*>(dest);
//			T* src_ptr = static_cast<T*>(src);
//
//			// 1. dest 위치에 있던 기존 객체 소멸 (삭제될 객체의 자원 해제)
//			dest_ptr->~T();
//
//			// 2. src 객체를 dest 위치로 이동 구성 (Placement New & Move)
//			// std::move를 통해 자원 소유권을 dest로 이전한다, 연속된 배열에대한 새로운 객체위치를 선언해준다 
//			new (dest_ptr) T(std::move(*src_ptr));
//
//			// 3. src 위치에 있던 객체 소멸 (moved-from 상태 정리)
//			src_ptr->~T();
//			// 만약 T가 Trivial하다면, 컴파일러는 이 모든 과정을 memcpy 수준으로 최적화합니다.
//		};
//}
//
//template<typename T>
//inline void Archetype::AddComponent(T&& component)
//{
//	std::type_index type = typeid(T);
//	_ASEERTION_NULCHK(m_Components.find(type) != m_Components.end(), "Component Chunk not exist");
//	ComponentChunk& chunk = m_Components[type];
//	size_t size = sizeof(T);
//	if (chunk.count + 1 >= chunk.capacity)
//	{
//		chunk.capacity *= 2;
//		T* newChunk = new T[chunk.capacity];
//		std::memcpy(newChunk, chunk.data, chunk.size);
//		delete[] static_cast<T*>(chunk.data);
//		chunk.data = newChunk;
//	}
//
//	T* data_ptr = static_cast<T*>(chunk.data);
//	data_ptr[chunk.count] = std::forward<T>(component);
//	chunk.count++;
//}
//
//template<typename T>
//inline T* Archetype::GetComponents()
//{
//	std::type_index type = typeid(T);
//	_ASEERTION_NULCHK(m_Components.find(type) != m_Components.end(), "Component nullptr");
//	ComponentChunk& chunk = m_Components[type];
//	_ASEERTION_NULCHK(chunk.data, "Chunk is nullptr");
//	return static_cast<T*>(chunk.data);
//}
//
//inline void Archetype::DeleteComponent(size_t chunkIdx)
//{
//	for (const auto& iter : m_Components)
//	{
//		ComponentChunk& chunk = m_Components[iter.first];
//		_ASEERTION_NULCHK(chunkIdx < chunk.count, "out of bound");
//
//		// 모든 컴포넌트의 데이터는 T 타입의 배열 형태로 저장되어 있다고 가정
//		BYTE* data_ptr = static_cast<BYTE*>(chunk.data);
//		size_t size = chunk.size;
//		size_t last_idx = chunk.count - 1;
//		if (chunkIdx >= last_idx)
//		{
//			//destroy only, 마지막요소만 제거
//			void* ptr = data_ptr + (chunkIdx * size);
//			chunk.destruct_only(ptr);
//		}
//		else
//		{
//			//swap and pop, 요소를 제거하고 기존요소를 집어넣는다
//			void* dest_ptr = data_ptr + (chunkIdx * size);
//			void* src_ptr = data_ptr + (last_idx * size);
//			chunk.swap_and_pop(dest_ptr, src_ptr);
//		}
//		chunk.count--;
//	}
//}

#pragma once
#include "CommonHeader.h"
class ComponentChunk
{
public:
	virtual ~ComponentChunk() = default;
	virtual void SwapAndPop(size_t chunkIdx) = 0;
	virtual size_t GetCount() const = 0;
};
template<typename T>
class ChunkData : public ComponentChunk
{
public:
	void SwapAndPop(size_t chunkIdx) override
	{
		_ASEERTION_NULCHK(GetCount() && chunkIdx < GetCount() , "idx out of bound");
		if (chunkIdx < GetCount() - 1)
			m_data[chunkIdx] = std::move(m_data.back());
		m_data.pop_back();
	}
	size_t GetCount() const override
	{
		return m_data.size();
	}
public:
	std::vector<T> m_data;
};

class Archetype
{
public:
	Archetype();
	~Archetype();
	Archetype(const Archetype&) = delete;
	Archetype& operator=(const Archetype&) = delete;
	Archetype(Archetype&&) = delete;
	Archetype& operator=(Archetype&&) = delete;
public:
	template<typename T>
	void RegisterComponent();
	template<typename T>
	void AddComponent(T&& component);
	template<typename T>
	T* GetComponents();
	void DeleteComponent(size_t chunkIdx);

private:
	std::unordered_map<std::type_index, ComponentChunk*> m_Components;
};


inline Archetype::Archetype()
{
}

inline Archetype::~Archetype()
{
	for (auto iter = m_Components.begin(); iter != m_Components.end();)
	{
		delete iter->second;
		iter = m_Components.erase(iter);
	}
}

template<typename T>
inline void Archetype::RegisterComponent()
{
	std::type_index type = typeid(T);
	_ASEERTION_NULCHK(m_Components.find(type) == m_Components.end(), "Already Chunk exist");
	ChunkData<T>* Chunk = new ChunkData<T>();
	Chunk->m_data.reserve(32);
	m_Components[type] = Chunk;
}

template<typename T>
inline void Archetype::AddComponent(T&& component)
{
	std::type_index type = typeid(T);
	_ASEERTION_NULCHK(m_Components.find(type) != m_Components.end(), "Component Chunk not exist");
	ChunkData<T>* chunk = static_cast<ChunkData<T>*>(m_Components[type]);
	chunk->m_data.push_back(std::forward<T>(component));
}

template<typename T>
inline T* Archetype::GetComponents()
{
	std::type_index type = typeid(T);
	_ASEERTION_NULCHK(m_Components.find(type) != m_Components.end(), "Component nullptr");
	ChunkData<T>* chunk = static_cast<ChunkData<T>*>(m_Components[type]);
	return chunk->m_data.data();
}

inline void Archetype::DeleteComponent(size_t chunkIdx)
{
	for (const auto& iter : m_Components)
		iter.second->SwapAndPop(chunkIdx);
}

