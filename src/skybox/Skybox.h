#if !defined(SKYBOX_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Marc TOFFANIN $
   $Notice: Given as-is without warranty AT ALL that it's working$
   ======================================================================== */

#define SKYBOX_H
#include <vector>
#include "glew/glew.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "../shader/Shader.h"
#include "stb/stb_image.h"
#include <iostream>

class Skybox
{
public :
    Skybox( const std::vector< const GLchar*>& faces );
    ~Skybox();
    void display( glm::mat4 view, glm::mat4 proj, GLuint depth );
private :
    GLuint loadCubemap( const std::vector<const GLchar*>& faces );
    GLuint texture_id;
    GLuint VAO;
    GLuint VBO;
    Shader* shader;
};
#endif
