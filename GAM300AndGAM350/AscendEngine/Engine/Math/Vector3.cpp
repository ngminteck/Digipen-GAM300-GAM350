/******************************************************************************/
/*!
\file   Vector3.cpp
\author Alfi Ramli
\par    email: alfi.ramli\@digipen.edu
\par    Digipen login: alfi.ramli
\date   02/02/2020
\brief
Contains definition of Vector4 maths functions.

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#include "pch.h"

const Vector3 Vector3::XAXIS = Vector3(1.0f, 0.0f, 0.0f);
const Vector3 Vector3::YAXIS = Vector3(0.0f, 1.0f, 0.0f);
const Vector3 Vector3::ZAXIS = Vector3(0.0f, 0.0f, 1.0f);
const Vector3 Vector3::ZERO = Vector3();

//Default Constructor
Vector3::Vector3() : x(0), y(0), z(0) {}

//Conversion Constructor
Vector3::Vector3(float X, float Y, float Z) : x(X), y(Y), z(Z) {}

//Copy Constructor
Vector3::Vector3(const Vector3& rhs) : x(rhs.x), y(rhs.y), z(rhs.z) {}

Vector3::Vector3(const float(&rhs)[3]) : x(rhs[0]), y(rhs[1]), z(rhs[2]) {}

Vector3::Vector3(const Vector2& rhs, float depth) : x(rhs.x), y(rhs.y), z(depth) {}

Vector3& Vector3::operator = (const Vector3& rhs)
{
  x = rhs.x; y = rhs.y; z = rhs.z;
  return *this;
}

Vector3 & Vector3::operator=(const float(& rhs)[3])
{
  x = rhs[0]; y = rhs[1]; z = rhs[2];
  return *this;
}

float Vector3::operator[](std::size_t index) const
{
  return *(&x + index);
}

Vector3 Vector3::operator-() const
{
  return Vec3(-x, -y, -z);
}

bool Vector3::operator==(const Vector3& rhs) const
{
  if (abs(x - rhs.x) > ASC_EPSILON ||
      abs(y - rhs.y) > ASC_EPSILON ||
      abs(z - rhs.z) > ASC_EPSILON) return false;
  return true;
}

bool Vector3::operator!=(const Vector3& rhs) const
{
  return !(operator==(rhs));
}

void Vector3::Set(float x, float y, float z) { this->x = x; this->y = y; this->z = z; }

Vector3 Vector3::operator+(const Vector3& rhs) const
{
  return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
}

Vector3 Vector3::operator-(const Vector3& rhs) const
{
  return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
}

Vector3 Vector3::operator/(float rhs) const
{
  return Vector3(x / rhs, y / rhs, z / rhs);
}

Vector3& Vector3::operator+=(const Vector3& rhs)
{
  x += rhs.x; y += rhs.y; z += rhs.z;
  return *this;
}

Vector3& Vector3::operator-=(const Vector3& rhs)
{
  x -= rhs.x; y -= rhs.y; z -= rhs.z;
  return *this;
}

Vector3& Vector3::operator/=(float rhs)
{
  x /= rhs; y /= rhs; z /= rhs;
  return *this;
}

Vector3& Vector3::operator*=(float rhs)
{
  x *= rhs; y *= rhs; z *= rhs;
  return *this;
}

Vector3 Vector3::RotateZ(float angle)
{
  angle = ASC_DEG2RAD(angle);
  float sinA = sin(angle);
  float cosA = cos(angle);
  Vector3 tmp;
  tmp.x = (x * cosA) - (y * sinA);
  tmp.y = (x * sinA) + (y * cosA);
  tmp.z = z;
  return tmp;
}

Vector3 Vector3::RotateY(float angle)
{
  angle = ASC_DEG2RAD(angle);
  float sinA = sin(angle);
  float cosA = cos(angle);
  Vector3 tmp;
  tmp.x = (x * cosA) + (z * sinA);
  tmp.y = y;
  tmp.z = -(x * sinA) + (z * cosA);
  return tmp;
}

Vector3 Vector3::RotateX(float angle)
{
  angle = ASC_DEG2RAD(angle);
  float sinA = sin(angle);
  float cosA = cos(angle);
  Vector3 tmp;
  tmp.x = x;
  tmp.y = (y * cosA) - (z * sinA);
  tmp.z = (y * sinA) + (z * cosA);
  return tmp;
}

Vector3 Vector3::VecQuatProduct(const Quaternion& Quat) const
{
    // Extract the vector part of the quaternion
    Vector3 u(Quat.x, Quat.y, Quat.z);

    // Extract the scalar part of the quaternion
    float s = Quat.w;

    // Do the math
    return Vector3(2.0f * u.Dot(*this) * u
        + (s * s - u.Dot( u)) * (*this)
        + 2.0f * s * u.Cross(*this));
}

Vector3 Vector3::Normalize() const
{
  float mag = InvSqrt(MagnitudeSq());
  return Vector3(x * mag, y * mag, z * mag);
}

float Vector3::Magnitude() const
{
  return sqrt(MagnitudeSq());
}

float Vector3::MagnitudeSq() const
{
  return x*x + y*y + z*z;
}

//Returns the length of the projected vector
float Vector3::Dot(const Vector3& rhs) const
{
  return x*rhs.x + y*rhs.y + z*rhs.z;
}

void Vector3::OrthoNormalize(Vector3 &normal, Vector3 &tangent)
{
  normal = normal.Normalize();
  Vector3 proj = normal * tangent.Dot(normal);
  tangent = tangent - proj;
  tangent = tangent.Normalize();
}

Vector3 operator*(const Vector3& lhs, float rhs)
{
  return Vector3(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
}

Vector3 operator*(float lhs, const Vector3& rhs)
{
  return Vector3(rhs.x * lhs, rhs.y * lhs, rhs.z * lhs);
}

Vector3 operator*(const Vector3 &a, const Vector3 &b)
{
  return Vector3(a.x * b.x, a.y * b.y, a.z * b.z);
}

Vector3 operator*(const Quaternion &q, const Vector3 &v)
{
  const Vector3 QuatVector(q.x, q.y, q.z);
  const Vector3 uv(QuatVector.Cross(v));
  const Vector3 uuv(QuatVector.Cross(uv));

  return v + ((uv * q.w) + uuv) * 2.0f;
}

Vector3 Vector3::Cross(const Vector3& r) const
{
  return Vector3(y*r.z - z*r.y, z*r.x - x*r.z, x*r.y - y*r.x);
}

Vector3 Vector3::ComponentProduct(const Vector3 &rhs) const
{
  return Vector3(x * rhs.x, y * rhs.y, rhs.z);
}

float Vector3::ScalarProduct(const Vector3 &vec) const
{
  return x*vec.x + y*vec.y + z*vec.z;
}

Vector3 Vector3::VectorProduct(const Vector3 &vec) const
{
  return Vector3(y*vec.z - z*vec.y,
                 z*vec.x - x*vec.z,
                 x*vec.y - y*vec.x);
}

std::stringstream &operator<<(std::stringstream &os, const Vector3& rhs)
{
  os << rhs.x << " " << rhs.y << " " << rhs.z;
  return os;
}

std::stringstream &operator >> (std::stringstream &is, Vector3& rhs)
{
  is >> rhs.x >> rhs.y >> rhs.z;
  return is;
}

std::ostream& operator<<(std::ostream& os, const Vector3& rhs)
{
  os << "< " << rhs.x << " : " << rhs.y << " : " << rhs.z << " >";
  return os;
}