#ifndef _GLOBALS_
#define _GLOBALS_

// Warning disabled ---
#pragma warning( disable : 4577 ) // Warning that exceptions are disabled
#pragma warning( disable : 4530 ) // Warning that exceptions are disabled
// -- Global Headers? --
#include <stdio.h>

// -- Globals for External Tools --
#include "LogHandler.h"
#include "LWTimer.h"
#include "SimpleTasker.h"


// -- Global Defines --
#define CAP(n) ((n <= 0.0f) ? n=0.0f : (n >= 1.0f) ? n=1.0f : n=n)

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f

// -- Global TypeDefs --
typedef uint32_t uint32;
typedef uint16_t uint16;

#endif 