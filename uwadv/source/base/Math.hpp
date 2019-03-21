//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2006,2019 Michael Fink
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
/// \file Math.hpp
/// \brief math stuff
//
// Note: all angle params should be passed to the functions in degrees, not
// radians.
//
#pragma once

#include <cmath>

#ifdef _MSC_VER
// pragmas for Visual C++
#pragma inline_depth( 255 )
#pragma inline_recursion( on )
#pragma auto_inline( on )
#endif

/// pi constant
const double c_pi = 3.141592653589793;

/// converts angle from degree values (0�..360�) to radians (0..2pi)
#define Deg2rad(a) ((double(a) / 180.0) * c_pi)
/// converts angle from randians values to degrees
#define Rad2deg(a) ((double(a) / c_pi) * 180.0)

/// \brief 2D vector class
/// The class represents a 2 dimensional vector with x and y coordinates.
class Vector2d
{
public:
   double x; ///< vector x element
   double y; ///< vector y element

public:
   /// ctor
   Vector2d()
      :x(0.0),
      y(0.0)
   {
   }

   /// copy ctor
   Vector2d(const Vector2d& vec)
      :x(vec.x),
      y(vec.y)
   {
   }

   /// move ctor
   Vector2d(const Vector2d&& vec)
      :x(vec.x),
      y(vec.y)
   {
   }

   /// ctor with x and y coordinates
   Vector2d(double _x, double _y)
      :x(_x),
      y(_y)
   {
   }

   // operators

   /// assign operator
   Vector2d& operator=(const Vector2d& vec)
   {
      if (this == &vec)
         return *this;

      x = vec.x;
      y = vec.y;
      return *this;
   }

   /// move assign operator
   Vector2d& operator=(const Vector2d&& vec)
   {
      if (this == &vec)
         return *this;

      x = vec.x;
      y = vec.y;
      return *this;
   }

   /// add assignment
   Vector2d& operator+=(const Vector2d& vec)
   {
      x += vec.x;
      y += vec.y;
      return *this;
   }

   /// subtract assignment
   Vector2d& operator-=(const Vector2d& vec)
   {
      x -= vec.x;
      y -= vec.y;
      return *this;
   }

   /// multiplication assignment
   Vector2d& operator*=(double scalar)
   {
      x *= scalar;
      y *= scalar;
      return *this;
   }

   // operations

   /// sets vector by giving vector elements
   void Set(double _x, double _y)
   {
      x = _x;
      y = _y;
   }

   /// sets vector by giving two vectors
   void Set(const Vector2d& v1, const Vector2d& v2)
   {
      x = v2.x - v1.x;
      y = v2.y - v1.y;
   }

   /// sets vector by giving polar coordinates (angle in degree, not radians)
   void SetPolar(double length, double angle)
   {
      x = length * cos(Deg2rad(angle));
      y = length * sin(Deg2rad(angle));
   }


   /// returns length of vector
   double Length() const
   {
      return sqrt(x * x + y * y);
   }

   /// normalizes vector
   void Normalize()
   {
      operator*=(1.0 / Length());
   }

   /// calculates the inner (dot) product of this and another vector
   double Dot(const Vector2d& vec) const
   {
      return x * vec.x + y * vec.y;
   }

   /// rotates vector; angle in degree
   void Rotate(double angle)
   {
      double temp = x * cos(Deg2rad(angle)) - y * sin(Deg2rad(angle));
      y = x * sin(Deg2rad(angle)) + y * cos(Deg2rad(angle));
      x = temp;
   }
};

// free functions

/// unary minus
inline Vector2d operator-(const Vector2d& vec)
{
   return Vector2d(-vec.x, -vec.y);
};

/// add operator
inline Vector2d operator+(const Vector2d& v1, const Vector2d& v2)
{
   return Vector2d(v1.x + v2.x, v1.y + v2.y);
}

/// subtract operator
inline Vector2d operator-(const Vector2d& v1, const Vector2d& v2)
{
   return Vector2d(v1.x - v2.x, v1.y - v2.y);
}

/// scalar multiplication, vector * scalar
inline Vector2d operator*(const Vector2d& vec, const double scalar)
{
   Vector2d ret(vec);
   ret *= scalar;
   return ret;
}

/// scalar multiplication, scalar * vector
inline Vector2d operator*(const double scalar, const Vector2d& vec)
{
   Vector2d ret(vec);
   ret *= scalar;
   return ret;
}

/// division by scalar
inline Vector2d operator/(const Vector2d& vec, const double scalar)
{
   return Vector2d(vec.x / scalar, vec.y / scalar);
}

/// \brief 3D vector class
/// The class represents a 3 dimensional vector with x, y and z coordinates.
/// It can also represent a point in 3d space, depending on use.
class Vector3d
{
public:
   double x; ///< vector x element
   double y; ///< vector y element
   double z; ///< vector z element

public:
   /// ctor
   Vector3d()
      :x(0.0),
      y(0.0),
      z(0.0)
   {
   }

   /// copy ctor
   Vector3d(const Vector3d& vec)
      :x(vec.x),
      y(vec.y),
      z(vec.z)
   {
   }

   /// move ctor
   Vector3d(const Vector3d&& vec)
      :x(vec.x),
      y(vec.y),
      z(vec.z)
   {
   }

   /// ctor with x, y and z coordinates
   Vector3d(double _x, double _y, double _z)
      :x(_x),
      y(_y),
      z(_z)
   {
   }

   // operators

   /// assignment operator
   Vector3d& operator=(const Vector3d& vec)
   {
      if (this == &vec)
         return *this;

      x = vec.x;
      y = vec.y;
      z = vec.z;
      return *this;
   }

   /// move assignment operator
   Vector3d& operator=(const Vector3d&& vec)
   {
      if (this == &vec)
         return *this;

      x = vec.x;
      y = vec.y;
      z = vec.z;
      return *this;
   }

   /// add assignment
   Vector3d& operator+=(const Vector3d& vec)
   {
      x += vec.x;
      y += vec.y;
      z += vec.z;
      return *this;
   }

   /// subtract assignment
   Vector3d& operator-=(const Vector3d& vec)
   {
      x -= vec.x;
      y -= vec.y;
      z -= vec.z;
      return *this;
   }

   /// multiplication assignment
   Vector3d& operator*=(double scalar)
   {
      x *= scalar;
      y *= scalar;
      z *= scalar;
      return *this;
   }


   /// multiplication assignment with vector (element-wise multiplication)
   Vector3d& operator*=(const Vector3d& vec)
   {
      x *= vec.x;
      y *= vec.y;
      z *= vec.z;
      return *this;
   }

   /// division assignment with vector (element-wise division)
   Vector3d& operator/=(const Vector3d& vec)
   {
      x /= vec.x;
      y /= vec.y;
      z /= vec.z;
      return *this;
   }

   // operations

   /// sets vector by giving vector elements
   void Set(double _x, double _y, double _z)
   {
      x = _x;
      y = _y;
      z = _z;
   }

   /// returns length of vector
   double Length() const
   {
      return sqrt(x * x + y * y + z * z);
   }

   /// normalizes vector
   void Normalize()
   {
      operator*=(1.0 / Length());
   }

   /// calculates inner (dot) product
   double Dot(const Vector3d& vec) const
   {
      return x * vec.x + y * vec.y + z * vec.z;
   }

   /// calculates outer (cross) product
   void Cross(const Vector3d& first, const Vector3d& second)
   {
      Set(first.y * second.z - first.z * second.y,
         first.z * second.x - first.x * second.z,
         first.x * second.y - first.y * second.x);
   }

   /// rotate vector around x axis
   void RotateX(double angle)
   {
      double y_temp = y, angle_rad = Deg2rad(angle);
      y = y * cos(angle_rad) - z * sin(angle_rad);
      z = y_temp * sin(angle_rad) + z * cos(angle_rad);
   }

   /// rotate vector around y axis
   void RotateY(double angle)
   {
      double x_temp = x, angle_rad = Deg2rad(angle);
      x = x * cos(angle_rad) + z * sin(angle_rad);
      z = -x_temp * sin(angle_rad) + z * cos(angle_rad);
   }

   /// rotate vector around z axis
   void RotateZ(double angle)
   {
      double x_temp = x, angle_rad = Deg2rad(angle);
      x = x * cos(angle_rad) - y * sin(angle_rad);
      y = x_temp * sin(angle_rad) + y * cos(angle_rad);
   }

   /// \brief rotates vector around axis
   /// \details calculates rotated vector using the rotation matrix given at
   ///http://www.makegames.com/3drotation/
   void Rotate(const Vector3d& axis, double angle)
   {
      double c = cos(Deg2rad(angle));
      double s = sin(Deg2rad(angle));
      double t = 1 - c;

      double ax = axis.x, ay = axis.y, az = axis.z;

      Set((t*ax*ax + c)*x + (t*ax*ay - s * az)*y + (t*ax*az + s * ay)*z,
         (t*ax*ay + s * az)*x + (t*ay*ay + c)*y + (t*ay*az - s * ax)*z,
         (t*ax*az - s * ay)*x + (t*ay*az + s * ax)*y + (t*az*az + c)*z);
   }
};

// free functions

/// unary minus
inline Vector3d operator-(const Vector3d& vec)
{
   return Vector3d(-vec.x, -vec.y, -vec.z);
};

/// add operator
inline Vector3d operator+(const Vector3d& v1, const Vector3d& v2)
{
   return Vector3d(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

/// subtract operator
inline Vector3d operator-(const Vector3d& v1, const Vector3d& v2)
{
   return Vector3d(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

/// scalar multiplication, vector * scalar
inline Vector3d operator*(const Vector3d& vec, const double scalar)
{
   Vector3d ret(vec);
   ret *= scalar;
   return ret;
}

/// scalar multiplication, scalar * vector
inline Vector3d operator*(const double scalar, const Vector3d& vec)
{
   Vector3d ret(vec);
   ret *= scalar;
   return ret;
}

/// division by scalar
inline Vector3d operator/(const Vector3d& vec, const double scalar)
{
   return Vector3d(vec.x / scalar, vec.y / scalar, vec.z / scalar);
}

/// plane in 3d
class Plane3d
{
public:
   /// ctor; takes origin and normal vector
   Plane3d(const Vector3d& the_origin, const Vector3d& the_normal)
      :origin(the_origin), normal(the_normal)
   {
      equation_3 = -normal.Dot(origin);
   }

   /// ctor; constructs plane from triangle
   Plane3d(Vector3d p1, Vector3d p2, Vector3d p3)
      : origin(p1)
   {
      p2 -= p1;
      p3 -= p1;
      normal.Cross(p2, p3);
      normal.Normalize();
      equation_3 = -normal.Dot(origin);
   }

   /// calculates if plane is front-facing to given direction vector
   bool IsFrontFacingTo(const Vector3d& direction) const
   {
      double dot = normal.Dot(direction);
      return dot <= 0.0;
   }

   /// calculates signed distance to plane
   double SignedDistanceTo(const Vector3d& point) const
   {
      return point.Dot(normal) + equation_3;
   }

   Vector3d origin; ///< plane origin
   Vector3d normal; ///< plane normal
   double equation_3;  ///< 3rd plane equation param
};
