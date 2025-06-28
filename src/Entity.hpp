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

    std::vector<float> e_vertices;
    std::vector<float> e_normals;
    std::vector<unsigned short> e_indices;

    Entity(Model _model) {
        e_model = _model;
        e_mesh = &e_model.meshes[0];

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
};

#endif // ENTITY_HPP