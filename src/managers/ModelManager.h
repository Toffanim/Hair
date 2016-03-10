#if !defined(MODELMANAGER_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator:  $
   $Notice:  $
   ======================================================================== */

#define MODELMANAGER_H

#include <string>
#include <map>
#include "../mesh/Model.h"

class ModelManager
{
public:
    static ModelManager& getInstance()
    {
        static ModelManager instance;
        return instance;
    }

    std::map<std::string, Model *>& getModels()
    { return( models ); }

private:
    ModelManager();
    ModelManager(ModelManager const&) = delete;
    void operator=(ModelManager const&) = delete;
    std::map<std::string, Model*> models;
    
};

#endif
