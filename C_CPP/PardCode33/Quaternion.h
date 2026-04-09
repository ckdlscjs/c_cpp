#pragma once
#include "Matrix4x4.h"
__declspec(align(16))
struct Quaternion
{
public:
	//생성자(복사/이동 생성자는 컴파일러의 자동생성에 맡긴다)
	inline Quaternion() : m_quat(0.0f, 0.0f, 0.0f, 1.0f) {}
	inline Quaternion(float w, float x, float y, float z) : m_quat(x, y, z, w) {}
	inline Quaternion(const Vector4& v) : m_quat(v) {}
	inline Quaternion(float w, const Vector3& v) : Quaternion(w, v.GetX(), v.GetY(), v.GetZ()) {}
	inline Quaternion(const Vector3& v, float degree)
	{
		float cos = cosf(_DEGTORAD(degree) * 0.5f);
		float sin = sinf(_DEGTORAD(degree) * 0.5f);
		m_quat.SetW(cos);
		m_quat.SetX(v.GetX() * sin);
		m_quat.SetY(v.GetY() * sin);
		m_quat.SetZ(v.GetZ() * sin);
	}
	inline Quaternion(float pitch, float yaw, float roll)
	{
		SetFromRotate(pitch, yaw, roll);
	}
	/*
	* 방향으로부터 사원수를 생성한다, 임의의 방향을 기저벡터의 forward와 같은평면상에 있다고
	* 가정한다면 이 둘을 외적하면 사원수의 임의회전축을 얻을수 있고 이 두벡터의 사잇각을 이용해
	* (cos(t), sin(t) * n) 을 구성한다
	*/
	inline Quaternion(const Vector3& direction)
	{
		//두 벡터를 구성한다
		Vector3 vForward(0.0f, 0.0f, 1.0f);
		Vector3 vDirection = direction.Normalize();

		//두 벡터의 각을 구한다
		//n dot v = |n||v| cos t n v는 정규화되어있으므로 n dot v 는 cos t
		//arccos의 범위는 -1~1이어야 하므로 오차방지를 위해 clamp시킨다
		float cost = vForward.DotProduct(vDirection);
		cost = Clamp(cost, -1.0f, 1.0f);
		float angle = acosf(cost); //radian

		//예외처리
		//1. 같은방향일경우
		if (cost > 1.0f - _EPSILON)
		{
			m_quat.SetW(1.0f);
			m_quat.SetX(0.0f);
			m_quat.SetY(0.0f);
			m_quat.SetZ(0.0f);
			return;
		}
			
		//2. 반대방향일경우, yaw를 기준축으로 삼는다
		if (cost < -1.0f + _EPSILON)
		{
			m_quat.SetW(0.0f);
			m_quat.SetX(0.0f);
			m_quat.SetY(1.0f);
			m_quat.SetZ(0.0f);
			return;
		}

		//일반적인경우, 두벡터의 외적을통해 기준축을 구한다
		Vector3 rotateAxis = vForward.CrossProduct(vDirection);	//forward to dir
		rotateAxis = rotateAxis.Normalize();
		angle *= 0.5f;	//q v q', theta/2를 사용한다
		float cosTheta = cosf(angle);
		float sinTheta = sinf(angle);
		Set(cosTheta, rotateAxis * sinTheta);
	}

	inline Quaternion(const Matrix4x4& mat)
	{
		SetFromMatrix(mat);
	}

	//성분별 접근
	inline float GetW() const { return m_quat.GetW(); }
	inline Vector3 GetV() const { return m_quat.ToVector3(); }
	inline void Set(float w, const Vector3& v) { m_quat.Set(v.GetX(), v.GetY(), v.GetZ(), w); }

	//연산자재정의
	inline Quaternion operator+(const Quaternion& other) const { return Quaternion(m_quat + other.m_quat); }
	inline Quaternion& operator+=(const Quaternion& other)
	{
		m_quat += other.m_quat;
		return *this;
	}
	//사원수반전, slerp등에사용
	inline Quaternion operator-() const
	{
		return Quaternion(-GetW(), -GetV());
	}
	inline Quaternion operator-(const Quaternion& other) const { return Quaternion(m_quat - other.m_quat); }
	inline Quaternion& operator-=(const Quaternion& other)
	{
		m_quat -= other.m_quat;
		return *this;
	}

	/*
	* 해밀턴 곱
	* a1a2 - b1b2 - c1c2 - d1d2   w
	* (a1b2 + b1a2 + c1d2 - d1c2) x
	* (a1c2	- b1d2 + a2c1 + d1b2) y
	* (a1d2 + b1c2 - c1b2 + d1a2) z
	* (W, V)
	* q1 * q2 = (w1w2 - (v1 dot v2), w1v2 + w2v1 + v1 cross v2)
	*/
	inline Quaternion& operator*=(const Quaternion& other)
	{
		float w1 = GetW(), w2 = other.GetW();
		Vector3 v1 = GetV(), v2 = other.GetV();
		Set(w1 * w2 - v1.DotProduct(v2), w1 * v2 + w2 * v1 + v1.CrossProduct(v2));
		return *this;
	}
	inline Quaternion operator*(const Quaternion& other) const
	{
		Quaternion ret(m_quat);
		ret *= other;
		return ret;
	}
	inline Quaternion& operator*=(const float scalar)
	{
		m_quat *= scalar;
		return *this;
	}
	inline Quaternion operator*(const float scalar) const
	{
		Quaternion ret(m_quat);
		ret *= scalar;
		return ret;
	}
	/*
	* 사원수를 이용한 회전
	* q * v * q*
	* t = 2.0f * (r cross v)
	* v' = v + w * t + r cross t
	*/
	inline Vector3 operator*(const Vector3& v) const
	{
		return (*this * Quaternion(0.0f, v) * this->Conjugate()).GetV();
	}
	inline Vector3 RotateVector(const Vector3& v) const
	{
		return *this * v;
	}
	inline Quaternion& operator/=(const float scalar)
	{
		m_quat /= scalar;
		return *this;
	}
	inline Quaternion operator/(float scalar) const
	{ 
		Quaternion ret(m_quat);
		ret /= scalar;
		return ret;
	}
	
	//inline bool operator==(const Vector4& other) { return DirectX::XMQuaternionMultiply(m_vec, other.m_vec, DirectX::XMVectorSplatEpsilon()); }
	
	//유틸함수들

	inline float LengthSquared() const
	{
		return m_quat.LengthSquared();
	}
	inline float Length() const 
	{
		return m_quat.Length();
	}

	//켤레사원수, q* = w -(xi + yj + zk)
	inline Quaternion Conjugate() const
	{
		return Quaternion(GetW(), -GetV());;
	}

	//단위사원수(회전사원수)화
	inline Quaternion Normalize() const
	{
		const float length = Length();
		if (std::abs(length - 0.0f) <= _EPSILON) return Quaternion();
		return Quaternion(m_quat / length);
	}

	//역원, q^-1 = q* / (||q|| ^ 2)
	inline Quaternion Inverse() const
	{
		const float lengthsq = LengthSquared();
		if(std::abs(lengthsq - 0.0f) <= _EPSILON) return Quaternion();
		return Conjugate() / lengthsq;
	}

	inline float DotProduct(const Quaternion& other) const 
	{
		return m_quat.DotProduct(other.m_quat);
	}
	friend inline float DotProduct(const Quaternion& v1, const Quaternion& v2);

	inline void SetFromRotate(float pitch, float yaw, float roll)
	{
		float theta_p = _DEGTORAD(pitch) * 0.5f;
		float theta_y = _DEGTORAD(yaw) * 0.5f;
		float theta_r = _DEGTORAD(roll) * 0.5f;
		float cp = cosf(theta_p), sp = sinf(theta_p);
		float cy = cosf(theta_y), sy = sinf(theta_y);
		float cr = cosf(theta_r), sr = sinf(theta_r);
		m_quat.SetW(cy * cp * cr + sy * sp * sr);
		m_quat.SetX(cy * sp * cr + sy * cp * sr);
		m_quat.SetY(sy * cp * cr - cy * sp * sr);
		m_quat.SetZ(cy * cp * sr - sy * sp * cr);
	}
	
	/*
	* 1.0f-2.0f(y^2 + z^2)		2.0f(xy + wz)			2.0f(xz - wy)			0.0f
	* 2.0f(xy - wz)				1.0f-2.0f(x^2+z^2)		2.0f(yz + wx)			0.0f
	* 2.0f(xz + wy)				2.0f(yz - wx)			1.0f-2.0f(x^2+y^2)		0.0f
	* 0.0f						0.0f					0.0f					1.0f
	* Matrix to quarterion의 목적은 가장 큰 수치(w, x, y, z)를 분모로 두어 계산의 정밀도를 올리는데 있다
	* trace = m00 + m11 + m22 -> 3 - 4(x^2 + y^2 + z^2) , x^2 + y^2 + z^2 + w^2 = 1.0f (회전/단위 사원수)
	*		= 3 - 4(1 - w^2) -> 4*w^2 - 1.0f
	*   w   = 1/2 * sqrt(trace + 1.0f)
	*/
	inline void SetFromMatrix(const Matrix4x4& mat)
	{
		// 각 축(Basis)의 스케일값을 계산하여 정규화 준비
		float sx = mat[0].ToVector3().Length();
		float sy = mat[1].ToVector3().Length();
		float sz = mat[2].ToVector3().Length();

		// 스케일이 0에 가까우면 회전 계산이 불가능하므로 단위 사원수 반환
		if (sx < _EPSILON || sy < _EPSILON || sz < _EPSILON) {
			m_quat = Quaternion(0.0f, 0.0f, 0.0f).m_quat;
			return;
		}

		// 정규화된 성분으로 m 배열 채우기, mat 내부의 순수 회전 성분만 사용하게 됨
		const float m[3][3] =
		{
			{mat[0].GetX() / sx, mat[0].GetY() / sx, mat[0].GetZ() / sx},
			{mat[1].GetX() / sy, mat[1].GetY() / sy, mat[1].GetZ() / sy},
			{mat[2].GetX() / sz, mat[2].GetY() / sz, mat[2].GetZ() / sz},
		};

		float w = 0.0f, x = 0.0f, y = 0.0f, z = 0.0f;
		float trace = m[0][0] + m[1][1] + m[2][2];
		if (trace > 0.0f)	//w^2이 0.25보다 대략적으로 크기때문에 안정적으로 분모로 사용 가능할경우
		{
			w = 0.5f * std::sqrt(trace + 1.0f);
			x = (m[1][2] - m[2][1]) / (4.0f * w);		//4wx / 4w
			y = (m[2][0] - m[0][2]) / (4.0f * w);		//4wy / 4w
			z = (m[0][1] - m[1][0]) / (4.0f * w);		//4wz / 4w
		}
		else				//w분모가 안정적이지 않아(매우작은값) 다른큰값을 분모로 잡을경우
		{
			//4 x^2 y^2 z^2들
			/*
			//정규화값으로수정(버그픽스)
			float xs = mat[0].GetX() - mat[1].GetY() - mat[2].GetZ();
			float ys = mat[1].GetY() - mat[0].GetX() - mat[2].GetZ();
			float zs = mat[2].GetZ() - mat[0].GetX() - mat[1].GetY();
			*/

			float xs = m[0][0] - m[1][1] - m[2][2];
			float ys = m[1][1] - m[0][0] - m[2][2];
			float zs = m[2][2] - m[0][0] - m[1][1];
			if (xs > ys && xs > zs)
			{
				x = 0.5f * std::sqrt(xs + 1.0f);
				w = (m[1][2] - m[2][1]) / (4.0f * x);	//4wx / 4x
				y = (m[0][1] + m[1][0]) / (4.0f * x);	//4xy / 4x
				z = (m[0][2] + m[2][0]) / (4.0f * x);	//4xz / 4x
			}
			else if (ys > xs && ys > zs)
			{
				y = 0.5f * std::sqrt(ys + 1.0f);
				w = (m[2][0] - m[0][2]) / (4.0f * y);	//4wy / 4y
				x = (m[0][1] + m[1][0]) / (4.0f * y);	//4xy / 4y
				z = (m[1][2] + m[2][1]) / (4.0f * y);	//4yz / 4y
			}
			else
			{
				z = 0.5f * std::sqrt(zs + 1.0f);
				w = (m[0][1] - m[1][0]) / (4.0f * z);	//4wz / 4z
				x = (m[0][2] + m[2][0]) / (4.0f * z);	//4xz / 4z
				y = (m[1][2] + m[2][1]) / (4.0f * z);	//4yz / 4z
			}
		}
		m_quat.Set(x, y, z, w);
	}

	//변환함수들
	inline Vector4 ToVector4() const { return m_quat; }
	inline DirectX::XMVECTOR ToXMVECTOR() const { return m_quat.ToXMVECTOR(); }
	inline DirectX::XMFLOAT4 ToXMFLOAT4() const { return m_quat.ToXMFLOAT4(); }
	inline Vector3 ToRotate() const
	{
		float pitch = 0.0f, yaw = 0.0f, roll = 0.0f;
		float x = m_quat.GetX();
		float y = m_quat.GetY();
		float z = m_quat.GetZ();
		float w = m_quat.GetW();
		pitch = 2.0f * (w * x - y * z);
		yaw = atan2f(2.0f * (x * z + w * y), 1.0f - 2.0f * (x * x + y * y));
		roll = atan2f(2.0f * (x * y + w * z), 1.0f - 2.0f * (x * x + z * z));
		//짐벌락발생시, +-90(pi*1/2)로 고정하고 yaw를 0으로바꾼다
		if (1.0f - std::abs(pitch) <= _EPSILON)
		{
			pitch = pitch >= 0.0f ? XM_PI * 0.5f : -XM_PI * 0.5f;
			yaw = 0.0f;
		}
		else
		{
			pitch = asinf(pitch);
		}
		return Vector3(_RADTODEG(pitch), _RADTODEG(yaw), _RADTODEG(roll));
	}
	inline Vector3 GetRightAxis() const
	{
		Vector3 forward(1.0f, 0.0f, 0.0f);
		return *this * forward;
	}
	inline Vector3 GetUpAxis() const
	{
		Vector3 forward(0.0f, 1.0f, 0.0f);
		return *this * forward;
	}
	inline Vector3 GetForwardAxis() const
	{
		Vector3 forward(0.0f, 0.0f, 1.0f);
		return *this * forward;
	}
private:
	Vector4 m_quat;
};

inline float DotProduct(const Quaternion& v1, const Quaternion& v2)
{
	return v1.m_quat.DotProduct(v2.m_quat);
}
