//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2021 Underworld Adventures Team
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
/// \file ImageScreen.hpp
/// \brief screen that provides a 320x200 image to draw to
//
#pragma once

#include "Screen.hpp"
#include "ImageQuad.hpp"
#include "FadingHelper.hpp"

class ImageScreen : public Screen
{
public:
   /// ctor
   ImageScreen(IGame& gameInterface, unsigned int paletteIndex, double fadeInOutTime);
   /// dtor
   virtual ~ImageScreen() noexcept {}

   /// called when image screen started fade out
   virtual void OnFadeOutStarted();

   /// called when image screen has been faded out; calls RemoveScreen() by default
   virtual void OnFadeOutEnded();

   // virtual functions from Screen
   virtual void Init() override;
   virtual void Draw() override;
   virtual void Tick() override;

protected:
   // methods usable to ImageScreen derived classes

   /// returns the indexed image that is used in the screen
   IndexedImage& GetImage();

   /// updates image on display, when indexed image was modified
   void UpdateImage();

   /// starts fading out the screen
   void StartFadeout();

private:
   /// the actual 320x200 image quad to display
   ImageQuad m_image;

   /// time to fade in/out
   const double m_fadeInOutTime;

   /// fade in/out state
   unsigned int m_fadeState;

   /// fading helper
   FadingHelper m_fader;
};
