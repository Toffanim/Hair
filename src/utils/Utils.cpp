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

    //ambiguous call to the std func
    //that's why i made it
    float abs(float a)
    {
        return a>=0?a:-a; 
    }
    float absmin( float a, float b)
    {
        float aabs = abs(a);
        float babs = abs(b);
        return aabs <= babs ? a : b;
    }

    #if 0
    //Load a texture from file
    GLuint TextureFromFile( const char* path, string directory)
    {
        //Generate texture ID and load texture data 
        string filename = string(path);
        filename = directory + '/' + filename;
        GLuint textureID;
        glGenTextures(1, &textureID);
        SDL_Surface *image = IMG_Load(filename.c_str());
        if(!image)
        {
            cout << "IMG_Load : " << IMG_GetError() << endl;
            return(0);
        }
        // Format de l'image
        GLenum formatInterne(0);
        GLenum format(0);
        // Détermination du format et du format interne pour les images à 3 composantes
        if(image->format->BytesPerPixel == 3)
        {
            // Format interne
            formatInterne = GL_RGB;
            // Format
            if(image->format->Rmask == 0xff)
                format = GL_RGB;
            else
                format = GL_BGR;
        }
        // Détermination du format et du format interne pour les images à 4 composantes
        else if(image->format->BytesPerPixel == 4)
        {    
            // Format interne
            formatInterne = GL_RGBA;
            // Format
            if(image->format->Rmask == 0xff)
                format = GL_RGBA;
            else
                format = GL_BGRA;
        }
        // Assign texture to ID
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, formatInterne, image->w, image->h, 0, format, GL_UNSIGNED_BYTE, image->pixels);
        glGenerateMipmap(GL_TEXTURE_2D);    

        // Parameters
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
        SDL_FreeSurface(image);
        return textureID;
    }
    #endif
    
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

    //AABB to AABB collision check
    bool AABBtoAABB(const AABB& tBox1, const AABB& tBox2)
    {
        return(tBox1.max.x > tBox2.min.x &&
               tBox1.min.x < tBox2.max.x &&
               tBox1.max.y > tBox2.min.y &&
               tBox1.min.y < tBox2.max.y &&
               tBox1.max.z > tBox2.min.z &&
               tBox1.min.z < tBox2.max.z);
    }

    //Check if c1 == c2
    bool isColorEquals( glm::vec3 c1, glm::vec3 c2)
    {
        if ( c1.x == c2.x && c1.y == c2.y && c1.z == c2.z)
            return (true);
        return (false);
    }

    //Function for reading ppms, based on http://josiahmanson.com/prose/optimize_ppm/
    // very interesting to read btw
    void eat_comment(ifstream &f)
    {
        char linebuf[1024];
        char ppp;
        while (ppp = f.peek(), ppp == '\n' || ppp == '\r')
            f.get();
        if (ppp == '#')
            f.getline(linebuf, 1023);
    }

    void load_ppm(ImageRGB &img, const string &name)
    {
        ifstream f(name.c_str(), ios::binary);
        if (f.fail())
        {
            cout << "Could not open file: " << name << endl;
            return;
        }

        // get type of file
        eat_comment(f);
        int mode = 0;
        string s;
        f >> s;
        if (s == "P3")
            mode = 3;
        else if (s == "P6")
            mode = 6;
    
        // get w
        eat_comment(f);
        f >> img.w;

        // get h
        eat_comment(f);
        f >> img.h;
    
        // get bits
        eat_comment(f);
        int bits = 0;
        f >> bits;
    
        // error checking
        if (mode != 3 && mode != 6)
        {
            cout << "Unsupported magic number" << endl;
            f.close();
            return;
        }
        if (img.w < 1)
        {
            cout << "Unsupported width: " << img.w << endl;
            f.close();
            return;
        }
        if (img.h < 1)
        {
            cout << "Unsupported height: " << img.h << endl;
            f.close();
            return;
        }
        if (bits < 1 || bits > 255)
        {
            cout << "Unsupported number of bits: " << bits << endl;
            f.close();
            return;
        }

        // load image data
        img.data.resize(img.w * img.h);

        if (mode == 6)
        {
            f.get();
            f.read((char*)&img.data[0], img.data.size() * 3);
        }
        else if (mode == 3)
        {
            for (unsigned int i = 0; i < img.data.size(); i++)
            {
                int v;
                f >> v;
                img.data[i].r = v;
                f >> v;
                img.data[i].g = v;
                f >> v;
                img.data[i].b = v;
            }
        }

        // close file
        f.close();
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
