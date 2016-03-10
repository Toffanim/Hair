/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2015 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */
#include "Shader.h"
using namespace std;

//Shader encapsulation
Shader::Shader(string name):name(name)
{
}

Shader::~Shader()
{
    for (int i=0;i<handles.size();i++)
    {
        glDetachShader(program_handle, handles[i]);
        glDeleteShader(handles[i]);
    }
    glDeleteProgram(program_handle);
}

void Shader::use()
{
    glUseProgram(program_handle);
}

void Shader::unuse()
{
    glUseProgram(0);
}

void Shader::attach(GLuint shader)
{
    handles.push_back(shader);
}

void Shader::attach(int type, const char* filename)
{
    char* mem=read_file(filename);
    GLuint handle = glCreateShader(type);
    glShaderSource(handle, 1, (const GLchar**)(&mem), 0);
    glCompileShader(handle);

    GLint compileSuccess=0;

    glGetShaderiv(handle, GL_COMPILE_STATUS, &compileSuccess);
    if(!compileSuccess)
    {
        int logLength;
        glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &logLength);
        char * log = new char[logLength];
        glGetShaderInfoLog(handle, logLength, &logLength, log);
        printf("Shader %s\n%s\ncompileSuccess=%d\n",filename,log,compileSuccess);
        delete[] log;
        return;
    }
    handles.push_back(handle);
}

void Shader::link()
{
    program_handle = glCreateProgram();
    for (int i=0;i<handles.size();i++)
    {
        glAttachShader(program_handle, handles[i]);
    }
    glLinkProgram(program_handle);

    GLint linkSuccess;
    glGetProgramiv(program_handle, GL_LINK_STATUS, &linkSuccess);
    if(!linkSuccess)
    {
        int logLength;
        glGetProgramiv(program_handle, GL_INFO_LOG_LENGTH, &logLength);
        char* log = new char[logLength];
        glGetProgramInfoLog(program_handle,logLength, &logLength, log);
        printf("Shader Linker:\n%s\nlinkSuccess=%d\n",log,linkSuccess);
        delete[] log;
        return;
    }
    printf("%s linked successful\n",name.c_str());
    
}


char* Shader::read_file(const char* name)
{
    FILE * fp = fopen (name, "rb");
        
    if (fp==0) 
    {
        printf ("File %s NOT FOUND\n", name);
        while(1);;      
    }
    fseek(fp, 0L, SEEK_END);
    int fsize = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    char* mem=(char*)malloc(fsize+1);
    for(int i=0;i<fsize+1;i++)mem[i]=0;
    fread (mem, 1, fsize, fp);
    fclose (fp);
    return mem;
}
