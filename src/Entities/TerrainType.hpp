#ifndef TERRAINTYPE_HPP
#define TERRAINTYPE_HPP

#include "Entity.hpp"

class TerrainType : public Entity {
public:
    TerrainType(Model _model, const std::string& _name, const std::string& _type);
    TerrainType(Mesh _mesh, const std::string& _name, const std::string& _type);

    ~TerrainType() override = default;

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

#endif //TERRAINTYPE_HPP
