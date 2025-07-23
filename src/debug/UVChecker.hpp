#ifndef UV_HPP
#define UV_HPP

#include "raylib.h"

class UVChecker {
public:
    UVChecker() = default;
    ~UVChecker() = default;

    Texture2D swappedTexture;

    Texture2D GenerateCheckerTexture(int size = 256, int checksX = 8, int checksY = 8);
    void swapTexture(Texture2D& texture, Texture2D& replace);
};

#endif //UV_HPP
