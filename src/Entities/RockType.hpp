#ifndef ROCKTYPE_HPP
#define ROCKTYPE_HPP

#include "Entity.hpp"

class RockType : public Entity {
public:
    RockType(Model _model, const std::string& _name, const std::string& _type);
    RockType(Mesh _mesh, const std::string& _name, const std::string& _type);

    ~RockType() override = default;

    BoundingBox GenMeshBoundingBox(const Mesh& mesh, float pos[3]) override;
    void UpdateBuffers() override;
    void RecalcNormals() override;

    float frequency;
    float amplitude;
    int rockType;
};

#endif //ROCKTYPE_HPP
