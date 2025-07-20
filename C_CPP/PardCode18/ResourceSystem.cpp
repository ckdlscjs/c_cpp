#include "ResourceSystem.h"
#include "Resource.h"
//// FileSystem
//#if __cplusplus <= 201402L 
//#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
//#include <experimental/filesystem>
//#endif
//
//#if __cplusplus >= 201703L
//#include <filesystem>
//#endif

ResourceSystem::ResourceSystem()
{
}

ResourceSystem::~ResourceSystem()
{
}

size_t ResourceSystem::HashFilePath(const std::wstring& path)
{
	// FNV-1a (64비트) 구현 예시
	size_t hash = 14695981039346656037ULL; // FNV_PRIME_64
	for (wchar_t c : path) {
		hash ^= static_cast<size_t>(c);
		hash *= 1099511628211ULL; // FNV_OFFSET_64
	}
	return hash;
}

void ResourceSystem::Init()
{

}

void ResourceSystem::Release()
{
	for (auto iter = m_Resources.begin(); iter != m_Resources.end();)
	{
		delete iter->second;
		iter = m_Resources.erase(iter);
	}
}