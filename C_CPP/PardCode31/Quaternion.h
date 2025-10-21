#pragma once
#include "Matrix4x4.h"
__declspec(align(16))
struct Quarternion
{
public:
	//������(����/�̵� �����ڴ� �����Ϸ��� �ڵ������� �ñ��)
	inline Quarternion() : m_quat(0.0f, 0.0f, 0.0f, 1.0f) {}
	inline Quarternion(float w, float x, float y, float z) : m_quat(x, y, z, w) {}
	inline Quarternion(const Vector4& v) : m_quat(v) {}
	inline Quarternion(float w, const Vector3& v) : Quarternion(w, v.GetX(), v.GetY(), v.GetZ()) {}
	inline Quarternion(const Vector3& v, float degree)
	{
		float cos = cosf(_DEGTORAD(degree) * 0.5f);
		float sin = sinf(_DEGTORAD(degree) * 0.5f);
		m_quat.SetW(cos);
		m_quat.SetX(v.GetX() * sin);
		m_quat.SetY(v.GetY() * sin);
		m_quat.SetZ(v.GetZ() * sin);
	}
	inline Quarternion(const Vector3& rotate)
	{
		SetFromRotate(rotate);
	}

	inline Quarternion(const Matrix4x4& mat)
	{
		SetFromMatrix(mat);
	}

	//���к� ����
	inline float GetW() const { return m_quat.GetW(); }
	inline Vector3 GetV() const { return m_quat.ToVector3(); }
	inline void Set(float w, const Vector3& v) { m_quat.Set(v.GetX(), v.GetY(), v.GetZ(), w); }

	//������������
	inline Quarternion operator+(const Quarternion& other) const { return Quarternion(m_quat + other.m_quat); }
	inline Quarternion& operator+=(const Quarternion& other)
	{
		m_quat += other.m_quat;
		return *this;
	}
	//���������, slerp����
	inline Quarternion operator-() const
	{
		return Quarternion(-GetW(), -GetV());
	}
	inline Quarternion operator-(const Quarternion& other) const { return Quarternion(m_quat - other.m_quat); }
	inline Quarternion& operator-=(const Quarternion& other)
	{
		m_quat -= other.m_quat;
		return *this;
	}

	/*
	* �ع��� ��
	* a1a2 - b1b2 - c1c2 - d1d2   w
	* (a1b2 + b1a2 + c1d2 - d1c2) x
	* (a1c2	- b1d2 + a2c1 + d1b2) y
	* (a1d2 + b1c2 - c1b2 + d1a2) z
	* (W, V)
	* q1 * q2 = (w1w2 - (v1 dot v2), w1v2 + w2v1 + v1 cross v2)
	*/
	inline Quarternion& operator*=(const Quarternion& other)
	{
		float w1 = GetW(), w2 = other.GetW();
		Vector3 v1 = GetV(), v2 = other.GetV();
		Set(w1 * w2 - v1.DotProduct(v2), w1 * v2 + w2 * v1 + v1.CrossProduct(v2));
		return *this;
	}
	inline Quarternion operator*(const Quarternion& other) const
	{
		Quarternion ret(m_quat);
		ret *= other;
		return ret;
	}
	inline Quarternion& operator*=(const float scalar)
	{
		m_quat *= scalar;
		return *this;
	}
	inline Quarternion operator*(const float scalar) const
	{
		Quarternion ret(m_quat);
		ret *= scalar;
		return ret;
	}
	/*
	* ������� �̿��� ȸ��
	* q * v * q*
	* t = 2.0f * (r cross v)
	* v' = v + w * t + r cross t
	*/
	inline Vector3 operator*(const Vector3& v) const
	{
		return (*this * Quarternion(0.0f, v) * this->Conjugate()).GetV();
	}
	inline Vector3 RotateVector(const Vector3& v) const
	{
		return *this * v;
	}
	inline Quarternion& operator/=(const float scalar)
	{
		m_quat /= scalar;
		return *this;
	}
	inline Quarternion operator/(float scalar) const
	{ 
		Quarternion ret(m_quat);
		ret /= scalar;
		return ret;
	}
	
	//inline bool operator==(const Vector4& other) { return DirectX::XMQuaternionMultiply(m_vec, other.m_vec, DirectX::XMVectorSplatEpsilon()); }
	
	//��ƿ�Լ���

	inline float LengthSquared() const
	{
		return m_quat.LengthSquared();
	}
	inline float Length() const 
	{
		return m_quat.Length();
	}

	//�ӷ������, q* = w -(xi + yj + zk)
	inline Quarternion Conjugate() const
	{
		return Quarternion(GetW(), -GetV());;
	}

	//���������(ȸ�������)ȭ
	inline Quarternion Normalize() const
	{
		const float length = Length();
		if (std::abs(length - 0.0f) <= _EPSILON) return Quarternion();
		return Quarternion(m_quat / length);
	}
	//����, q^-1 = q* / (||q|| ^ 2)
	inline Quarternion Inverse() const
	{
		const float lengthsq = LengthSquared();
		if(std::abs(lengthsq - 0.0f) <= _EPSILON) return Quarternion();
		return Conjugate() / lengthsq;
	}

	inline float DotProduct(const Quarternion& other) const 
	{
		return m_quat.DotProduct(other.m_quat);
	}
	friend inline float DotProduct(const Quarternion& v1, const Quarternion& v2);

	inline void SetFromRotate(const Vector3& rotate)
	{
		float theta_p = _DEGTORAD(rotate.GetX()) * 0.5f;
		float theta_y = _DEGTORAD(rotate.GetY()) * 0.5f;
		float theta_r = _DEGTORAD(rotate.GetZ()) * 0.5f;
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
	* Matrix to quarterion�� ������ ���� ū ��ġ(w, x, y, z)�� �и�� �ξ� ����� ���е��� �ø��µ� �ִ�
	* trace = m00 + m11 + m22 -> 3 - 4(x^2 + y^2 + z^2) , x^2 + y^2 + z^2 + w^2 = 1.0f (ȸ��/���� �����)
	*		= 3 - 4(1 - w^2) -> 4*w^2 - 1.0f
	*   w   = 1/2 * sqrt(trace + 1.0f)
	*/
	inline void SetFromMatrix(const Matrix4x4& mat)
	{
		float w = 0.0f, x = 0.0f, y = 0.0f, z = 0.0f;
		const float m[3][3] =
		{
			{mat[0].GetX(), mat[0].GetY(), mat[0].GetZ()},
			{mat[1].GetX(), mat[1].GetY(), mat[1].GetZ()},
			{mat[2].GetX(), mat[2].GetY(), mat[2].GetZ()},
		};
		float trace = m[0][0] + m[1][1] + m[2][2];
		if (trace > 0.0f)	//w^2�� 0.25���� �뷫������ ũ�⶧���� ���������� �и�� ��� �����Ұ��
		{
			w = 0.5f * std::sqrt(trace + 1.0f);
			x = (m[1][2] - m[2][1]) / (4.0f * w);		//4wx / 4w
			y = (m[2][0] - m[0][2]) / (4.0f * w);		//4wy / 4w
			z = (m[0][1] - m[1][0]) / (4.0f * w);		//4wz / 4w
		}
		else				//w�и� ���������� �ʾ�(�ſ�������) �ٸ�ū���� �и�� �������
		{
			//4 x^2 y^2 z^2��
			float xs = mat[0].GetX() - mat[1].GetY() - mat[2].GetZ();
			float ys = mat[1].GetY() - mat[0].GetX() - mat[2].GetZ();
			float zs = mat[2].GetZ() - mat[0].GetX() - mat[1].GetY();
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

	//��ȯ�Լ���
	inline Vector4 ToVector4() const { return m_quat; }
	inline DirectX::XMVECTOR ToXMVECTOR() const { return m_quat.ToXMVECTOR(); }
	inline DirectX::XMFLOAT4 ToXMFLOAT4() const { return m_quat.ToXMFLOAT4(); }
	inline Vector3 GetRotate() const
	{
		float pitch = 0.0f, yaw = 0.0f, roll = 0.0f;
		float x = m_quat.GetX();
		float y = m_quat.GetY();
		float z = m_quat.GetZ();
		float w = m_quat.GetW();
		pitch = 2.0f * (w * x - y * z);
		yaw = atan2f(2.0f * (x * z + w * y), 1.0f - 2.0f * (x * x + y * y));
		roll = atan2f(2.0f * (x * y + w * z), 1.0f - 2.0f * (x * x + z * z));
		//�������߻���, +-90(pi*1/2)�� �����ϰ� yaw�� 0���ιٲ۴�
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
private:
	Vector4 m_quat;
};

inline float DotProduct(const Quarternion& v1, const Quarternion& v2)
{
	return v1.m_quat.DotProduct(v2.m_quat);
}
