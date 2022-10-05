#ifndef OPEN_MP_POINT3D_H_
#define OPEN_MP_POINT3D_H_

#include <cmath>

struct Vector3D {
  double x = 0;
  double y = 0;
  double z = 0;

  Vector3D(){}
  Vector3D(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {}

  struct Vector3D& operator+=(const Vector3D& rhs) {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
  }
  struct Vector3D& operator-=(const Vector3D& rhs) {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    return *this;
  }
  struct Vector3D& operator+=(const double& k) {
    x += k;
    y += k;
    z += k;
    return *this;
  }
  struct Vector3D& operator*=(const double& k) {
    x *= k;
    y *= k;
    z *= k;
    return *this;
  }
  struct Vector3D& operator/=(const double& k) {
    x /= k;
    y /= k;
    z /= k;
    return *this;
  }
};

Vector3D operator+(Vector3D lhs, const Vector3D& rhs);
Vector3D operator+(Vector3D lhs, const double k);
Vector3D operator+(const double k, Vector3D rhs);
Vector3D operator-(Vector3D lhs, const Vector3D& rhs);
Vector3D operator*(Vector3D lhs, const double k);
Vector3D operator*(const double k, Vector3D rhs);
Vector3D operator/(Vector3D lhs, const double k);

bool operator==(Vector3D lhs, Vector3D rhs);
bool operator!=(Vector3D lhs, Vector3D rhs);

double Distance(Vector3D p1, Vector3D p2);
double Length(Vector3D point);
Vector3D Normalize(Vector3D point);

#endif  // OPEN_MP_POINT3D_H_