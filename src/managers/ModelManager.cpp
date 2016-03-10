/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator:  $
   $Notice:  $
   ======================================================================== */
#include "ModelManager.h"
using namespace std;
//Singleton to manage the loaded models (easier to use everywhere
// on the program for a minus drawback)

ModelManager::ModelManager()
        : models( map<string, Model*>() )
{
}
