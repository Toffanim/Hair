#if !defined(GAME_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator:  $
   $Notice: $
   ======================================================================== */

#define GAME_H

#include <map>
#include <utility>
#include <stdlib.h>
#include <iostream>
#include <cfloat>
#include <cstdint>
#include <cstdlib>
#include "glew/glew.h"
#include "../player/Player.h"
#include "../mesh/Model.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "../managers/Manager.h"
#include "GLFW/glfw3.h"
#include "stb/stb_image.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"
#include "../skybox/Skybox.h"

struct Times
{
    float startTime = 0.f;
    float previousTime = 0.f;
    float elapsedTime = 0.f;
    float deltaTime = 0.f;
    float globalTime = 0.f;    
};

class Game
{    
public :    
    Game(int width, int height);
    int init();
    void close();
    int mainLoop();
    void loadAssets();
    void drawScene( );
    void scene1(Player* p, Skybox* skybox, Times times);
    
private :
    GLFWwindow* window;
    bool endgame;
    int screenWidth;
    int screenHeight;
   
    void startupGLDiagnostics();
    void loadGeometry();
    void initGbuffer();
    void initFX();
    void initShadows();
    void loadShaders();
};

#endif
