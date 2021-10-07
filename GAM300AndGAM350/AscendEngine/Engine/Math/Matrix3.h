#pragma once

class Matrix3
{
public:
  /**
  * Holds the tensor matrix data in array form.
  */
  float data[9];

  /**
  * Creates a new matrix.
  */
  Matrix3()
  {
    data[0] = data[1] = data[2] = data[3] = data[4] = data[5] =
      data[6] = data[7] = data[8] = 0.0f;
  }

  /**
  * Creates a new matrix with the given three vectors making
  * up its columns.
  */
  Matrix3(const Vector3 &compOne, const Vector3 &compTwo,
          const Vector3 &compThree)
  {
    setComponents(compOne, compTwo, compThree);
  }

  /**
  * Creates a new matrix with explicit coefficients.
  */
  Matrix3(float c0, float c1, float c2, float c3, float c4, float c5,
          float c6, float c7, float c8)
  {
    data[0] = c0; data[1] = c1; data[2] = c2;
    data[3] = c3; data[4] = c4; data[5] = c5;
    data[6] = c6; data[7] = c7; data[8] = c8;
  }

  /**
  * Sets the matrix to be a diagonal matrix with the given
  * values along the leading diagonal.
  */
  void setDiagonal(float a, float b, float c)
  {
    setInertiaTensorCoeffs(a, b, c);
  }

  /**
  * Sets the value of the matrix from inertia tensor values.
  */
  void setInertiaTensorCoeffs(float ix, float iy, float iz,
                              float ixy = 0, float ixz = 0, float iyz = 0)
  {
    data[0] = ix;
    data[1] = data[3] = -ixy;
    data[2] = data[6] = -ixz;
    data[4] = iy;
    data[5] = data[7] = -iyz;
    data[8] = iz;
  }

  /**
  * Sets the value of the matrix as an inertia tensor of
  * a rectangular block aligned with the body's coordinate
  * system with the given axis half-sizes and mass.
  */
  void setBlockInertiaTensor(const Vector3 &halfSizes, float mass)
  {
    Vector3 squares = halfSizes.ComponentProduct(halfSizes);
    setInertiaTensorCoeffs(0.3f*mass*(squares.y + squares.z),
                           0.3f*mass*(squares.x + squares.z),
                           0.3f*mass*(squares.x + squares.y));
  }

  /**
  * Sets the matrix to be a skew symmetric matrix based on
  * the given vector. The skew symmetric matrix is the equivalent
  * of the vector product. So if a,b are vectors. a x b = A_s b
  * where A_s is the skew symmetric form of a.
  */
  void setSkewSymmetric(const Vector3 vector)
  {
    data[0] = data[4] = data[8] = 0;
    data[1] = -vector.z;
    data[2] = vector.y;
    data[3] = vector.z;
    data[5] = -vector.x;
    data[6] = -vector.y;
    data[7] = vector.x;
  }

  /**
  * Sets the matrix values from the given three vector components.
  * These are arranged as the three columns of the vector.
  */
  void setComponents(const Vector3 &compOne, const Vector3 &compTwo,
                     const Vector3 &compThree)
  {
    data[0] = compOne.x;
    data[1] = compTwo.x;
    data[2] = compThree.x;
    data[3] = compOne.y;
    data[4] = compTwo.y;
    data[5] = compThree.y;
    data[6] = compOne.z;
    data[7] = compTwo.z;
    data[8] = compThree.z;

  }

  /**
  * Transform the given vector by this matrix.
  *
  * @param vector The vector to transform.
  */
  Vector3 operator*(const Vector3 &vector) const
  {
    return Vector3(
      vector.x * data[0] + vector.y * data[1] + vector.z * data[2],
      vector.x * data[3] + vector.y * data[4] + vector.z * data[5],
      vector.x * data[6] + vector.y * data[7] + vector.z * data[8]
    );
  }

  /**
  * Transform the given vector by this matrix.
  *
  * @param vector The vector to transform.
  */
  Vector3 transform(const Vector3 &vector) const
  {
    return (*this) * vector;
  }

  /**
  * Transform the given vector by the transpose of this matrix.
  *
  * @param vector The vector to transform.
  */
  Vector3 transformTranspose(const Vector3 &vector) const
  {
    return Vector3(
      vector.x * data[0] + vector.y * data[3] + vector.z * data[6],
      vector.x * data[1] + vector.y * data[4] + vector.z * data[7],
      vector.x * data[2] + vector.y * data[5] + vector.z * data[8]
    );
  }

  /**
  * Gets a vector representing one row in the matrix.
  *
  * @param i The row to return.
  */
  Vector3 getRowVector(int i) const
  {
    return Vector3(data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
  }

  /**
  * Gets a vector representing one axis (i.e. one column) in the matrix.
  *
  * @param i The row to return.
  *
  * @return The vector.
  */
  Vector3 getAxisVector(int i) const
  {
    return Vector3(data[i], data[i + 3], data[i + 6]);
  }

  /**
  * Sets the matrix to be the inverse of the given matrix.
  *
  * @param m The matrix to invert and use to set this.
  */
  void setInverse(const Matrix3 &m)
  {
    float t4 = m.data[0] * m.data[4];
    float t6 = m.data[0] * m.data[5];
    float t8 = m.data[1] * m.data[3];
    float t10 = m.data[2] * m.data[3];
    float t12 = m.data[1] * m.data[6];
    float t14 = m.data[2] * m.data[6];

    // Calculate the determinant
    float t16 = (t4*m.data[8] - t6*m.data[7] - t8*m.data[8] +
                 t10*m.data[7] + t12*m.data[5] - t14*m.data[4]);

    // Make sure the determinant is non-zero.
    if (fabs(t16) < FLT_EPSILON)
      return;
    float t17 = 1 / t16;

    data[0] = (m.data[4] * m.data[8] - m.data[5] * m.data[7])*t17;
    data[1] = -(m.data[1] * m.data[8] - m.data[2] * m.data[7])*t17;
    data[2] = (m.data[1] * m.data[5] - m.data[2] * m.data[4])*t17;
    data[3] = -(m.data[3] * m.data[8] - m.data[5] * m.data[6])*t17;
    data[4] = (m.data[0] * m.data[8] - t14)*t17;
    data[5] = -(t6 - t10)*t17;
    data[6] = (m.data[3] * m.data[7] - m.data[4] * m.data[6])*t17;
    data[7] = -(m.data[0] * m.data[7] - t12)*t17;
    data[8] = (t4 - t8)*t17;
  }

  /** Returns a new matrix containing the inverse of this matrix. */
  Matrix3 inverse() const
  {
    Matrix3 result;
    result.setInverse(*this);
    return result;
  }

  /**
  * Inverts the matrix.
  */
  void invert()
  {
    setInverse(*this);
  }

  /**
  * Sets the matrix to be the transpose of the given matrix.
  *
  * @param m The matrix to transpose and use to set this.
  */
  void setTranspose(const Matrix3 &m)
  {
    data[0] = m.data[0];
    data[1] = m.data[3];
    data[2] = m.data[6];
    data[3] = m.data[1];
    data[4] = m.data[4];
    data[5] = m.data[7];
    data[6] = m.data[2];
    data[7] = m.data[5];
    data[8] = m.data[8];
  }

  /** Returns a new matrix containing the transpose of this matrix. */
  Matrix3 transpose() const
  {
    Matrix3 result;
    result.setTranspose(*this);
    return result;
  }

  /**
  * Returns a matrix which is this matrix multiplied by the given
  * other matrix.
  */
  Matrix3 operator*(const Matrix3 &o) const
  {
    return Matrix3(
      data[0] * o.data[0] + data[1] * o.data[3] + data[2] * o.data[6],
      data[0] * o.data[1] + data[1] * o.data[4] + data[2] * o.data[7],
      data[0] * o.data[2] + data[1] * o.data[5] + data[2] * o.data[8],

      data[3] * o.data[0] + data[4] * o.data[3] + data[5] * o.data[6],
      data[3] * o.data[1] + data[4] * o.data[4] + data[5] * o.data[7],
      data[3] * o.data[2] + data[4] * o.data[5] + data[5] * o.data[8],

      data[6] * o.data[0] + data[7] * o.data[3] + data[8] * o.data[6],
      data[6] * o.data[1] + data[7] * o.data[4] + data[8] * o.data[7],
      data[6] * o.data[2] + data[7] * o.data[5] + data[8] * o.data[8]
    );
  }

  /**
  * Multiplies this matrix in place by the given other matrix.
  */
  void operator*=(const Matrix3 &o)
  {
    float t1;
    float t2;
    float t3;

    t1 = data[0] * o.data[0] + data[1] * o.data[3] + data[2] * o.data[6];
    t2 = data[0] * o.data[1] + data[1] * o.data[4] + data[2] * o.data[7];
    t3 = data[0] * o.data[2] + data[1] * o.data[5] + data[2] * o.data[8];
    data[0] = t1;
    data[1] = t2;
    data[2] = t3;

    t1 = data[3] * o.data[0] + data[4] * o.data[3] + data[5] * o.data[6];
    t2 = data[3] * o.data[1] + data[4] * o.data[4] + data[5] * o.data[7];
    t3 = data[3] * o.data[2] + data[4] * o.data[5] + data[5] * o.data[8];
    data[3] = t1;
    data[4] = t2;
    data[5] = t3;

    t1 = data[6] * o.data[0] + data[7] * o.data[3] + data[8] * o.data[6];
    t2 = data[6] * o.data[1] + data[7] * o.data[4] + data[8] * o.data[7];
    t3 = data[6] * o.data[2] + data[7] * o.data[5] + data[8] * o.data[8];
    data[6] = t1;
    data[7] = t2;
    data[8] = t3;
  }

  /**
  * Multiplies this matrix in place by the given scalar.
  */
  void operator*=(const float scalar)
  {
    data[0] *= scalar; data[1] *= scalar; data[2] *= scalar;
    data[3] *= scalar; data[4] *= scalar; data[5] *= scalar;
    data[6] *= scalar; data[7] *= scalar; data[8] *= scalar;
  }

  /**
  * Does a component-wise addition of this matrix and the given
  * matrix.
  */
  void operator+=(const Matrix3 &o)
  {
    data[0] += o.data[0]; data[1] += o.data[1]; data[2] += o.data[2];
    data[3] += o.data[3]; data[4] += o.data[4]; data[5] += o.data[5];
    data[6] += o.data[6]; data[7] += o.data[7]; data[8] += o.data[8];
  }

  /**
  * Sets this matrix to be the rotation matrix corresponding to
  * the given quaternion.
  */
  void setOrientation(const Quaternion &q)
  {
    data[0] = 1.0f - (2.0f * q.y*q.y + 2.0f * q.z*q.z);
    data[1] = 2.0f * q.x*q.y + 2.0f * q.z*q.w;
    data[2] = 2.0f * q.x*q.z - 2.0f * q.y*q.w;
    data[3] = 2.0f * q.x*q.y - 2.0f * q.z*q.w;
    data[4] = 1.0f - (2.0f * q.x*q.x + 2.0f * q.z*q.z);
    data[5] = 2.0f * q.y*q.z + 2.0f * q.x*q.w;
    data[6] = 2.0f * q.x*q.z + 2.0f * q.x*q.w;
    data[7] = 2.0f * q.y*q.z - 2.0f * q.x*q.w;
    data[8] = 1.0f - (2.0f * q.x*q.x + 2.0f * q.y*q.y);
  }

  Vector3 Transform(Vector3 & vector3) const;
  void SetIdentity();

  /**
  * Interpolates a couple of matrices.
  */
  static Matrix3 linearInterpolate(const Matrix3& a, const Matrix3& b, float prop);
};

using Mat3 = Matrix3;