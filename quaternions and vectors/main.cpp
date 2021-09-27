#include "quaternions.h"
#include "vectors.h"

constexpr double PI = 3.14159265359;

int main() {
  Vec3 axis{1, 2, 4};
  Vec3 point{5, 1, -4};

  const double rot = PI / 2 + 0.22334;

  std::cout << "Rotation matrix from quaternions:\n"
            << quat_rotate_mat(axis, rot) << "\n";
  std::cout << "Rotated using quaternions: " << rotate_quat(axis, point, rot)
            << "\n\n";

  std::cout << "Rotation matrix without quaternions:\n"
            << vector_rotate_mat(axis, rot) << "\n";
  std::cout << "Rotated without quaternions: "
            << rotate_vector(axis, point, rot) << "\n";
}