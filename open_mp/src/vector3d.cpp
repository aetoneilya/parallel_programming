#include "vector3d.h"


Vector3D operator+(Vector3D lhs, const Vector3D& rhs) { return lhs += rhs; }
Vector3D operator+(Vector3D lhs, const double k) { return lhs += k; }
Vector3D operator+(const double k, Vector3D rhs) { return rhs += k; }
Vector3D operator-(Vector3D lhs, const Vector3D& rhs) { return lhs -= rhs; }
Vector3D operator*(Vector3D lhs, const double k) { return lhs *= k; }
Vector3D operator*(const double k, Vector3D rhs) { return rhs *= k; }
Vector3D operator/(Vector3D lhs, const double k) { return lhs /= k; }

bool operator==(Vector3D lhs, Vector3D rhs) {
  return (fabs(lhs.x - rhs.x) < __DBL_EPSILON__ &&
          fabs(lhs.y - rhs.y) < __DBL_EPSILON__ &&
          fabs(lhs.z - rhs.z) < __DBL_EPSILON__);
}

bool operator!=(Vector3D lhs, Vector3D rhs) {
  return !(lhs == rhs);
}

double Distance(Vector3D p1, Vector3D p2) {
  return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2) + pow(p1.z - p2.z, 2));
}

double Length(Vector3D point) { 
    return Distance(point, Vector3D(0, 0, 0)); }

Vector3D Normalize(Vector3D point) { return point / Length(point); }
