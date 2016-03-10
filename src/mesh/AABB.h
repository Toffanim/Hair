#if !defined(AABB_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator:  $
   $Notice: $
   ======================================================================== */

#define AABB_H
#include <glm/glm.hpp>

//Simply the AABB structure
struct AABB
{
    glm::vec3 min;
    glm::vec3 max;
    glm::vec3 size;
    glm::vec3 center;
};
#endif
