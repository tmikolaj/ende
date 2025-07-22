#include "UV.hpp"

Texture2D UV::GenerateCheckerTexture(int size, int checksX, int checksY) {
    Image image = GenImageChecked(size, size, checksX, checksY, DARKGRAY, LIGHTGRAY);
    Texture2D texture = LoadTextureFromImage(image);
    UnloadImage(image);
    return texture;
}

void UV::swapTexture(Texture2D& texture, Texture2D& replace) {
    swappedTexture = texture;
    texture = replace;
}
