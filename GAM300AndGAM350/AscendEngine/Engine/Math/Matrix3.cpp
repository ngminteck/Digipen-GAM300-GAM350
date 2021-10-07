#include "pch.h"

Vector3 Matrix3::Transform(Vector3 &vector3) const
{
  return (*this) * vector3;
}

void Matrix3::SetIdentity()
{
  for (int i = 0; i < 9; ++i)
    data[i] = 0.0f;

  data[0] = data[4] = data[8] = 1.0f;
}

Matrix3 Matrix3::linearInterpolate(const Matrix3 &a, const Matrix3 &b, float prop)
{
  Matrix3 result;
  for (unsigned i = 0; i < 9; i++) {
    result.data[i] = a.data[i] * (1 - prop) + b.data[i] * prop;
  }

  return result;
}