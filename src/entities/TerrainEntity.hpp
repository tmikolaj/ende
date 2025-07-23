#ifndef TERRAINENTITY_HPP
#define TERRAINENTITY_HPP

#include "Entity.hpp"

class TerrainEntity : public Entity {
public:
    TerrainEntity(Model _model, const std::string& _name, const std::string& _type);
    TerrainEntity(Mesh _mesh, const std::string& _name, const std::string& _type);

    ~TerrainEntity() override = default;

    BoundingBox GenMeshBoundingBox(const Mesh& mesh, float pos[3]) override;
    void UpdateBuffers() override;
    void RecalcNormals() override;

    float amplitude;
    float frequency;
    std::string noiseType;
    int octaves;
    float lacunarity;
    float persistence;
};

#endif //TERRAINENTITY_HPP
