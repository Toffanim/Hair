/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator:  $
   $Notice:  $
   ======================================================================== */
#include "Controller.h"
//The controller is set to control callback from pressed/released key, etc
//Generally has to be bind to a player, but can also be bind to a monster
//to control IA for example

using namespace std;

Controller::Controller()
{
}

void Controller::processKey( int key, int action )
{;
    const auto & func = mappingKey[ {key, action} ];
    if(func)
    {
        func();
    }
}

void Controller::processCursor( double xpos, double ypos )
{
    if(cursorEvent)
        cursorEvent(xpos, ypos);
}

void Controller::processMouse( int button, int action )
{
    
}

void Controller::addKeyListener( std::pair< int, int > key_action, const std::function<void(void)> &func )
{
    mappingKey[ key_action ] = func;
}

void Controller::addMouseListener( std::pair< int, int > button_action, const std::function<void(void)> &func)
{
}

void Controller::addCursorListener( const std::function<void(double, double)> &func)
{
    cursorEvent = func;
}


#if 0
bool Controller::onKeyPress(SDL_Event e)
{

    SDL_Keycode code = e.key.keysym.sym;
    const auto & pressFunc = mappingKeyPress[code];
    if(pressFunc)
    {
        pressFunc();
        return(true);
    }
    return(false);
}

bool Controller::onMiscCallback(unsigned char e)
{
    const auto & func = mappingMisc[e];
    if(func)
    {
        func();
        return(true);
    }
    return(false);
}

void Controller::setMiscCallback(unsigned char code, const std::function<void(void)> &func)
{
    mappingMisc[code] = func;
}

bool Controller::onKeyRelease(SDL_Event e)
{

    SDL_Keycode code = e.key.keysym.sym;
    const auto & releaseFunc = mappingKeyRelease[code];
    if(releaseFunc)
    {
        releaseFunc();
        return(true);
    }
    return(false);
}

void Controller::setKeyReleaseCallback( SDL_Keycode code, const std::function<void(void)> &func)
{
    mappingKeyRelease[code] = func;
}

void Controller::setKeyPressCallback( SDL_Keycode code, const std::function<void(void)> &func)
{
    mappingKeyPress[code] = func;
}

void Controller::setMouseCallback( std::function<void(int, int)> &func )
{
    mouseEvent = func;
}

//Call every frame

#endif
