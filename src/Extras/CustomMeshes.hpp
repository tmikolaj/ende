#ifndef CUSTOMMESHES_HPP
#define CUSTOMMESHES_HPP

#include <external/glm/glm.hpp>
#include "raylib.h"

class CustomMeshes {
public:
    CustomMeshes() = default;
    ~CustomMeshes() = default;

    Mesh GenMeshIcosahedron();
};

#endif //CUSTOMMESHES_HPP
