/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2015 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */
#include "Utils.h"

//Utils namespace for static functions
namespace Utils {
    using namespace std;
    
    //read text file for shaders
    const char *textFileRead( const char *fn )
    {
        /* Note: the `fatalError' thing is a bit of a hack, The proper course of
         * action would be to have people check the return values of textFileRead,
         * but lets avoid cluttering the lab-code even further.
         */

        FILE *fp;
        char *content = NULL;
        int count = 0;

        if( fn != NULL ) 
        {
            fp = fopen( fn, "rt" );
            if( fp != NULL ) 
            {
                fseek( fp, 0, SEEK_END );
                count = ftell( fp );
                fseek( fp, 0, SEEK_SET );

                if( count > 0 ) 
                {
                    content = new char[count+1];
                    count = fread( content, sizeof(char), count, fp );
                    content[count] = '\0';
                }
                else
                {
                    printf("Error : file empty");
                }

                fclose( fp );
            }
            else
            {
                printf("Error : Cant read file");
            }
        }
        else
        {
            printf("Error : NULL arg");
        }

        return content;
    }

//Is there on or more error since last call ?
    bool checkGlError(const char* title)
    {
        int error;
        if((error = glGetError()) != GL_NO_ERROR)
        {
            std::string errorString;
            switch(error)
            {
                case GL_INVALID_ENUM:
                    errorString = "GL_INVALID_ENUM";
                    break;
                case GL_INVALID_VALUE:
                    errorString = "GL_INVALID_VALUE";
                    break;
                case GL_INVALID_OPERATION:
                    errorString = "GL_INVALID_OPERATION";
                    break;
                case GL_INVALID_FRAMEBUFFER_OPERATION:
                    errorString = "GL_INVALID_FRAMEBUFFER_OPERATION";
                    break;
                case GL_OUT_OF_MEMORY:
                    errorString = "GL_OUT_OF_MEMORY";
                    break;
                default:
                    errorString = "UNKNOWN";
                    break;
            }
            fprintf(stdout, "OpenGL Error(%s): %s\n", errorString.c_str(), title);
        }
        return error == GL_NO_ERROR;
    }


};
