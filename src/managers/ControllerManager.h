#if !defined(CONTROLLERMANAGER_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Marc TOFFANIN $
   $Notice: Given as-is without warranty AT ALL that it's working$
   ======================================================================== */

#define CONTROLLERMANAGER_H
#include "../controller/Controller.h"
#include "GLFW/glfw3.h"

#include <vector>

class ControllerManager
{
public :
    static ControllerManager& getInstance()
    {
        static ControllerManager instance;
        return instance;
    }

    void add( Controller* c )
    {
        controllers.push_back(c);
    }

    void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        for( std::vector<Controller*>::iterator it = controllers.begin();
             it != controllers.end();
             ++it)
        {
            (*it)->processKey( key, action );
        }
    }

    void mouseCursorCallback(GLFWwindow* window, double xpos, double ypos)
    {
        for( std::vector<Controller*>::iterator it = controllers.begin();
             it != controllers.end();
             ++it)
        {
            (*it)->processCursor( xpos, ypos );
        }
    }

    void mouseCallback(GLFWwindow* window, int button, int action, int mods)
    {
        for( std::vector<Controller*>::iterator it = controllers.begin();
             it != controllers.end();
             ++it)
        {
            (*it)->processMouse( button, action );
        }
    }

private:
    ControllerManager();
    ControllerManager(ControllerManager const&) = delete;
    void operator=(ControllerManager const&) = delete;
    std::vector<Controller*> controllers;
};

#endif
