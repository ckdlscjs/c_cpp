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
#define CHUNK_BYTE_LIMIT 16384		//16KB

class ComponentChunk
{
public:
	virtual ~ComponentChunk() = default;
	virtual void Swap(size_t srcCol, ComponentChunk* destChunk, size_t destCol) = 0;
	virtual void SwapAndPop(size_t chunkIdx, ComponentChunk* srcChunk) = 0;
	virtual void AddChunk() = 0;
	virtual size_t GetCount() const = 0;
};
template<typename T>
class ChunkData : public ComponentChunk
{
public:
	ChunkData(size_t chunkCapacity) : m_capacity(chunkCapacity)
	{
		m_data.reserve(m_capacity);
	}
	void Swap(size_t srcCol, ComponentChunk* destChunk, size_t destCol) override
	{
		_ASEERTION_NULCHK(GetCount() && srcCol < GetCount(), "idx out of bound");
		_ASEERTION_NULCHK(destChunk->GetCount() && destCol < destChunk->GetCount(), "idx out of bound");
		std::vector<T>& pChunks = static_cast<ChunkData<T>*>(destChunk)->m_data;
		std::swap(m_data[srcCol], pChunks[destCol]);
	}
	void SwapAndPop(size_t chunkIdx, ComponentChunk* srcChunk) override
	{
		_ASEERTION_NULCHK(GetCount() && chunkIdx < GetCount(), "idx out of bound");
		std::vector<T>& pChunks = static_cast<ChunkData<T>*>(srcChunk)->m_data;
		if(chunkIdx < pChunks.size()-1)
			m_data[chunkIdx] = std::move(pChunks.back());
		pChunks.pop_back();
	}
	void AddChunk() override
	{
		_ASEERTION_NULCHK(GetCount() < m_capacity, "Size full");
		m_data.emplace_back();
	}
	size_t GetCount() const override
	{
		return m_data.size();
	}
public:
	std::vector<T> m_data;
	size_t m_capacity;
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
	bool NeedNewChunk();
	template<typename T>
	void CreateNewChunk();
	void ReserveIndexes(const size_t lookupIdx, size_t& out_idxRow, size_t& out_idxCol);
	template<typename T>
	void AddComponent(T&& component);
	template<typename T>
	std::vector<T>& GetComponents(size_t idxRow);
	std::pair<size_t, size_t> SwapChunkData(size_t srcRow, size_t srcCol, size_t destRow, size_t destCol);
	size_t DeleteComponent(size_t idxRow, size_t idxCol);
	size_t GetCapacity_Chunks() const;
	size_t GetCount_Chunks() const;
	size_t GetCount_Chunk(size_t row) const;
	size_t GetAllChunkCount() const;

	size_t m_transfer_row = 0;
	size_t m_transfer_col = 0;
private:
	size_t m_ChunksCapacity;
	std::unordered_map<std::type_index, std::vector<ComponentChunk*>> m_Components;
	std::vector<size_t> m_LookupIdxs;
};


inline Archetype::Archetype()
{
}

inline Archetype::~Archetype()
{
	for (auto& pair : m_Components)
	{
		for (ComponentChunk* chunk : pair.second)
			delete chunk;
	}
}

template<typename T>
inline void Archetype::RegisterComponent()
{
	std::type_index type = typeid(T);
	_ASEERTION_NULCHK(m_Components.find(type) == m_Components.end(), "Already Chunk exist");
	if (m_Components.empty())
		m_ChunksCapacity = std::max((size_t)1, CHUNK_BYTE_LIMIT / sizeof(T));
	m_Components[type] = std::vector<ComponentChunk*>();
	m_Components[type].push_back(new ChunkData<T>(m_ChunksCapacity));
}

inline bool Archetype::NeedNewChunk()
{
	ComponentChunk* lastChunk = m_Components.begin()->second.back();
	return lastChunk->GetCount() >= m_ChunksCapacity;
}

inline void Archetype::ReserveIndexes(const size_t lookupIdx, size_t& out_idxRow, size_t& out_idxCol)
{
	_ASEERTION_NULCHK(m_Components.size(), "Component Chunk not exist");

	m_LookupIdxs.push_back(lookupIdx);

	for (auto& iter : m_Components)
		iter.second.back()->AddChunk();
	auto& chunkList = m_Components.begin()->second;
	ComponentChunk* lastChunk = chunkList.back();
	
	out_idxRow = chunkList.size() - 1;
	out_idxCol = lastChunk->GetCount() - 1;
}

template<typename T>
inline void Archetype::CreateNewChunk()
{
	std::type_index type = typeid(T);
	_ASEERTION_NULCHK(m_Components.find(type) != m_Components.end(), "Component Chunk not exist");
	m_Components[type].push_back(new ChunkData<T>(m_ChunksCapacity));
}

template<typename T>
inline void Archetype::AddComponent(T&& component)
{
	std::type_index type = typeid(T);
	_ASEERTION_NULCHK(m_Components.find(type) != m_Components.end(), "Component Chunk not exist");
	ComponentChunk* lastChunk = m_Components[type].back();
	ChunkData<T>* dataChunk = static_cast<ChunkData<T>*>(lastChunk);
	dataChunk->m_data.back() = std::forward<T>(component);
}


template<typename T>
inline std::vector<T>& Archetype::GetComponents(size_t idxRow)
{
	std::type_index type = typeid(T);
	_ASEERTION_NULCHK(m_Components.find(type) != m_Components.end(), "Component nullptr");
	_ASEERTION_NULCHK(idxRow < m_Components[type].size(), "Chunk row Out of Bound");

	ChunkData<T>* chunk = static_cast<ChunkData<T>*>(m_Components[type][idxRow]);
	return chunk->m_data;
}

inline std::pair<size_t, size_t> Archetype::SwapChunkData(size_t srcRow, size_t srcCol, size_t destRow, size_t destCol)
{
	const auto& chunks = m_Components.begin()->second;
	_ASEERTION_NULCHK(srcRow < chunks.size(), "Chunk row Out of Bound");
	_ASEERTION_NULCHK(!m_LookupIdxs.empty(), "Entity Lookup empty");

	//swap
	for (auto& iter : m_Components)
		iter.second[srcRow]->Swap(srcCol, iter.second[destRow], destCol);
	
	//Lookup을 교체, 리턴
	size_t srcIdx = srcRow * m_ChunksCapacity + srcCol;
	size_t destIdx = destRow * m_ChunksCapacity + destCol;
	size_t lookupSrc = m_LookupIdxs[srcIdx];
	size_t lookupDest = m_LookupIdxs[destIdx];
	std::swap(m_LookupIdxs[srcIdx], m_LookupIdxs[destIdx]);
	return { lookupSrc, lookupDest };
}

inline size_t Archetype::DeleteComponent(size_t idxRow, size_t idxCol)
{
	const auto& chunks = m_Components.begin()->second;
	_ASEERTION_NULCHK(idxRow < chunks.size(), "Chunk row Out of Bound");
	_ASEERTION_NULCHK(!m_LookupIdxs.empty(), "Entity Lookup empty");

	//Archetype의 요소들중 끝 값을 앞으로 swap하고 맨뒷값 삭제
	for (auto& iter : m_Components)
		iter.second[idxRow]->SwapAndPop(idxCol, iter.second.back());

	//청크가 가진 삭제위치의 Lookup값을 맨 끝값으로 바꾼다
	size_t movedIdx = m_LookupIdxs.back();
	size_t deletedIdx = m_ChunksCapacity * idxRow + idxCol;
	if (deletedIdx < m_LookupIdxs.size() - 1)
		m_LookupIdxs[deletedIdx] = movedIdx;
	m_LookupIdxs.pop_back();

	//끝의 청크가 요소가 없을때 삭제한다
	if (chunks[idxRow]->GetCount() <= 0)
	{
		for (auto& pair : m_Components)
		{
			delete pair.second[idxRow];
			pair.second.erase(pair.second.begin() + idxRow);
		}
	}
	
	//이동된Lookup값을 리턴
	return movedIdx;
}

inline size_t Archetype::GetCount_Chunks() const
{
	const auto& chunks = m_Components.begin()->second;
	return chunks.size();
}

inline size_t Archetype::GetCapacity_Chunks() const
{
	return m_ChunksCapacity;
}

inline size_t Archetype::GetCount_Chunk(size_t row) const
{
	const auto& chunks = m_Components.begin()->second;
	return chunks[row]->GetCount();
}

inline size_t Archetype::GetAllChunkCount() const
{
	const auto& chunks = m_Components.begin()->second;
	const auto& lastchunk = chunks.back();
	return (GetCount_Chunks() - 1) * m_ChunksCapacity + lastchunk->GetCount();
}