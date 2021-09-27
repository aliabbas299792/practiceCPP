#ifndef QUATERNIONS_H
#define QUATERNIONS_H

#include <cmath>
#include <iostream>

#include "vectors.h"

template <class T> class Quaternion;

using quat = Quaternion<double>;

template <class T>
inline T pure_quaternion_dot(const Quaternion<T> &p, const Quaternion<T> &q) {
  return p.e[1] * q.e[1] + p.e[2] * q.e[2] +
         p.e[3] * q.e[3]; // assumes input is pure quaternions
}

template <class T>
inline Quaternion<T> pure_quaternion_cross(const Quaternion<T> &p,
                                           const Quaternion<T> &q) {
  return Quaternion<T>( // assumes input is pure quaternions
      0, p.e[2] * q.e[3] - p.e[3] * q.e[2], p.e[3] * q.e[1] - p.e[1] * q.e[3],
      p.e[1] * q.e[2] - p.e[2] * q.e[1]);
}

template <class T>
inline Quaternion<T> operator+(const Quaternion<T> &p, const Quaternion<T> &q) {
  return Quaternion<T>{p.e[0] + q.e[0], p.e[1] + q.e[1], p.e[2] + q.e[2],
                       p.e[3] + q.e[3]};
}

template <class T>
inline Quaternion<T> operator-(const Quaternion<T> &p, const Quaternion<T> &q) {
  return Quaternion<T>{p.e[0] - q.e[0], p.e[1] - q.e[1], p.e[2] - q.e[2],
                       p.e[3] - q.e[3]};
}

template <class T>
inline Quaternion<T> operator*(const Quaternion<T> &p, const T q) {
  return Quaternion<T>{p.e[0] * q, p.e[1] * q, p.e[2] * q, p.e[3] * q};
}

template <class T>
inline Quaternion<T> operator/(const Quaternion<T> &p, const T q) {
  return Quaternion<T>{p.e[0] / q, p.e[1] / q, p.e[2] / q, p.e[3] / q};
}

template <class T>
inline Quaternion<T> operator*(const T q, const Quaternion<T> &p) {
  return Quaternion<T>{p.e[0] * q, p.e[1] * q, p.e[2] * q, p.e[3] * q};
}

template <class T>
inline Quaternion<T> operator*(const Quaternion<T> &p, const Quaternion<T> &q) {
  const T s = p.e[0] * q.e[0] - pure_quaternion_dot(p, q);
  Quaternion<T> ret_quat =
      p.e[0] * q + q.e[0] * p + pure_quaternion_cross(p, q);
  ret_quat.e[0] = s;
  return ret_quat;
}

template <class T>
inline std::ostream &operator<<(std::ostream &out, const Quaternion<T> &q) {
  return out << " { s: " << q.e[0] << ", x: " << q.e[1] << ", y: " << q.e[2]
             << ", z: " << q.e[3] << " }";
}

template <class T> class Quaternion {
public:
  T e[4]{};

public:
  Quaternion(T s, T i, T j, T k) {
    e[0] = s;
    e[1] = i;
    e[2] = j;
    e[3] = k;
  }

  T s() const { return e[0]; }
  T i() const { return e[1]; }
  T j() const { return e[2]; }
  T k() const { return e[3]; }

  Quaternion inverse() {
    const auto len_squared = length_squared();
    return Quaternion{e[0] / len_squared, -e[1] / len_squared,
                      -e[2] / len_squared, -e[3] / len_squared};
  }

  T operator[](int idx) const { return e[idx]; }
  T &operator[](int idx) { return e[idx]; }

  Quaternion operator-() { return Quaternion<T>(-e[0], -e[1], -e[2], -e[3]); }

  Quaternion &operator+=(const Quaternion &q) {
    this->e[0] += q->e[0];
    this->e[1] += q->e[1];
    this->e[2] += q->e[2];
    this->e[3] += q->e[3];
    return *this;
  }

  Quaternion &operator-=(const Quaternion &q) {
    this->e[0] -= q->e[0];
    this->e[1] -= q->e[1];
    this->e[2] -= q->e[2];
    this->e[3] -= q->e[3];
    return *this;
  }

  Quaternion &operator*=(const Quaternion &q) { return *this * q; }

  Quaternion &operator*=(const T s) {
    this->e[0] *= s;
    this->e[1] *= s;
    this->e[2] *= s;
    this->e[3] *= s;
    return *this;
  }

  Quaternion &operator/=(const T s) { return *this *= (1.0 / s); }

  T length_squared() {
    return e[0] * e[0] + e[1] * e[1] + e[2] * e[2] + e[3] * e[3];
  }

  T length() { return std::sqrt(length_squared()); }

  Quaternion &normalise() { return *this /= length(); }
};

using Point = Vec3;

inline Point rotate_quat(const Vec3 &axis, const Point &point,
                         const double angle) {
  auto P = quat{0, point[0], point[1], point[2]};

  auto q = quat{0, axis[0], axis[1], axis[2]}.normalise();
  q *= std::sin(angle / 2);
  q.e[0] = std::cos(angle / 2);

  auto P_img = q * P * q.inverse();
  return {P_img.e[1], P_img.e[2], P_img.e[3]};
}

inline Mat3 quat_rotate_mat(const Vec3 &axis, const double angle) {
  auto q = quat{0, axis[0], axis[1], axis[2]}.normalise();
  q *= std::sin(angle / 2);
  q.e[0] = std::cos(angle / 2);

  auto s = q.e[0];
  auto x = q.e[1];
  auto y = q.e[2];
  auto z = q.e[3];

  return {1 - 2 * (y * y + z * z), 2 * (x * y - s * z),
          2 * (x * z + s * y),     2 * (x * y + s * z),
          1 - 2 * (x * x + z * z), 2 * (y * z - s * x),
          2 * (x * z - s * y),     2 * (y * z + s * x),
          1 - 2 * (x * x + y * y)};
}

#endif