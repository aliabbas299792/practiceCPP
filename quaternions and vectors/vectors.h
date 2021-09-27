#ifndef VECTORS_H
#define VECTORS_H

#include <cmath>
#include <iostream>
#include <ostream>

struct Vec3 {
  double e[3];
  Vec3(double e1, double e2, double e3) {
    e[0] = e1;
    e[1] = e2;
    e[2] = e3;
  }

  Vec3() {}

  double operator[](int idx) const { return e[idx]; }

  double &operator[](int idx) { return e[idx]; }
};

inline std::ostream &operator<<(std::ostream &out, const Vec3 &v) {
  return out << " { x: " << v.e[0] << ", y: " << v.e[1] << ", z: " << v.e[2]
             << " }";
}

struct Mat3 {
  double e[9];
  Mat3(double e1, double e2, double e3, double e4, double e5, double e6,
       double e7, double e8, double e9) {
    e[0] = e1;
    e[1] = e2;
    e[2] = e3;
    e[3] = e4;
    e[4] = e5;
    e[5] = e6;
    e[6] = e7;
    e[7] = e8;
    e[8] = e9;
  }

  Mat3(Vec3 x, Vec3 y, Vec3 z) {
    e[0] = x.e[0];
    e[1] = y.e[0];
    e[2] = z.e[0];
    e[3] = x.e[1];
    e[4] = y.e[1];
    e[5] = z.e[1];
    e[6] = x.e[2];
    e[7] = y.e[2];
    e[8] = z.e[2];
  }

  double operator()(int m, int n) const { return e[m * 3 + n]; }
  double &operator()(int m, int n) { return e[m * 3 + n]; }
};

inline std::string format_num(double num) {
  char out[20];
  sprintf(out, "%8.5f", num);
  return out;
}

inline std::ostream &operator<<(std::ostream &out, Mat3 mat) {
  return out << "[ " << format_num(mat(0, 0)) << ", " << format_num(mat(0, 1))
             << ", " << format_num(mat(0, 2)) << "\n  " << format_num(mat(1, 0))
             << ", " << format_num(mat(1, 1)) << ", " << format_num(mat(1, 2))
             << "\n  " << format_num(mat(2, 0)) << ", " << format_num(mat(2, 1))
             << ", " << format_num(mat(2, 2)) << " ]";
}

inline Vec3 operator-(const Vec3 &v1, const Vec3 &v2) {
  return {v1[0] - v2[0], v1[1] - v2[1], v1[2] - v2[2]};
}

inline Vec3 operator+(const Vec3 &v1, const Vec3 &v2) {
  return {v1[0] + v2[0], v1[1] + v2[1], v1[2] + v2[2]};
}

inline Vec3 operator*(const Vec3 &v1, double scalar) {
  return {v1[0] * scalar, v1[1] * scalar, v1[2] * scalar};
}

inline Vec3 operator*(double scalar, const Vec3 &v1) {
  return {v1[0] * scalar, v1[1] * scalar, v1[2] * scalar};
}

inline double dot(const Vec3 &v1, const Vec3 &v2) {
  return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

inline double magnitude_squared(const Vec3 &input) {
  return input[0] * input[0] + input[1] * input[1] + input[2] * input[2];
}

inline double length(const Vec3 &input) {
  return std::sqrt(magnitude_squared(input));
}

inline Vec3 normalise(const Vec3 &input) {
  const double len = length(input);
  return {input[0] / len, input[1] / len, input[2] / len};
}

inline Vec3 cross_product(const Vec3 &v1, const Vec3 &v2) {
  return {v1[1] * v2[2] - v1[2] * v2[1], v1[2] * v2[0] - v1[0] * v2[2],
          v1[0] * v2[1] - v1[1] * v2[0]};
}

inline Vec3 rotate_vector(const Vec3 &axis, const Vec3 &point, double theta) {
  const Vec3 n_normalised = normalise(axis); // normalised axis of rotation

  // so we rotate only perpendicular to the axis
  const Vec3 parallel_to_axis_component =
      dot(n_normalised, point) * n_normalised;
  const Vec3 point_projected = point - parallel_to_axis_component;

  const Vec3 cross_axis_projected_point =
      cross_product(n_normalised, point_projected);

  // n_normalised, point_projected and cross_axis_projected_point
  // form a set of 3 linearly independent basis vectors of the same magnitude

  // recall that the parametric equiation of a circle with radius 1 is
  // x=cos(t) and y=sin(t)
  // but that is for the case when x=(1,0) and y=(0,1), if we instead have
  // the basis vector for y be Y and for x be X, we have:
  // x=X*cos(t) and y=Y*sin(t), and that is in effect what we have below
  // we are rotating about a point on a plane in 3D space with
  // the parameter theta

  const Vec3 rotated_point = point_projected * std::cos(theta) +
                             cross_axis_projected_point * std::sin(theta);

  return rotated_point + parallel_to_axis_component;
}

inline Mat3 vector_rotate_mat(const Vec3 &axis, const double angle) {
  const Vec3 n_normalised = normalise(axis); // normalised axis of rotation

  Vec3 output_basis_vector_x;
  {
    Vec3 basis_vector_x{1, 0, 0};

    const Vec3 parallel_to_axis_component =
        dot(n_normalised, basis_vector_x) * n_normalised;

    // get the component purely parallel to the given axis
    const Vec3 point_projected = basis_vector_x - parallel_to_axis_component;

    // get the vector which acts as our 'y' axis on a 2D plane
    // (see comments in rotate_vector)
    const Vec3 cross_axis_projected_point =
        cross_product(n_normalised, point_projected);

    // (see comments in rotate_vector)
    const Vec3 rotated_point = point_projected * std::cos(angle) +
                               cross_axis_projected_point * std::sin(angle);

    // the final output basis vector
    output_basis_vector_x = rotated_point + parallel_to_axis_component;
  }

  Vec3 output_basis_vector_y;
  {
    Vec3 basis_vector_y{0, 1, 0};

    const Vec3 parallel_to_axis_component =
        dot(n_normalised, basis_vector_y) * n_normalised;

    // get the component purely parallel to the given axis
    const Vec3 point_projected = basis_vector_y - parallel_to_axis_component;

    // get the vector which acts as our 'y' axis on a 2D plane
    // (see comments in rotate_vector)
    const Vec3 cross_axis_projected_point =
        cross_product(n_normalised, point_projected);

    // (see comments in rotate_vector)
    const Vec3 rotated_point = point_projected * std::cos(angle) +
                               cross_axis_projected_point * std::sin(angle);

    // the final output basis vector
    output_basis_vector_y = rotated_point + parallel_to_axis_component;
  }

  Vec3 output_basis_vector_z;
  {
    Vec3 basis_vector_z{0, 0, 1};

    const Vec3 parallel_to_axis_component =
        dot(n_normalised, basis_vector_z) * n_normalised;

    // get the component purely parallel to the given axis
    const Vec3 point_projected = basis_vector_z - parallel_to_axis_component;

    // get the vector which acts as our 'y' axis on a 2D plane
    // (see comments in rotate_vector)
    const Vec3 cross_axis_projected_point =
        cross_product(n_normalised, point_projected);

    // (see comments in rotate_vector)
    const Vec3 rotated_point = point_projected * std::cos(angle) +
                               cross_axis_projected_point * std::sin(angle);

    // the final output basis vector
    output_basis_vector_z = rotated_point + parallel_to_axis_component;
  }

  // the idea is that if you rotate the 3 basis vectors properly,
  // you can make them into a 3x3 matrix, since each column
  // of a 3x3 matrix represents the transformation of a basis vector
  return {output_basis_vector_x, output_basis_vector_y, output_basis_vector_z};
}

#endif