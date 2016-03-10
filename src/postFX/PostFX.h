#if !defined(POSTFX_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Marc TOFFANIN $
   $Notice: Given as-is without warranty AT ALL that it's working$
   ======================================================================== */

#define POSTFX_H

#include "glew/glew.h"
#include "../shader/Shader.h"
#include "../utils/Utils.h"
namespace PostFX
{
    GLuint blur(Shader s, GLuint texture, int amount);    
}
#endif
