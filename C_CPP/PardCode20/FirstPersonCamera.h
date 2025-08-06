#pragma once
#include "Camera.h"
class FirstPersonCamera : public BaseCamera
{
public:
	FirstPersonCamera();
	~FirstPersonCamera() override;
	void Frame(float deltaTime) override;
};

