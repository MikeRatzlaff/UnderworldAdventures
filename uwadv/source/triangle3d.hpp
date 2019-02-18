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
/// \file triangle3d.hpp
/// \brief triangle class
//
#pragma once

#include "uamath.hpp"

/// \brief vertex in 3d space
/// Represents a vertex in 3d space, which can form a triangle. The vertex
/// additionally has texture coordinates associated with them.
struct ua_vertex3d
{
   /// struct ctor
   ua_vertex3d()
      :u(0.0),
      v(0.0)
   {
   }

   /// vertex position
   ua_vector3d pos;

   /// texture coordinates
   double u, v;
};

/// \brief textured triangle
/// Represents a textured triangle made up with 3 ua_vertex3d objects. There
/// also is a texnum member to specify used texture.
struct ua_triangle3d_textured
{
   /// stock texture number used
   Uint16 texnum;

   /// vertices
   ua_vertex3d vertices[3];

   /// sets triangle point properties
   void set(unsigned int point, double x, double y, double z, double u, double v)
   {
      vertices[point].pos.set(x, y, z); vertices[point].u = u; vertices[point].v = v;
   }

   /// compare operator for std::sort
   bool operator<(const ua_triangle3d_textured& tri) const
   {
      return texnum > tri.texnum;
   }
};
