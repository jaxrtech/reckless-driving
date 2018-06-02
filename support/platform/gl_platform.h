#pragma once

#if defined(__APPLE__)

#include <gl.h>

#elif defined(_WIN32)

#include "windows_gl.h"
#include <gl/gl.h>
#include <gl/glu.h>

#endif