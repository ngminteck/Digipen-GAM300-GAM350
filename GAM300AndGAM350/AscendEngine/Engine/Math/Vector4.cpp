/******************************************************************************/
/*!
\file   Vector4.cpp
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

const Vector4 Vector4::XAXIS = Vector4(1.0f, 0.0f, 0.0f, 0.0f);
const Vector4 Vector4::YAXIS = Vector4(0.0f, 1.0f, 0.0f, 0.0f);
const Vector4 Vector4::ZAXIS = Vector4(0.0f, 0.0f, 1.0f, 0.0f);
const Vector4 Vector4::ZERO = Vector4();

Color Color::RED            ASC_COLOR(255, 0, 0)
Color Color::GREEN          ASC_COLOR(0, 255, 0)
Color Color::BLUE           ASC_COLOR(0, 0, 255)
Color Color::YELLOW         ASC_COLOR(255, 255, 0)
Color Color::ORANGE         ASC_COLOR(255, 127, 0)
Color Color::ORANGE_CORAL   ASC_COLOR(255, 127, 80)
Color Color::INDIGO         ASC_COLOR(75, 0, 130)
Color Color::PURPLE         ASC_COLOR(184, 129, 238)

Color Color::WHITE          ASC_COLOR(255, 255, 255)
Color Color::BLACK          ASC_COLOR(0, 0, 0)
Color Color::GREY           ASC_COLOR(127, 127, 127)
Color Color::BLUE_LIGHT     ASC_COLOR(61, 158, 255)
Color Color::HOT_PINK       ASC_COLOR(255, 105, 180)


//Default Constructor
Vector4::Vector4() : x(0), y(0), z(0), w(0) {}

//Conversion Constructor
Vector4::Vector4(float X, float Y, float Z, float W) : x(X), y(Y), z(Z), w(W) {}

//Copy Constructor
Vector4::Vector4(const Vector4& rhs) : x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w) {}

Vector4::Vector4(const Vector3& rhs, float W) : x(rhs.x), y(rhs.y), z(rhs.z), w(W) {}

void Vector4::Set(float x, float y, float z, float w) { this->x = x; this->y = y; this->z = z; this->w = w; }

float& Vector4::operator[](int index)
{
  return m[index];
}

const float& Vector4::operator[](int index) const
{
  return m[index];
}

Vector4 & Vector4::operator=(const Vector4 & rhs)
{
  x = rhs.x, y = rhs.y, z = rhs.z, w = rhs.w;
  return *this;
}

Vector4 & Vector4::operator=(const float(&rhs)[4])
{
  x = rhs[0], y = rhs[1], z = rhs[2], w = rhs[3];
  return *this;
}

Vector4 & Vector4::operator=(const float(&rhs)[3])
{
  x = rhs[0], y = rhs[1], z = rhs[2];
  return *this;
}

bool Vector4::operator==(const Vector4& rhs)
{
  if (x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w) return true;
  return false;
}

bool Vector4::operator!=(const Vector4& rhs)
{
  if (operator==(rhs)) return false;
  return true;
}

Vector4& Vector4::operator-()
{
  x = -x; y = -y; z = -z; w = -w;
  return *this;
}

bool Vector4::operator==(const Vector4& rhs) const
{
  if (abs(x - rhs.x) > ASC_EPSILON ||
      abs(y - rhs.y) > ASC_EPSILON ||
      abs(z - rhs.z) > ASC_EPSILON ||
      abs(w - rhs.w) > ASC_EPSILON) return false;
  return true;
}

bool Vector4::operator!=(const Vector4& rhs) const
{
  return !(operator==(rhs));
}

Vector4 Vector4::operator+(const Vector4& rhs) const
{
  return Vector4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
}

Vector4 Vector4::operator-(const Vector4& rhs) const
{
  return Vector4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
}

Vector4 Vector4::operator/(float rhs) const
{
  return Vector4(x / rhs, y / rhs, z / rhs, w / rhs);
}

Vector4& Vector4::operator+=(const Vector4& rhs)
{
  x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w;
  return *this;
}

Vector4& Vector4::operator-=(const Vector4& rhs)
{
  x -= rhs.x; y -= rhs.y; z -= rhs.z; w -= rhs.w;
  return *this;
}

Vector4& Vector4::operator/=(float rhs)
{
  x /= rhs; y /= rhs; z /= rhs; w /= rhs;
  return *this;
}

Vector4& Vector4::operator*=(float rhs)
{
  x *= rhs; y *= rhs; z *= rhs; w *= rhs;
  return *this;
}

Vector4 Vector4::Normalize() const
{
  float mag = Magnitude();
  return Vector4(x / mag, y / mag, z / mag, w / mag);
}

float Vector4::Magnitude() const
{
  return sqrt(MagnitudeSq());
}

float Vector4::MagnitudeSq() const
{
  return x*x + y*y + z*z + w*w;
}

//Returns the length of the projected vector
float Vector4::Dot(const Vector4& rhs) const
{
  return x*rhs.x + y*rhs.y + z*rhs.z + w*rhs.w;
}

Vector4 operator*(const Vector4& lhs, float rhs)
{
  return Vector4(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs);
}

Vector4 operator*(float lhs, const Vector4& rhs)
{
  return Vector4(rhs.x * lhs, rhs.y * lhs, rhs.z * lhs, rhs.w * lhs);
}

std::ostream& operator<<(std::ostream& os, const Vector4& rhs)
{
  os << "< " << rhs.x << " : " << rhs.y << " : " << rhs.z << " : " << rhs.w << " >";
  return os;
}