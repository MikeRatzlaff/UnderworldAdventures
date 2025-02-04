//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2006,2019,2021 Underworld Adventures Team
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
/// \file Base.hpp
/// \brief Base module includes
//
#pragma once

#include "common.hpp"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// compiler macros;
//   The following macros will be defined if the feature is present
//
/// \def HAVE_MSVC     Microsoft Visual C++ compiler
/// \def HAVE_GCC      Gnu gcc compiler
/// \def HAVE_WIN32    Win32 API is available through <windows.h>
/// \def HAVE_LINUX    Linux is used as target system
/// \def HAVE_MINGW    MinGW32 development/runtime environment
/// \def HAVE_MACOSX   MacOS X development/runtime environment

#ifdef _MSC_VER
#define HAVE_MSVC _MSC_VER
#define HAVE_WIN32
#endif

#if defined(UNICODE) || defined(_UNICODE)
#define HAVE_UNICODE
#endif

#ifdef __gcc__
#define HAVE_GCC
#endif

#ifdef __MINGW32__
#define HAVE_MINGW
#define HAVE_WIN32
#endif

#ifdef __linux__
#define HAVE_LINUX
#endif

#ifdef __MACH__
#define HAVE_MACOSX
#endif

#ifdef BEOS
#define HAVE_BEOS
#endif


/// checks for assert
void UaAssertCheck(bool cond, const char* cond_str, const char* message, const char* file, int line);

/// macro to check for conditions and pass source filename and line
#define UaAssert(cond) UaAssertCheck((cond), #cond, nullptr, __FILE__, __LINE__);

/// macro to check for conditions and pass message, source filename and line
#define UaAssertMsg(cond, message) UaAssertCheck((cond), #cond, message, __FILE__, __LINE__);

/// macro to verify expression/statement
#define UaVerify(cond) if (!(cond)) UaAssertCheck((cond), #cond, nullptr, __FILE__, __LINE__);


// trace messages

/// prints out a trace message (don't use directly, use UaTrace instead!)
int UaTracePrintf(const char *fmt, ...);

/// \def UaTrace
/// \brief debug output
/// Used to log text during the game. The text is printed on the console (the
/// program has to be built with console support to show the text).
///
/// The function has the same syntax as the printf function and uses the
/// UaTracePrintf() helper function. The function can be switched off
/// conditionally.
#if 1 //defined(_DEBUG) || defined(DEBUG)
# define UaTrace UaTracePrintf
#else
# define UaTrace true ? 0 : UaTracePrintf
#endif

#include "Exception.hpp"
#include "String.hpp"
#include <memory>
#include <SDL_types.h>

struct SDL_RWops;

/// \brief Base classes namespace
/// \details Contains all base classes, functions and types used in uwadv.
namespace Base
{
   /// smart pointer to SDL_RWops struct
   typedef std::shared_ptr<SDL_RWops> SDL_RWopsPtr;

   /// creates SDL_RWops shared ptr from pointer
   SDL_RWopsPtr MakeRWopsPtr(SDL_RWops* rwops);

} // namespace Base
