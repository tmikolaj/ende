#ifndef ROCKENTITY_HPP
#define ROCKENTITY_HPP

#include "Entity.hpp"

class RockEntity : public Entity {
public:
    RockEntity(Model _model, const std::string& _name, const std::string& _type);
    RockEntity(Mesh _mesh, const std::string& _name, const std::string& _type);

    ~RockEntity() override = default;

    BoundingBox GenMeshBoundingBox(const Mesh& mesh, float pos[3]) override;
    void UpdateBuffers() override;
    void RecalcNormals() override;

    float frequency;
    float amplitude;
    int rockType;
};

#endif //ROCKENTITY_HPP
