//
// Underworld Adventures - an Ultima Underworld hacking project
// Copyright (c) 2002,2003,2004,2019 Underworld Adventures Team
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
/// \file PhysicsModel.hpp
/// \brief game physics model
//
#pragma once

#include "Triangle3d.hpp"
#include <vector>

class PhysicsBody;

/// physics parameter
enum PhysicsParam
{
   /// controls if gravity is active in the physics model
   physicsGravity = 0,

   /// last param; not used
   physicsParamMax
};

/// physics model callback interface
class IPhysicsModelCallback
{
public:
   /// returns surrounding triangles on given position
   virtual void GetSurroundingTriangles(unsigned int xpos,
      unsigned int ypos, std::vector<Triangle3dTextured>& allTriangles) = 0;
};

/// physics model class
class PhysicsModel
{
public:
   /// ctor
   PhysicsModel()
      :m_callback(NULL)
   {
   }

   /// inits the physics model
   void Init(IPhysicsModelCallback* callback);

   bool GetPhysicsParam(PhysicsParam param) const
   {
      return m_params[param];
   }

   void SetPhysicsParam(PhysicsParam param, bool value)
   {
      m_params[param] = value;
   }

   /// evaluate physics on objects
   void EvaluatePhysics(double elapsedTime);

   /// tracks object movement for given body
   void TrackObject(PhysicsBody& body);

   /// add physics body to track to model
   void AddTrackBody(PhysicsBody* body)
   {
      m_trackedBodies.push_back(body);
   }

   /// removes physics body
   void RemoveTrackBody(PhysicsBody* body)
   {
      auto iter = std::find(m_trackedBodies.begin(), m_trackedBodies.end(), body);
      UaAssert(iter != m_trackedBodies.end());

      if (iter != m_trackedBodies.end())
         m_trackedBodies.erase(iter);
   }

private:
   /// model parameters
   bool m_params[physicsParamMax];

   /// callback interface pointer
   IPhysicsModelCallback* m_callback;

   /// list of pointer to bodies tracked by physics model
   std::vector<PhysicsBody*> m_trackedBodies;
};
