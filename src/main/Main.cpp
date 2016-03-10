/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator:  $
   $Notice: $
   ======================================================================== */
#include "Main.h"
using namespace std;

//Launch game
int main(int argc, char** argv)
{
    Game* mygame = new Game(800,600);
    mygame->mainLoop();
    return(0);
}
