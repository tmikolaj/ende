#include "ThermalErosion.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

#include <external/glm/vec2.hpp>

static constexpr float EPS = 1e-4f;

ThermalErosion::ThermalErosion() :
talus(1.0f),
strength(0.5f),
iterations(1),
use4(true) {

}

static float findLargestGap(const std::vector<float>& u) {
    float maxGap = 0.0f;

    for (size_t i = 1; i < u.size(); ++i) {
        float gap = u[i] - u[i - 1];
        if (gap > maxGap) maxGap = gap;
    }
    return maxGap;
}

static std::pair<int, float> computeGridAxis(const std::vector<float>& raw) {
    if (raw.empty()) return { 0, 1.0f };

    std::vector<float> s = raw;
    std::sort(s.begin(), s.end());

    std::vector<float> u;
    u.reserve(s.size());

    for (float v : s) {
        if (u.empty() || std::abs(v - u.back()) > EPS) u.emplace_back(v);
    }

    float cellSize = findLargestGap(u);

    float extent = u.back() - u.front();
    int count = static_cast<int>(std::round(extent / cellSize)) + 1;

    return { count, cellSize };
}

int ThermalErosion::computeGridWidth(const std::vector<float>& vertices) {
    std::vector<float> xs;
    xs.reserve(vertices.size() / 3);

    for (size_t i = 0; i < vertices.size(); i += 3) {
        xs.emplace_back(vertices[i]);
    }
    auto [w, cellX] = computeGridAxis(xs);

    return w;
}

int ThermalErosion::computeGridHeight(const std::vector<float>& vertices) {
    std::vector<float> zs;
    zs.reserve(vertices.size() / 3);

    for (size_t i = 0; i < vertices.size(); i += 3) {
        zs.emplace_back(vertices[i + 2]);
    }

    auto [h, cellZ] = computeGridAxis(zs);

    return h;
}

void ThermalErosion::Apply(std::vector<float>& vertices) {
    int width = computeGridWidth(vertices);
    int height = computeGridHeight(vertices);

    const int n = width * height;
    const int vertCount =static_cast<int>(vertices.size() / 3);

    if (n != vertCount) {
        std::cerr << "ThermalErosion::Apply: " << n << " != " << vertCount << '\n';
        return;
    }

    std::vector<float> heightMap(n);
    for (int i = 0; i < n; i++) {
        heightMap[i] = vertices[i * 3 + 1];
    }

    std::vector<glm::ivec2> offsets;
    if (use4) {
        offsets = {{1,0},{-1,0},{0,1},{0,-1}};
    } else {
        offsets = {{1,0},{-1,0},{0,1},{0,-1},{1,1},{-1,1},{1,-1},{-1,-1}};
    }

    for (int i = 0; i < iterations; i++) {
        std::vector<float> delta(n, 0.0f);

        for (int y = 1; y < height - 1; y++) {
            for (int x = 1; x < width - 1; x++) {
                int index = y * width + x;
                float h0 = heightMap[index];

                for (auto& offset : offsets) {
                    int neighborIndex = (y + offset.y) * width + (x + offset.x);
                    float heightDiff = h0 - heightMap[neighborIndex];

                    if (heightDiff > talus) {
                        float raw = (heightDiff - talus) * (1.0f + strength);
                        float maxTransfer;
                        if (width >= 100 || height >= 100) {
                            maxTransfer = 0.0005f;
                        } else if (width < 100 || height < 100) {
                            maxTransfer = 0.002f;
                        }
                        delta[index] -= std::min(raw, maxTransfer);
                        delta[neighborIndex] += std::min(raw, maxTransfer);
                    }
                }
            }
        }
        for (int j = 0; j < n; j++) {
            heightMap[j] += delta[j];
        }
    }

    bool reported = false;
    for (int i = 0; i < n; i++) {
        float oldHeight = vertices[i * 3 + 1];
        float newHeight = heightMap[i];
        if (!reported && std::abs(newHeight - oldHeight) > 1e-6f) {
            std::cout << "ThermalErosion::Apply: change" << '\n';
            reported = true;
        }
        vertices[i * 3 + 1] = newHeight;
    }
}
