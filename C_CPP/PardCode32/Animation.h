#pragma once
#include "Resource.h"

class Animation : public BaseResource<Animation>
{
	friend class BaseResource<Animation>;
public:
	Animation(size_t hash, const std::wstring& szFilePath);
	Animation(const Animation&) = delete;
	Animation& operator=(const Animation&) = delete;
	Animation(Animation&&) = delete;
	Animation& operator=(Animation&&) = delete;

public:
	
private:

};


