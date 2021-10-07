/******************************************************************************/
/*!
\file   Vector4.h
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

#pragma once

union Vector4;
typedef Vector4 Color;

union Vector4
{
  float m[4];
  struct { float x, y, z, w; };
  struct { float r, g, b, a; };
  struct { float rgb[3]; float alpha; };
  struct { float xyz[3]; float wComp; };

  Vector4();
  Vector4(float, float, float, float);
  Vector4(const Vector4&);
  Vector4(const Vector3&, float = 0.0f);
  void Set(float = 0.0f, float = 0.0f, float = 0.0f, float = 0.0f);

  Vector4& operator=(const Vector4&);
  Vector4& operator=(const float(&rhs)[4]);
  Vector4& operator=(const float(&rhs)[3]);

  bool operator==(const Vector4&);
  bool operator!=(const Vector4&);

  float& operator[](int index);
  const float& operator[](int index) const;

  Vector4& operator-();
  bool operator==(const Vector4&) const;
  bool operator!=(const Vector4&) const;

  Vector4 operator+(const Vector4&) const;
  Vector4 operator-(const Vector4&) const;
  Vector4 operator/(float) const;

  Vector4& operator+=(const Vector4&);
  Vector4& operator-=(const Vector4&);
  Vector4& operator/=(float);
  Vector4& operator*=(float);

  Vector4 Normalize() const;

  float Magnitude() const;
  float MagnitudeSq() const;
  float Dot(const Vector4&) const;

  static Color RED;
  static Color GREEN;
  static Color BLUE;
  static Color YELLOW;
  static Color ORANGE;
  static Color ORANGE_CORAL;
  static Color INDIGO;
  static Color PURPLE;

  static Color WHITE;
  static Color BLACK;
  static Color GREY;
  static Color BLUE_LIGHT;
  static Color HOT_PINK;

  static const Vector4 XAXIS; 
  static const Vector4 YAXIS;
  static const Vector4 ZAXIS;
  static const Vector4 ZERO;
};

using Vec4 = Vector4;
const std::size_t Vec4Size = sizeof(Vec4);

Vector4 operator*(const Vector4&, float);
Vector4 operator*(float, const Vector4&);
std::ostream& operator<<(std::ostream&, const Vector4&);
