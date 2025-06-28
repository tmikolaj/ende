#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "external/imgui/imgui.h"
#include <vector>
#include "raylib.h"

struct Entity {
    ImVec4 e_colorValues;
    Color e_color;
    Model e_model;
    Mesh* e_mesh;
    BoundingBox e_boundingBox;

    std::vector<float> e_vertices;
    std::vector<float> e_normals;
    std::vector<unsigned short> e_indices;

    Entity(Model _model) {
        e_model = _model;
        e_mesh = &e_model.meshes[0];

        e_boundingBox = GenBoundingBox(*e_mesh);

        auto rawVerts = static_cast<float*>(e_mesh->vertices);
        e_vertices.assign(rawVerts, rawVerts + e_mesh->vertexCount * 3);
        auto rawNormals = static_cast<float*>(e_mesh->normals);
        e_normals.assign(rawNormals, rawNormals + e_mesh->vertexCount * 3);
        auto rawIndices = static_cast<unsigned short*>(e_mesh->indices);
        e_indices.assign(rawIndices, rawIndices + e_mesh->triangleCount * 3);

        e_colorValues = { 1.0f, 1.0f, 1.0f, 1.0f };
        e_color = ImVecToColor(e_colorValues);
    }
    Color ImVecToColor(const ImVec4& toConvert) {
        return (Color){
            static_cast<unsigned char>(toConvert.x * 255.0f),
            static_cast<unsigned char>(toConvert.y * 255.0f),
            static_cast<unsigned char>(toConvert.z * 255.0f),
            255
        };
    }
    BoundingBox GenBoundingBox(const Mesh& mesh) {
        BoundingBox box = { 0 };

        if (mesh.vertexCount == 0 || mesh.vertices == nullptr) return box;

        float* vertices = mesh.vertices;

        Vector3 min = { vertices[0], vertices[1], vertices[2] };
        Vector3 max = min;

        for (int i = 1; i < mesh.vertexCount; i++) {
            int index = i * 3;
            float x = vertices[index + 0];
            float y = vertices[index + 1];
            float z = vertices[index + 2];

            if (x < min.x) min.x = x;
            if (y < min.y) min.y = y;
            if (z < min.z) min.z = z;
            if (x > max.x) max.x = x;
            if (y > max.y) max.y = y;
            if (z > max.z) max.z = z;
        }
        box.min = min;
        box.max = max;
        return box;
    }
    void UpdateBuffers() {
        UpdateMeshBuffer(*e_mesh, 0, e_vertices.data(), e_vertices.size() * sizeof(float), 0);
        UpdateMeshBuffer(*e_mesh, 2, e_normals.data(), e_normals.size() * sizeof(float), 0);
    }
};

#endif // ENTITY_HPP