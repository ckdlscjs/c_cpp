#pragma once
#include "CommonHeader.h"

class BaseView 
{
public:
	BaseView();
	virtual ~BaseView();
	BaseView(const BaseView&) = delete;
	BaseView& operator=(const BaseView&) = delete;
	BaseView(BaseView&&) = delete;
	BaseView& operator=(BaseView&&) = delete;
};

inline BaseView::BaseView()
{
	std::cout << "Initialize : " << "BaseView " <<" Class" << '\n';
}

inline BaseView::~BaseView()
{
	std::cout << "Release : " << "BaseView" << " Class" << '\n';
}