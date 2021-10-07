/******************************************************************************/
/*!
\file   Vector2.h
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
#pragma once

union Vector2
{
  float m[2];
  struct { float x, y; };

  Vector2();
  Vector2(const float(&rhs)[2]);
  Vector2(float, float);
  Vector2(int x, int y);
  Vector2(const Vector2&);
  void Set(float = 0.0f, float = 0.0f);
  void SetX(float);
  void SetY(float);
  Vector2& operator=(const Vector2&);
  Vector2& operator=(const float(&rhs)[2]);

  Vector2& operator-();
  bool operator==(const Vector2&) const;
  bool operator!=(const Vector2&) const;

  Vector2 operator+(const Vector2&) const;
  Vector2 operator-(const Vector2&) const;
  Vector2 operator/(float) const;
  Vector2 operator/(const Vector2&) const;

  Vector2& operator+=(const Vector2&);
  Vector2& operator-=(const Vector2&);
  Vector2& operator/=(float);
  Vector2& operator*=(float);

  Vector2 Normalize() const;
  Vector2 RotateZ(float);

  float Magnitude() const;
  float MagnitudeSq() const;
  float Dot(const Vector2&) const;
  float GetAngle(const Vector2&) const;
  static Vector2 Vec2FromAngle(float deg);

  static const Vector2 XAXIS;
  static const Vector2 YAXIS;
  static const Vector2 ZERO;
};

using Vec2 = Vector2;
const std::size_t Vector2Size = sizeof(Vector2);

Vector2 operator*(const Vector2&, float);
Vector2 operator*(float, const Vector2&);


// For reflection
std::stringstream &operator<<(std::stringstream&, const Vector2&);
std::stringstream &operator >> (std::stringstream&, Vector2&);
std::ostream& operator<<(std::ostream&, const Vector2&);