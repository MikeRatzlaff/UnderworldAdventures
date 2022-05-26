//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2019,2021,2022 Underworld Adventures Team
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
/// \file IngamePowerGem.cpp
/// \brief ingame power gem control
//
#include "pch.hpp"
#include "IngamePowerGem.hpp"
#include "../OriginalIngameScreen.hpp"
#include "GameLogic.hpp"
#include "ImageManager.hpp"

void IngamePowerGem::Init(IGame& game, unsigned int xpos,
   unsigned int ypos)
{
   m_attackMode = false;

   game.GetImageManager().LoadList(m_powerGemImages, "power");

   GetImage().Create(31, 12);
   GetImage().SetPalette(game.GetImageManager().GetPalette(0));

   ImageQuad::Init(game, xpos, ypos);
}

void IngamePowerGem::UpdateGem()
{
   unsigned int frame = 0;

   unsigned int power =
      m_parent->GetGameInterface().GetGameLogic().GetAttackPower();

   if (power > 0)
   {
      if (power >= 100)
         frame = (m_maxPowerFrameIndex & 3) + 10;
      else
      {
         frame = (unsigned(power / 100.0 * 8.0) % 9) + 1;
         m_maxPowerFrameIndex = 0;
      }
   }

   unsigned int dest = m_hasBorder ? 1 : 0;
   GetImage().PasteImage(m_powerGemImages[frame], dest, dest);

   Update();
}

bool IngamePowerGem::MouseEvent(bool buttonClicked, bool leftButton,
   bool buttonDown, unsigned int mouseX, unsigned int mouseY)
{
   return false;
}

void IngamePowerGem::Tick()
{
   /*
      // check if we have to update
      unsigned int power =
         m_parent->GetGameInterface().GetUnderworld().GetAttackPower();
      if (power >= 100)
      {
         if (++m_maxPowerFrameIndex>3)
            m_maxPowerFrameIndex = 0;
         UpdateGem();
      }
   */
}
