#ifndef OPEN_MP_BODY_H_
#define OPEN_MP_BODY_H_

#include "vector3d.h"

class Body {
 public:
  Body(Vector3D pos, Vector3D move, double radius, double mass)
      : position_(pos), velocity_(move), radius_(radius), mass_(mass) {}

  void Move() { position_ = position_ + velocity_; }

  // a = F/m
  void ApplyForce(Vector3D force) { velocity_ += force / mass_; }
  void SetSpeed(Vector3D move) { velocity_ = move; }
  Vector3D SetPosition(Vector3D p) { return position_ = p; }

  double GetMass() { return mass_; }
  double GetRadius() { return radius_; }
  Vector3D GetPosition() { return position_; }
  Vector3D GetVelocity() { return velocity_; }

 private:
  // meters
  Vector3D position_;
  Vector3D velocity_;  // movement per time_dx
  double radius_;
  // kg
  double mass_;
};

#endif  //  OPEN_MP_BODY_H_