#include "SubdivisionShaper.hpp"

SubdivisionShaper::SubdivisionShaper(Entity* _entity, const bool _isCircular) :
Shaper(_entity),
isCircular(_isCircular),
subdivisions(0) {

}

int SubdivisionShaper::GetMidpoint(int i1, int i2, std::vector<float>& vertices,std::map<std::pair<int, int>, int>& cache) {
    auto key = std::minmax(i1, i2);

    if (cache.contains(key)) return cache[key];

    const glm::vec3 v1(vertices[i1 * 3], vertices[i1 * 3 + 1], vertices[i1 * 3 + 2]);
    const glm::vec3 v2(vertices[i2 * 3], vertices[i2 * 3 + 1], vertices[i2 * 3 + 2]);

    glm::vec3 midpoint;
    if (isCircular) {
        midpoint = glm::normalize(0.5f * (v1 + v2));
    } else {
        midpoint = 0.5f * (v1 + v2);
    }

    vertices.push_back(midpoint.x);
    vertices.push_back(midpoint.y);
    vertices.push_back(midpoint.z);

    int index = static_cast<int>(vertices.size() / 3 - 1);
    cache[key] = index;

    return index;
}

void SubdivisionShaper::Apply(std::unique_ptr<Entity>& e) {
    Texture2D tex = e->e_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture;

    subdivisions++;

    std::vector<float> newVertices = e->e_vertices;
    std::vector<unsigned short> newIndices;
    std::vector<float> newNormals;

    std::vector<float> newTexcoords;

    std::map<std::pair<int, int>, int> midpointCache;

    float minX = FLT_MAX, maxX = -FLT_MAX;
    float minZ = FLT_MAX, maxZ = -FLT_MAX;

    for (int i = 0; i < e->e_mesh->triangleCount; i++) {
        int i0 = e->e_indices[i * 3];
        int i1 = e->e_indices[i * 3 + 1];
        int i2 = e->e_indices[i * 3 + 2];

        int a = GetMidpoint(i0, i1, newVertices, midpointCache);
        int b = GetMidpoint(i1, i2, newVertices, midpointCache);
        int c = GetMidpoint(i2, i0, newVertices, midpointCache);

        newIndices.push_back(i0); newIndices.push_back(a); newIndices.push_back(c);
        newIndices.push_back(a); newIndices.push_back(i1); newIndices.push_back(b);
        newIndices.push_back(c); newIndices.push_back(b); newIndices.push_back(i2);
        newIndices.push_back(a); newIndices.push_back(b); newIndices.push_back(c);
    }

    for (int i = 0; i < newVertices.size(); i += 3) {
        float x = newVertices[i];
        float z = newVertices[i + 2];

        if (x < minX) minX = x;
        if (x > maxX) maxX = x;
        if (z < minZ) minZ = z;
        if (z > maxZ) maxZ = z;
    }

    for (int i = 0; i < newVertices.size(); i++) {
        float x = newVertices[i * 3];
        float z = newVertices[i * 3 + 2];

        float u = (x - minX) / (maxX - minX);
        float v = (z - minZ) / (maxZ - minZ);

        newTexcoords.push_back(u);
        newTexcoords.push_back(v);
    }

    e->RecalcNormals();
    e->e_normals = newNormals;

    UnloadModel(e->e_model);

    Mesh newMesh = { 0 };

    newMesh.vertices = static_cast<float*>(RL_MALLOC(sizeof(float) * newVertices.size()));
    memcpy(newMesh.vertices, newVertices.data(), sizeof(float) * newVertices.size());

    newMesh.indices = static_cast<unsigned short*>(RL_MALLOC(sizeof(unsigned short) * newIndices.size()));
    memcpy(newMesh.indices, newIndices.data(), sizeof(unsigned short) * newIndices.size());

    newMesh.normals = static_cast<float*>(RL_MALLOC(sizeof(float) * newNormals.size()));
    memcpy(newMesh.normals, newNormals.data(), sizeof(float) * newNormals.size());

    newMesh.texcoords = static_cast<float*>(RL_MALLOC(sizeof(float) * newTexcoords.size()));
    memcpy(newMesh.texcoords, newTexcoords.data(), sizeof(float) * newTexcoords.size());

    newMesh.vertexCount = static_cast<int>(newVertices.size()) / 3;
    newMesh.triangleCount = static_cast<int>(newIndices.size()) / 3;

    UploadMesh(&newMesh, true);

    e->e_vertices = std::move(newVertices);
    e->e_indices = std::move(newIndices);
    e->e_mesh = new Mesh(newMesh);

    e->e_model = LoadModelFromMesh(newMesh);

    e->e_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = tex;
}
