#if !defined(UTILS_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator:  $
   $Notice:  $
   ======================================================================== */

#define UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "glew/glew.h"
#include "glm/glm.hpp"

namespace Utils
{
    struct RGB
    {
        unsigned char r, g, b;
    };

    struct ImageRGB
    {
        int w, h;
        std::vector<RGB> data;
    };
    bool checkGlError(const char* title );
    const char* textFileRead( const char *fn );
};
#endif
