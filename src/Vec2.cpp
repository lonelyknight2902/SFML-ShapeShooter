#include "Vec2.h"
#include <math.h>
#include <iostream>

// Vec2::x = 0;
// Vec2::y = 0;

Vec2::Vec2() {

}

Vec2::Vec2(float xin, float yin)
  : x(xin), y(yin)
{

}

bool Vec2::operator == (const Vec2& rhs) const
{
  return x == rhs.x && y == rhs.y;
}

bool Vec2::operator != (const Vec2& rhs) const
{
  return x != rhs.x || y != rhs.y;
}

Vec2 Vec2::operator + (const Vec2& rhs) const
{
  return Vec2(x + rhs.x, y + rhs.y);
}

Vec2 Vec2::operator - (const Vec2& rhs) const
{
  return Vec2(x - rhs.x, y - rhs.y);
}

Vec2 Vec2::operator * (const float val) const
{
  return Vec2(x * val, y * val);
}

Vec2 Vec2::operator / (const float val) const
{
  return Vec2(x / val, y / val);
}

void Vec2::operator += (const Vec2& rhs) {
  x += rhs.x;
  y += rhs.y;
}

void Vec2::operator -= (const Vec2& rhs) {
  x -= rhs.x;
  y -= rhs.y;
}

void Vec2::operator *= (const float val) {
  x *= val;
  y *= val;
}

void Vec2::operator /= (const float val) {
  x /= val;
  y /= val;
}

// Vec2& Vec2::add(const Vec2& v) {
//   x += v.x;
//   y += v.y;
//   return *this;
// }

// Vec2& Vec2::scale(float s) {
//   x *= s;
//   y *= s;
//   return *this;
// }

// Vec2& Vec2::rotate(float deg) {
//   return *this;
// }

float Vec2::dist(const Vec2& v) const {
  return 0;
}

float Vec2::length() const {
  return std::sqrt(x * x + y * y);
}

void Vec2::normalize() {
  float l = length();
  if(l != 0)
  {
    *this /= l;
  }
}