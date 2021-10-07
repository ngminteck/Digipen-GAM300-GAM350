/******************************************************************************/
/*!
\file   Vector2.cpp
\author Alfi Ramli
\par    email: alfi.ramli\@digipen.edu
\par    Digipen login: alfi.ramli
\date   02/02/2020
\brief
maths functions.

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#include "pch.h"

const Vector2 Vector2::XAXIS = Vector2(1.0f, 0.0f);
const Vector2 Vector2::YAXIS = Vector2(0.0f, 1.0f);
const Vector2 Vector2::ZERO = Vector2();

// Default Constructor
Vector2::Vector2() : x(0), y(0) {}

Vector2::Vector2(const float(&rhs)[2]) : x(rhs[0]), y(rhs[1]) {}

// Conversion Constructors
Vector2::Vector2(float X, float Y) : x(X), y(Y) {}
Vector2::Vector2(int x, int y) : x((float)x), y((float)y) {}
//Vector2::Vector2(const Vector3& rhs) : x(rhs.x), y(rhs.y) {}

//Copy Constructor
Vector2::Vector2(const Vector2& rhs) : x(rhs.x), y(rhs.y) {}


//Assignment Operator
Vector2& Vector2::operator=(const Vector2& rhs)
{
  x = rhs.x; y = rhs.y;
  return *this;
}

Vector2 & Vector2::operator=(const float(&rhs)[2])
{
  x = rhs[0]; y = rhs[1];
  return *this;
}

void Vector2::Set(float x, float y) { this->x = x; this->y = y; }
void Vector2::SetX(float x) { this->x = x; }
void Vector2::SetY(float y) { this->y = y; }

Vector2& Vector2::operator-()
{
  x = -x; y = -y;
  return *this;
}

bool Vector2::operator==(const Vector2& rhs) const
{
  if (abs(x - rhs.x) > ASC_EPSILON ||
      abs(y - rhs.y) > ASC_EPSILON) return false;
  return true;
}

bool Vector2::operator!=(const Vector2& rhs) const
{
  return !(operator==(rhs));
}

Vector2 Vector2::operator+(const Vector2& rhs) const
{
  return Vector2(x + rhs.x, y + rhs.y);
}

Vector2 Vector2::operator-(const Vector2& rhs) const
{
  return Vector2(x - rhs.x, y - rhs.y);
}

Vector2 Vector2::operator/(float rhs) const
{
  return Vector2(x / rhs, y / rhs);
}

Vector2 Vector2::operator/(const Vector2& rhs) const
{
  return Vector2(x / rhs.x, y / rhs.y);
}

Vector2& Vector2::operator+=(const Vector2& rhs)
{
  x += rhs.x; y += rhs.y;
  return *this;
}

Vector2& Vector2::operator-=(const Vector2& rhs)
{
  x -= rhs.x; y -= rhs.y;
  return *this;
}

Vector2& Vector2::operator/=(float rhs)
{
  x /= rhs; y /= rhs;
  return *this;
}

Vector2& Vector2::operator*=(float rhs)
{
  x *= rhs; y *= rhs;
  return *this;
}

Vector2 Vector2::Normalize() const
{
  float mag = Magnitude();
  if (!mag) return Vec2(0, 0); //Check for infinity
  else return Vector2(x / mag, y / mag);
}

Vector2 Vector2::RotateZ(float angle)
{
  angle = ASC_DEG2RAD(angle);
  float sinA = sin(angle);
  float cosA = cos(angle);
  Vector2 tmp;
  tmp.x = (x * cosA) - (y * sinA);
  tmp.y = (x * sinA) + (y * cosA);
  return tmp;
}

float Vector2::Magnitude() const
{
  return sqrt(MagnitudeSq());
}

float Vector2::MagnitudeSq() const
{
  return x*x + y*y;
}

//Returns the length of the projected vector
float Vector2::Dot(const Vector2& rhs) const
{
  return x*rhs.x + y*rhs.y;
}

float Vector2::GetAngle(const Vector2& rhs) const
{
  float magMult = this->Magnitude() * rhs.Magnitude();
  if (abs(magMult) < ASC_EPSILON*ASC_EPSILON) return 0;
  return ASC_RAD2DEG(acos(Dot(rhs) / magMult));
}


// Static function to return a vector from an angle in radians.
// Optimize: Compute using sin/cos directly.
Vector2 Vector2::Vec2FromAngle(float deg)
{
  deg = ASC_DEG2RAD(deg);
  return Vector2(cos(deg), sin(deg)).Normalize();
}

Vector2 operator*(const Vector2& lhs, float rhs)
{
  return Vector2(lhs.x * rhs, lhs.y * rhs);
}

Vector2 operator*(float lhs, const Vector2& rhs)
{
  return Vector2(rhs.x * lhs, rhs.y * lhs);
}

std::stringstream &operator<<(std::stringstream &os, const Vector2& rhs)
{
  os << rhs.x << " " << rhs.y;
  return os;
}

std::stringstream &operator >> (std::stringstream &is, Vector2& rhs)
{
  is >> rhs.x >> rhs.y;
  return is;
}

std::ostream& operator<<(std::ostream& os, const Vector2& rhs)
{
  os << "< " << rhs.x << " : " << rhs.y << " >";
  return os;
}