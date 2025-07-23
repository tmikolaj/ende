#ifndef CUSTOMMESHFACTORY_HPP
#define CUSTOMMESHFACTORY_HPP

#include <../../external/glm/glm.hpp>
#include "../../external/raylib/src/raylib.h"

class CustomMeshFactory {
public:
    CustomMeshFactory() = default;
    ~CustomMeshFactory() = default;

    Mesh GenMeshIcosahedron();
};

#endif //CUSTOMMESHFACTORY_HPP
