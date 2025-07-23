#include <string>
#include "../../external/raylib/src/rlights.h"

struct lLight {
    Light _l_light;
    bool deleted;
    std::string name;

    lLight(const std::string& _name, Light l) : _l_light(l), deleted(false), name(_name) {}
};