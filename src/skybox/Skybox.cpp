/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Marc TOFFANIN $
   $Notice: Given as-is without warranty AT ALL that it's working$
   ======================================================================== */
#include "Skybox.h"
using namespace std;

Skybox::Skybox( const vector<const GLchar*>& faces )
{

    GLfloat vertices[] = {
        // Positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
  
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
  
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
   
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
  
        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
  
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
    };    

    shader = new Shader("Skybox shader");
    shader->attach(GL_VERTEX_SHADER, "assets/shaders/skybox.vert");
    shader->attach(GL_FRAGMENT_SHADER, "assets/shaders/skybox.frag");
    shader->link();
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glBindVertexArray(0);
    
    texture_id = loadCubemap(faces);

    Utils::checkGlError("Load skybox");
}

Skybox::~Skybox()
{
    glDeleteTextures(1, &texture_id);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    delete shader;
}


// Loads a cubemap texture from 6 individual texture faces
// Order should be:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front)
// -Z (back)
GLuint Skybox::loadCubemap(const vector<const GLchar*>& faces)
{
    GLuint textureID;
    glGenTextures(1, &textureID);

    int x,y,comp;
    unsigned char* image;
    
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    for(GLuint i = 0; i < faces.size(); i++)
    {
        image = stbi_load(faces[i], &x, &y, &comp, STBI_rgb);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB8, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        stbi_image_free(image);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    Utils::checkGlError("Load cubemap");

    return textureID;
}


void Skybox::display( glm::mat4 view, glm::mat4 proj, GLuint depth )
{
    //glDisable(GL_DEPTH_TEST);
        shader->use();             
        glUniformMatrix4fv(glGetUniformLocation(shader->getProgram(), "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shader->getProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(proj));
        //glUniform3f(glGetUniformLocation(shader->getProgram(), "sun"), sunDir.x, sunDir.y, sunDir.z);
        // skybox cube
        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(shader->getProgram(), "skybox"), 0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
        glActiveTexture(GL_TEXTURE1);
        glUniform1i(glGetUniformLocation(shader->getProgram(), "DepthBuffer"), 1);
        glBindTexture(GL_TEXTURE_2D, depth);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // Set depth function back to default
        shader->unuse();
        Utils::checkGlError("Display skybox");
}
