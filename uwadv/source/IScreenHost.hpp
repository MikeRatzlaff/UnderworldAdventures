//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2023 Underworld Adventures Team
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
/// \file IScreenHost.hpp
/// \brief screen host interface
//
#pragma once

/// interface to a scren host, managing IScreen instances
class IScreenHost
{
public:
   virtual ~IScreenHost() noexcept {}

   /// replaces current screen with new one; saves current on a screen stack when selected
   virtual void ReplaceScreen(Screen* newScreen, bool saveCurrent) = 0;

   /// removes current screen at next event processing
   virtual void RemoveScreen() = 0;
};
