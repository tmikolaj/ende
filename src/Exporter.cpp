#include "Exporter.hpp"

#include <fstream>
#include <iostream>

bool Engine::Exporter::ExportOBJ(std::unique_ptr<Entity>& entity, std::string path) {
    std::ofstream file(path);

    if (!file.is_open()) {
        std::cerr << "Engine::Exporter::ExportOBJ: Failed to open file at path" << path << '\n';
        return false;
    }

    for (size_t i = 0; i < entity->e_vertices.size(); i += 3) {
        float x = entity->e_vertices[i];
        float y = entity->e_vertices[i + 1];
        float z = entity->e_vertices[i + 2];

        file << "v " << x << " " << y << " " << z << "\n";
    }

    for (size_t i = 0; i < entity->e_texcoords.size(); i += 2) {
        float u = entity->e_texcoords[i];
        float v = entity->e_texcoords[i + 1];

        file << "vt " << u << " " << 1.0f - v << "\n";
    }

    for (size_t i = 0; i < entity->e_normals.size(); i += 3) {
        float n0 = entity->e_normals[i];
        float n1 = entity->e_normals[i + 1];
        float n2 = entity->e_normals[i + 2];

        file << "vn " << n0 << " " << n1 << " " << n2 << "\n";
    }

    for (size_t i = 0; i < entity->e_indices.size(); i += 3) {
        unsigned short i0 = entity->e_indices[i] + 1;
        unsigned short i1 = entity->e_indices[i + 1] + 1;
        unsigned short i2 = entity->e_indices[i + 2] + 1;

        file << "f ";

        for (auto idx : {i0, i1, i2}) {
            file << idx;

            if (!entity->e_texcoords.empty() && !entity->e_normals.empty()) {
                file << "/" << idx << "/" << idx;

            } else if (!entity->e_texcoords.empty()) {
                file << "/" << idx;

            } else if (!entity->e_normals.empty()) {
                file << "//" << idx;

            }

            file << " ";
        }
        file << "\n";
    }

    file.close();

    return true;
}
