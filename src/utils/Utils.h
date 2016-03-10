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
#include "../mesh/AABB.h"

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
    bool isColorEquals( glm::vec3 c1, glm::vec3 c2);
    const char* textFileRead( const char *fn );
    GLuint TextureFromFile( const char* path, std::string directory);
    float absmin( float a, float b);
    void load_ppm(ImageRGB &img, const std::string &name);
    bool AABBtoAABB(const AABB& tBox1, const AABB& tBox2);
};
#endif
