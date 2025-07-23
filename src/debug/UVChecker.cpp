#include "UVChecker.hpp"

Texture2D UVChecker::GenerateCheckerTexture(int size, int checksX, int checksY) {
    Image image = GenImageChecked(size, size, checksX, checksY, DARKGRAY, LIGHTGRAY);
    Texture2D texture = LoadTextureFromImage(image);
    UnloadImage(image);
    return texture;
}

void UVChecker::swapTexture(Texture2D& texture, Texture2D& replace) {
    swappedTexture = texture;
    texture = replace;
}
