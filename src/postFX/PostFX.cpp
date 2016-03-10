/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Marc TOFFANIN $
   $Notice: Given as-is without warranty AT ALL that it's working$
   ======================================================================== */
#include "PostFX.h"

namespace PostFX
{
    GLuint blur( Shader blur, GLuint texture, int amount)
    {
        static bool createAll = true;
        
        static GLuint pingpongFBO[2];
        static GLuint pingpongBuffer[2];
        static GLuint vao;
        static GLuint vbo[2];
        
        if(createAll)
        {
            int w, h;
            glBindTexture(GL_TEXTURE_2D, texture);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
            std::cout << w << h << std::endl;
            glGenFramebuffers(2, pingpongFBO);
            glGenTextures(2, pingpongBuffer);
            
            for (GLuint i = 0; i < 2; i++)
            {
                glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
                glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]);
                glTexImage2D(
                    GL_TEXTURE_2D, 0, GL_RGB16F, w, h, 0, GL_RGB, GL_FLOAT, NULL
                             );
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glFramebufferTexture2D(
                    GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffer[i], 0
                                       );
            }

            int   quad_triangleCount = 2;
            int   quad_triangleList[] = {0, 1, 2, 2, 1, 3}; 
            float quad_vertices[] =  {-1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, 1.0};
            
            // Quad
            glBindVertexArray(vao);
            // Bind indices and upload data
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[0]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_triangleList), quad_triangleList, GL_STATIC_DRAW);
            // Bind vertices and upload data
            glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*2, (void*)0);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);            
            createAll = false;
        }
        
        GLboolean horizontal = true, first_iteration = true;
        amount = amount + (amount%2);
        blur.use();
        for (GLuint i = 0; i < amount; i++)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]); 
            glUniform1i(glGetUniformLocation(blur.getProgram(), "horizontal"), horizontal);
            glBindTexture(
                GL_TEXTURE_2D, first_iteration ? texture : pingpongBuffer[!horizontal]
                          ); 
            glBindVertexArray(vao);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
            horizontal = !horizontal;
            if (first_iteration)
                first_iteration = false;
        }
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        Utils::checkGlError("blur");
        return pingpongBuffer[!horizontal];
    }
}
