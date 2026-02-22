#pragma once
#include "CommonMath.h"
//선분(2차원기반)
struct Segment
{
	Vector2 pos;				//시작점
	Vector2 vec;				//벡터
	Segment(Vector2 _pos, Vector2 _vec) : pos(_pos), vec(_vec) {}
};

//특정한 좌표(pos)가 선분들끼리의 교점을 충족하는가?
bool IsCollision(const Vector2& pos, const Segment& s0, const Segment& s1)
{
	if (s0.vec.GetX() == 0.0f || s0.vec.GetY() == 0.0f)
	{

	}
}

struct Sphere
{
	Vector3 pos;				//중심위치
	float r;					//반지름
	Sphere(Vector3 _pos, float _r) : pos(_pos), r(_r) {}
};

struct Plane
{
	float a, b, c, d;			//평면의방정식ax + by + cz + d = 0
	Plane() {}
	/*
	* normal, pos로 평면을 구성한다
	* ax' + by' + cz' -> 내적의결과 가 되므로 -d -> 해당점(x', y', z')의 거리를 의미한다 
	*/
	Plane(Vector3 normal, Vector3 pos)
	{
		Vector3 n = normal.Normalize();
		a = n.GetX();
		b = n.GetY();
		c = n.GetZ();
		d = -(n.DotProduct(pos));
	}

	//세 점을 받아 평면을 구성한다, 두 벡터를 구성해 외적시켜 법선을 구성하고 이를 한 점을 이용해 평면의방정식을 유도한다
	Plane(Vector3 pos0, Vector3 pos1, Vector3 pos2)
	{
		Vector3 n = ((pos1 - pos0).CrossProduct(pos2 - pos0)).Normalize();
		*this = Plane(n, pos0);
	}
};

struct Triangle
{
	Vector3 pos[3];				//꼭짓점0, 1, 2
	Plane plane;				//삼각형의평면

	Triangle(Vector3 pos0, Vector3 pos1, Vector3 pos2) : plane(pos0, pos1, pos2)
	{
		pos[0] = pos0;
		pos[1] = pos1;
		pos[2] = pos2;
	}
};