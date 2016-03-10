#if !defined(SHADER_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator:  $
   $Notice: $
   ======================================================================== */

#define SHADER_H

#include <iostream>
#include <stdio.h>
#include <sstream>
#include <string>
#include <map>
#include <GL/glew.h>
#include "../utils/Utils.h"

class Shader
{
public:
    Shader(std::string name);
    ~Shader();
    void attach( GLuint shader );
    void attach( int type, const char* filename);
    void link();
    void unuse();
    GLuint getProgram() { return(program_handle); }
    void use();
    GLuint operator[](int idx){ return(handles[idx]);}
private :
    std::vector<GLuint> handles;
    GLuint program_handle;
    std::string name;

    char* read_file(const char* name);
    
};

#endif
