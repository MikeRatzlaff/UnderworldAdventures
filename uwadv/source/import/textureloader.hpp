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
/// \file textureloader.hpp
/// \brief texture loader class
//
#pragma once

#include "indexedimage.hpp"

namespace Import
{
   /// texture loader
   class TextureLoader
   {
   public:
      /// loads texture images
      static void LoadTextures(
         std::vector<IndexedImage>& textureImages,
         unsigned int startIndex, const char* textureName,
         Palette256Ptr palette);
   };

} // namespace Import