#if !defined(CONTROLLER_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator:  $
   $Notice: $
   ======================================================================== */

#define CONTROLLER_H

#include <vector>
#include <functional>
#include <map>
#include <iostream>

class Controller
{
public:
    Controller();
   
    void processKey( int key, int action );
    void processCursor( double xpos, double ypos );
    void processMouse( int button, int action );

    void addKeyListener( std::pair< int, int > key_action, const std::function<void(void)> &func );
    void addMouseListener( std::pair< int, int > button_action, const std::function<void(void)> &func);
    void addCursorListener( const std::function<void(double, double)> &func);
#if 0
    void setKeyCallback(SDL_Keycode code, const std::function<void(void)> &func);
    void setMiscCallback(unsigned char code, const std::function<void(void)> &func);
    void setMouseCallback(std::function<void(int, int)> &func);
    #endif
private:
    
    std::map< std::pair<int,int>, std::function<void(void)> > mappingKey;
    std::map< std::pair<int,int>, std::function<void(void)> > mappingMouseKey;
    std::map<unsigned char, std::function<void(void)>> mappingMisc;
    std::function<void(int, int)> cursorEvent;
};
#endif
