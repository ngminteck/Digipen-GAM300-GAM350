/******************************************************************************/
/*!
\file   MathExt.h
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

#ifndef MATH_EXT_H
#define MATH_EXT_H

#include "Vector3.h"


#pragma warning( disable : 4201) //Ignore nameless struct


#define ASC_PI 3.14159265f
#define ASC_HALFPI  1.57079632679f
#define ASC_EPSILON 0.00001f
#define ASC_DEG2RAD(x) ((x) * 0.0174532925f)

//for color
#define ASC_COLOR(r, g, b) {(float)(r)/255.0f, (float)(g)/255.0f, (float)(b)/255.0f, 1.0f};

//for double 
#define ASC_DEG2RAD_D(x) ((x) * 0.01745329251994329576923690768489)
#define ASC_RAD2DEG(x) ((x) * 57.2958f)
#define ASC_RANGE(X, Min, Max) (((X) < (Min)) ? (Min) : ((X) > (Max)) ? (Max) : (X))

//Returns a value range from [0.0, 1.0]
#define ASC_RAND_COL ((float)(rand()%800) / 800.0f)

//Returns a value range from [-1.0, 1.0]
#define ASC_RAND_RANGE (((float)(rand()%800) / 400.0f) - 1.0f)

#define ASC_RAND_RANGE_(minVal, maxVal) (minVal) + (ASC_RAND_COL * ((maxVal) - (minVal)))

#define ASC_LIMITANGLE(x, MAX)  ((x) + ((x) < 0.0f ? (MAX) : (x) >= (MAX) ? -(MAX) : 0.0f))

#define ASC_DISTBETWEEN_2PT(v1, v2) ((v1 - v2).Magnitude())
#define ASC_DISTBETWEEN_2PTSQ(v1, v2) ((v1 - v2).MagnitudeSq())

#define ASC_SIGN(val) (val < 0 ? -1 : 1)

inline bool Equal(float lhs, float rhs) { return fabs(lhs - rhs) < FLT_EPSILON; }

inline Vec3 RandomVec3(float xStart, float xEnd, float yStart, float yEnd, float zStart, float zEnd)
{
  const auto x = 0.5f * ((xEnd - xStart) * (ASC_RAND_RANGE + 1.0f)) + xStart;
  const auto y = 0.5f * ((yEnd - yStart) * (ASC_RAND_RANGE + 1.0f)) + yStart;
  const auto z = 0.5f * ((zEnd - zStart) * (ASC_RAND_RANGE + 1.0f)) + zStart;

  return Vec3(x, y, z);
}

inline Vec3 RandomVec3(float xRange, float yRange, float zRange)
{
  return RandomVec3(-xRange, xRange, -yRange, yRange, -zRange, zRange);
}



//typedef union Vector2
//{
//  float m[2];
//  struct { float x, y; };
//
//  Vector2();
//  Vector2(float, float);
//  Vector2(int x, int y);
//  Vector2(const Vector2&);
//  void Set(float = 0.0f, float = 0.0f);
//  void SetX(float);
//  void SetY(float);
//  Vector2& operator=(const Vector2&);
//
//  Vector2& operator-();
//  bool operator==(const Vector2&) const;
//  bool operator!=(const Vector2&) const;
//
//  Vector2 operator+(const Vector2&) const;
//  Vector2 operator-(const Vector2&) const;
//  Vector2 operator/(float) const;
//  Vector2 operator/(const Vector2&) const;
//
//  Vector2& operator+=(const Vector2&);
//  Vector2& operator-=(const Vector2&);
//  Vector2& operator/=(float);
//  Vector2& operator*=(float);
//
//
//  Vector2 Normalize() const;
//  Vector2 RotateZ(float);
//
//  float Magnitude() const;
//  float MagnitudeSq() const;
//  float Dot(const Vector2&) const;
//  float GetAngle(const Vector2&) const;
//  static Vector2 Vec2FromAngle(float deg);
//
//  static const Vector2 XAXIS;
//  static const Vector2 YAXIS;
//  static const Vector2 ZERO;
//
//} Vector2, Vec2;
//
//const size_t Vec2Size = sizeof(Vec2);
//
//Vector2 operator*(const Vector2&, float);
//Vector2 operator*(float, const Vector2&);
//
//
//// For reflection
//std::stringstream &operator<<(std::stringstream&, const Vector2&);
//std::stringstream &operator >> (std::stringstream&, Vector2&);
//std::ostream& operator<<(std::ostream&, const Vector2&);

//typedef union Vector3
//{
//  float m[3];
//  struct { float x, y, z; };
//  //struct { float pitch, yaw, roll; };
//
//  Vector3();
//  Vector3(float, float, float);
//  Vector3(const Vector3&);
//  Vector3(const Vector2&, float = 0.0f);
//  void Set(float = 0.0f, float = 0.0f, float = 0.0f);
//  Vector3& operator=(const Vector3&);
//
//  float operator[](size_t index) const;
//
//  Vector3 operator-() const;
//  bool operator==(const Vector3&) const;
//  bool operator!=(const Vector3&) const;
//
//  Vector3 operator+(const Vector3&) const;
//  Vector3 operator-(const Vector3&) const;
//  Vector3 operator/(float) const;
//
//  Vector3& operator+=(const Vector3&);
//  Vector3& operator-=(const Vector3&);
//  Vector3& operator/=(float);
//  Vector3& operator*=(float);
//
//  friend Vector3 operator*(const Quaternion &lhs, const Vector3 &rhs);
//
//  Vector3 RotateZ(float);
//  Vector3 RotateY(float);
//  Vector3 RotateX(float);
//
//  Vector3 Normalize() const;
//  float Magnitude() const;
//  float MagnitudeSq() const;
//
//  float Dot(const Vector3&) const;
//  Vector3 Cross(const Vector3&) const;
//
//  Vector3 ComponentProduct(const Vector3&) const;
//  float ScalarProduct(const Vector3 & vector3) const;
//  Vector3 VectorProduct(const Vector3 & vector3) const;
//  static const Vector3 XAXIS;
//  static const Vector3 YAXIS;
//  static const Vector3 ZAXIS;
//  static const Vector3 ZERO;
//
//} Vector3, Vec3;

//const size_t Vec3Size = sizeof(Vec3);
//
//Vector3 operator*(const Vector3&, float);
//Vector3 operator*(float, const Vector3&);
//Vector3 operator*(const Vector3&, const Vector3&);
//Vector3 operator*(const Quaternion &lhs, const Vector3 &rhs);
//
//// For reflection
//std::stringstream &operator<<(std::stringstream&, const Vector3&);
//std::stringstream &operator >> (std::stringstream&, Vector3&);
//std::ostream& operator<<(std::ostream&, const Vector3&);

//typedef union Vector4
//{
//  float m[4];
//  struct { float x, y, z, w; };
//  struct { float r, g, b, a; };
//
//  Vector4();
//  Vector4(float, float, float, float);
//  Vector4(const Vector4&);
//  Vector4(const Vector3&, float = 0.0f);
//  void Set(float = 0.0f, float = 0.0f, float = 0.0f, float = 0.0f);
//  bool operator==(const Vector4&);
//  bool operator!=(const Vector4&);
//
//  float& operator[](int index);
//  const float& operator[](int index) const;
//
//  Vector4& operator-();
//  bool operator==(const Vector4&) const;
//  bool operator!=(const Vector4&) const;
//
//  Vector4 operator+(const Vector4&) const;
//  Vector4 operator-(const Vector4&) const;
//  Vector4 operator/(float) const;
//
//  Vector4& operator+=(const Vector4&);
//  Vector4& operator-=(const Vector4&);
//  Vector4& operator/=(float);
//  Vector4& operator*=(float);
//
//  Vector4 Normalize() const;
//
//  float Magnitude() const;
//  float MagnitudeSq() const;
//  float Dot(const Vector4&) const;
//
//  Vector3 xyz() const { return Vector3(x, y, z); }
//
//} Vector4, Vec4;
//
//const size_t Vec4Size = sizeof(Vec4);
//
//Vector4 operator*(const Vector4&, float);
//Vector4 operator*(float, const Vector4&);
//std::ostream& operator<<(std::ostream&, const Vector4&);

//typedef union Matrix4
//{
//  float m[4][4];
//  struct { float mat[16]; };
//  struct {
//    float
//      m00, m01, m02, m03,
//      m10, m11, m12, m13,
//      m20, m21, m22, m23,
//      m30, m31, m32, m33;
//  };
//
//  Matrix4();
//
//  Vector3 operator*(const Vector3 &vector) const;
//  Vector4 operator*(const Vector4&) const;
//  Matrix4 operator*(const Matrix4&) const;
//  Matrix4& operator*=(const Matrix4&);
//  Matrix4& operator=(const Matrix4&);
//
//  Matrix4 Transpose() const;
//  Vec3 Transform(const Vec3 & point) const;
//  Vec3 TransformInverse(const Vec3 & point) const;
//  void SetIdentity();
//  Vector3 GetAxisVector(int i) const;
//  static Matrix4 Translate(const Vec3 &pos);
//
//  static Matrix4 ConcatModelMat(const Vector3& pos, const Vector3& scale, const Vector3& axis, float angle_deg);
//  static Matrix4 ConcatModelMat(const Vector3& pos, const Vector3& scale, const Quaternion& q);
//  static Matrix4 Rotate(const Vector3& axis, float angle_deg);
//  static Matrix4 SymPerspective(float fov, float aspect, float zNear, float zFar);
//  static Matrix4 SymOrthographic(float width, float height, float zNear, float zFar);
//  static Matrix4 NonSymOrthographic(float l, float r, float t, float b, float n, float f);
//  static Matrix4 NonSymPerspective(float l, float r, float t, float b, float n, float f);
//  static Matrix4 LookAt(const Vector3& pos, const Vector3& look, const Vector3& right);
//
//  static const Matrix4 IDENTITY;
//
//} Matrix4, Mat4;
//
//const size_t Mat4Size = sizeof(Mat4);

/**
* Holds a three degree of freedom orientation.
*
* Quaternions have
* several mathematical properties that make them useful for
* representing orientations, but require four items of data to
* hold the three degrees of freedom. These four items of data can
* be viewed as the coefficients of a complex number with three
* imaginary parts. The mathematics of the quaternion is then
* defined and is roughly correspondent to the math of 3D
* rotations. A quaternion is only a valid rotation if it is
* normalised: i.e. it has a length of 1.
*
* @note Angular velocity and acceleration can be correctly
* represented as vectors. Quaternions are only needed for
* orientation.
*/

//typedef union Quaternion
//{
//  float m[4];
//  //struct { float i, j, k, r; };
//  struct { float w, x, y, z; };
//
//  Quaternion() : Quaternion(1.0f, 0.0f, 0.0f, 0.0f) {}
//  Quaternion(float r, float i, float j, float k) : w(r), x(i), y(j), z(k) {}
//  Quaternion(const Quaternion& q) : Quaternion(q.w, q.x, q.y, q.z) {}
//
//  Quaternion& operator /= (float Scalar);
//  Quaternion& operator *= (float Scalar);
//  Quaternion& operator *= (const Quaternion& Q);
//  Quaternion& operator -= (const Quaternion& Q);
//  Quaternion& operator += (const Vector3& vec);
//
//  bool operator==(const Quaternion &) const;
//  bool operator!=(const Quaternion &) const;
//
//  friend Quaternion operator * (const Quaternion&, const Quaternion&);
//  friend Quaternion operator * (float, const Quaternion&);
//  friend Quaternion operator * (const Quaternion&, float);
//  friend Quaternion operator - (const Quaternion&);
//  friend Quaternion operator - (const Quaternion&, const Quaternion&);
//
//  inline void Set(float, float, float, float);
//  inline void SetIdentity();
//
//  Quaternion& SetRotationX(float deg);
//  Quaternion& SetRotationY(float deg);
//  Quaternion& SetRotationZ(float deg);
//
//  float Dot(const Quaternion&) const;
//  Quaternion Normalize() const;
//  Quaternion Invert() const;
//  float Length() const;
//  float LengthSq() const;
//
//  //Returns Euler angle in degrees
//  //Vector3 GetEuler() const;
//  Vector3 GetAxis() const;
//  float GetAngle() const;
//  Matrix4 GetMatrix(const Vector3& = Vector3()) const;
//
//  static Quaternion LookAt(const Vec3 & pos, const Vec3 & direction, Vec3 up = Vec3::YAXIS);
//
//} Quaternion, Quat;
//
////Quaternion EulerToQuaternion(float yaw_deg, float pitch_deg, float roll_deg);
//Quaternion EulerToQuaternion(const Vector3& euler);
//Vector3 QuaternionToEuler(const Quaternion& q);
//Matrix4 QuaternionToMatrix(const Quaternion& q);
//Quaternion AxisAngleToQuat(const Vector3& axis, float angle_deg);
//Vec3 QuatToAxisAngle(const Quat &axis);
//Quaternion RotationBetweenVectors(Vec3 start, Vec3 dest);
//Quaternion QuaternionFromMatrix(Matrix4 const& m);
//
//std::ostream& operator<<(std::ostream&, const Quaternion&);

/**
* Holds an inertia tensor, consisting of a 3x3 row-major matrix.
* This matrix is not padding to produce an aligned structure, since
* it is most commonly used with a mass (single real) and two
* damping coefficients to make the 12-element characteristics array
* of a rigid body.
*/
//typedef class Matrix3
//{
//public:
//  /**
//  * Holds the tensor matrix data in array form.
//  */
//  float data[9];
//
//  /**
//  * Creates a new matrix.
//  */
//  Matrix3()
//  {
//    data[0] = data[1] = data[2] = data[3] = data[4] = data[5] =
//      data[6] = data[7] = data[8] = 0.0f;
//  }
//
//  /**
//  * Creates a new matrix with the given three vectors making
//  * up its columns.
//  */
//  Matrix3(const Vector3 &compOne, const Vector3 &compTwo,
//    const Vector3 &compThree)
//  {
//    setComponents(compOne, compTwo, compThree);
//  }
//
//  /**
//  * Creates a new matrix with explicit coefficients.
//  */
//  Matrix3(float c0, float c1, float c2, float c3, float c4, float c5,
//    float c6, float c7, float c8)
//  {
//    data[0] = c0; data[1] = c1; data[2] = c2;
//    data[3] = c3; data[4] = c4; data[5] = c5;
//    data[6] = c6; data[7] = c7; data[8] = c8;
//  }
//
//  /**
//  * Sets the matrix to be a diagonal matrix with the given
//  * values along the leading diagonal.
//  */
//  void setDiagonal(float a, float b, float c)
//  {
//    setInertiaTensorCoeffs(a, b, c);
//  }
//
//  /**
//  * Sets the value of the matrix from inertia tensor values.
//  */
//  void setInertiaTensorCoeffs(float ix, float iy, float iz,
//    float ixy = 0, float ixz = 0, float iyz = 0)
//  {
//    data[0] = ix;
//    data[1] = data[3] = -ixy;
//    data[2] = data[6] = -ixz;
//    data[4] = iy;
//    data[5] = data[7] = -iyz;
//    data[8] = iz;
//  }
//
//  /**
//  * Sets the value of the matrix as an inertia tensor of
//  * a rectangular block aligned with the body's coordinate
//  * system with the given axis half-sizes and mass.
//  */
//  void setBlockInertiaTensor(const Vector3 &halfSizes, float mass)
//  {
//    Vector3 squares = halfSizes.ComponentProduct(halfSizes);
//    setInertiaTensorCoeffs(0.3f*mass*(squares.y + squares.z),
//      0.3f*mass*(squares.x + squares.z),
//      0.3f*mass*(squares.x + squares.y));
//  }
//
//  /**
//  * Sets the matrix to be a skew symmetric matrix based on
//  * the given vector. The skew symmetric matrix is the equivalent
//  * of the vector product. So if a,b are vectors. a x b = A_s b
//  * where A_s is the skew symmetric form of a.
//  */
//  void setSkewSymmetric(const Vector3 vector)
//  {
//    data[0] = data[4] = data[8] = 0;
//    data[1] = -vector.z;
//    data[2] = vector.y;
//    data[3] = vector.z;
//    data[5] = -vector.x;
//    data[6] = -vector.y;
//    data[7] = vector.x;
//  }
//
//  /**
//  * Sets the matrix values from the given three vector components.
//  * These are arranged as the three columns of the vector.
//  */
//  void setComponents(const Vector3 &compOne, const Vector3 &compTwo,
//    const Vector3 &compThree)
//  {
//    data[0] = compOne.x;
//    data[1] = compTwo.x;
//    data[2] = compThree.x;
//    data[3] = compOne.y;
//    data[4] = compTwo.y;
//    data[5] = compThree.y;
//    data[6] = compOne.z;
//    data[7] = compTwo.z;
//    data[8] = compThree.z;
//
//  }
//
//  /**
//  * Transform the given vector by this matrix.
//  *
//  * @param vector The vector to transform.
//  */
//  Vector3 operator*(const Vector3 &vector) const
//  {
//    return Vector3(
//      vector.x * data[0] + vector.y * data[1] + vector.z * data[2],
//      vector.x * data[3] + vector.y * data[4] + vector.z * data[5],
//      vector.x * data[6] + vector.y * data[7] + vector.z * data[8]
//    );
//  }
//
//  /**
//  * Transform the given vector by this matrix.
//  *
//  * @param vector The vector to transform.
//  */
//  Vector3 transform(const Vector3 &vector) const
//  {
//    return (*this) * vector;
//  }
//
//  /**
//  * Transform the given vector by the transpose of this matrix.
//  *
//  * @param vector The vector to transform.
//  */
//  Vector3 transformTranspose(const Vector3 &vector) const
//  {
//    return Vector3(
//      vector.x * data[0] + vector.y * data[3] + vector.z * data[6],
//      vector.x * data[1] + vector.y * data[4] + vector.z * data[7],
//      vector.x * data[2] + vector.y * data[5] + vector.z * data[8]
//    );
//  }
//
//  /**
//  * Gets a vector representing one row in the matrix.
//  *
//  * @param i The row to return.
//  */
//  Vector3 getRowVector(int i) const
//  {
//    return Vector3(data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
//  }
//
//  /**
//  * Gets a vector representing one axis (i.e. one column) in the matrix.
//  *
//  * @param i The row to return.
//  *
//  * @return The vector.
//  */
//  Vector3 getAxisVector(int i) const
//  {
//    return Vector3(data[i], data[i + 3], data[i + 6]);
//  }
//
//  /**
//  * Sets the matrix to be the inverse of the given matrix.
//  *
//  * @param m The matrix to invert and use to set this.
//  */
//  void setInverse(const Matrix3 &m)
//  {
//    float t4 = m.data[0] * m.data[4];
//    float t6 = m.data[0] * m.data[5];
//    float t8 = m.data[1] * m.data[3];
//    float t10 = m.data[2] * m.data[3];
//    float t12 = m.data[1] * m.data[6];
//    float t14 = m.data[2] * m.data[6];
//
//    // Calculate the determinant
//    float t16 = (t4*m.data[8] - t6*m.data[7] - t8*m.data[8] +
//      t10*m.data[7] + t12*m.data[5] - t14*m.data[4]);
//
//    // Make sure the determinant is non-zero.
//    if (fabs(t16) < FLT_EPSILON)
//      return;
//    float t17 = 1 / t16;
//
//    data[0] = (m.data[4] * m.data[8] - m.data[5] * m.data[7])*t17;
//    data[1] = -(m.data[1] * m.data[8] - m.data[2] * m.data[7])*t17;
//    data[2] = (m.data[1] * m.data[5] - m.data[2] * m.data[4])*t17;
//    data[3] = -(m.data[3] * m.data[8] - m.data[5] * m.data[6])*t17;
//    data[4] = (m.data[0] * m.data[8] - t14)*t17;
//    data[5] = -(t6 - t10)*t17;
//    data[6] = (m.data[3] * m.data[7] - m.data[4] * m.data[6])*t17;
//    data[7] = -(m.data[0] * m.data[7] - t12)*t17;
//    data[8] = (t4 - t8)*t17;
//  }
//
//  /** Returns a new matrix containing the inverse of this matrix. */
//  Matrix3 inverse() const
//  {
//    Matrix3 result;
//    result.setInverse(*this);
//    return result;
//  }
//
//  /**
//  * Inverts the matrix.
//  */
//  void invert()
//  {
//    setInverse(*this);
//  }
//
//  /**
//  * Sets the matrix to be the transpose of the given matrix.
//  *
//  * @param m The matrix to transpose and use to set this.
//  */
//  void setTranspose(const Matrix3 &m)
//  {
//    data[0] = m.data[0];
//    data[1] = m.data[3];
//    data[2] = m.data[6];
//    data[3] = m.data[1];
//    data[4] = m.data[4];
//    data[5] = m.data[7];
//    data[6] = m.data[2];
//    data[7] = m.data[5];
//    data[8] = m.data[8];
//  }
//
//  /** Returns a new matrix containing the transpose of this matrix. */
//  Matrix3 transpose() const
//  {
//    Matrix3 result;
//    result.setTranspose(*this);
//    return result;
//  }
//
//  /**
//  * Returns a matrix which is this matrix multiplied by the given
//  * other matrix.
//  */
//  Matrix3 operator*(const Matrix3 &o) const
//  {
//    return Matrix3(
//      data[0] * o.data[0] + data[1] * o.data[3] + data[2] * o.data[6],
//      data[0] * o.data[1] + data[1] * o.data[4] + data[2] * o.data[7],
//      data[0] * o.data[2] + data[1] * o.data[5] + data[2] * o.data[8],
//
//      data[3] * o.data[0] + data[4] * o.data[3] + data[5] * o.data[6],
//      data[3] * o.data[1] + data[4] * o.data[4] + data[5] * o.data[7],
//      data[3] * o.data[2] + data[4] * o.data[5] + data[5] * o.data[8],
//
//      data[6] * o.data[0] + data[7] * o.data[3] + data[8] * o.data[6],
//      data[6] * o.data[1] + data[7] * o.data[4] + data[8] * o.data[7],
//      data[6] * o.data[2] + data[7] * o.data[5] + data[8] * o.data[8]
//    );
//  }
//
//  /**
//  * Multiplies this matrix in place by the given other matrix.
//  */
//  void operator*=(const Matrix3 &o)
//  {
//    float t1;
//    float t2;
//    float t3;
//
//    t1 = data[0] * o.data[0] + data[1] * o.data[3] + data[2] * o.data[6];
//    t2 = data[0] * o.data[1] + data[1] * o.data[4] + data[2] * o.data[7];
//    t3 = data[0] * o.data[2] + data[1] * o.data[5] + data[2] * o.data[8];
//    data[0] = t1;
//    data[1] = t2;
//    data[2] = t3;
//
//    t1 = data[3] * o.data[0] + data[4] * o.data[3] + data[5] * o.data[6];
//    t2 = data[3] * o.data[1] + data[4] * o.data[4] + data[5] * o.data[7];
//    t3 = data[3] * o.data[2] + data[4] * o.data[5] + data[5] * o.data[8];
//    data[3] = t1;
//    data[4] = t2;
//    data[5] = t3;
//
//    t1 = data[6] * o.data[0] + data[7] * o.data[3] + data[8] * o.data[6];
//    t2 = data[6] * o.data[1] + data[7] * o.data[4] + data[8] * o.data[7];
//    t3 = data[6] * o.data[2] + data[7] * o.data[5] + data[8] * o.data[8];
//    data[6] = t1;
//    data[7] = t2;
//    data[8] = t3;
//  }
//
//  /**
//  * Multiplies this matrix in place by the given scalar.
//  */
//  void operator*=(const float scalar)
//  {
//    data[0] *= scalar; data[1] *= scalar; data[2] *= scalar;
//    data[3] *= scalar; data[4] *= scalar; data[5] *= scalar;
//    data[6] *= scalar; data[7] *= scalar; data[8] *= scalar;
//  }
//
//  /**
//  * Does a component-wise addition of this matrix and the given
//  * matrix.
//  */
//  void operator+=(const Matrix3 &o)
//  {
//    data[0] += o.data[0]; data[1] += o.data[1]; data[2] += o.data[2];
//    data[3] += o.data[3]; data[4] += o.data[4]; data[5] += o.data[5];
//    data[6] += o.data[6]; data[7] += o.data[7]; data[8] += o.data[8];
//  }
//
//  /**
//  * Sets this matrix to be the rotation matrix corresponding to
//  * the given quaternion.
//  */
//  void setOrientation(const Quaternion &q)
//  {
//    data[0] = 1.0f - (2.0f * q.y*q.y + 2.0f * q.z*q.z);
//    data[1] = 2.0f * q.x*q.y + 2.0f * q.z*q.w;
//    data[2] = 2.0f * q.x*q.z - 2.0f * q.y*q.w;
//    data[3] = 2.0f * q.x*q.y - 2.0f * q.z*q.w;
//    data[4] = 1.0f - (2.0f * q.x*q.x + 2.0f * q.z*q.z);
//    data[5] = 2.0f * q.y*q.z + 2.0f * q.x*q.w;
//    data[6] = 2.0f * q.x*q.z + 2.0f * q.x*q.w;
//    data[7] = 2.0f * q.y*q.z - 2.0f * q.x*q.w;
//    data[8] = 1.0f - (2.0f * q.x*q.x + 2.0f * q.y*q.y);
//  }
//
//  Vector3 Transform(Vector3 & vector3) const;
//  void SetIdentity();
//
//  /**
//  * Interpolates a couple of matrices.
//  */
//  static Matrix3 linearInterpolate(const Matrix3& a, const Matrix3& b, float prop);
//} Matrix3, Mat3;

inline float InvSqrt(float number)
{
  long i;
  float x2, y;
  const float threehalfs = 1.5F;

  x2 = number * 0.5F;
  y = number;
  i = *(long *)&y;                       // evil floating point bit level hacking
  i = 0x5f3759df - (i >> 1);               // what the fuck? 
  y = *(float *)&i;
  y = y * (threehalfs - (x2 * y * y));   // 1st iteration
  y = y * (threehalfs - (x2 * y * y));   // 2nd iteration, this can be removed
  return y;
}

#endif
