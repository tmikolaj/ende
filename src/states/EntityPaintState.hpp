#ifndef ENTITYPAINTSTATE_HPP
#define ENTITYPAINTSTATE_HPP

#include <memory>
#include <random>

#include "imgui.h"
#include "BaseState.hpp"

struct Context;

class EntityPaintState : public BaseState {
private:

    // brush related variables
    float brushSize;
    ImVec4 paintColor;
    RenderTexture2D* paintCanvas;
    int textureWidth;
    int textureHeight;
    bool validHit;
    Vector3 hitPos;
    Vector3 hitNormal;
    bool lockDraw;
    Texture2D brushTexture;
    int selectedBrush;
    ImVec4 textureTint;
    Texture2D standardBrushTexture;
    Texture2D textureBrushTexture;

    // random
    bool toggleAngleJitter;
    bool useAngle;
    int angle;
    std::default_random_engine gen;
    std::random_device rd;
    std::uniform_int_distribution<int> dist;
public:
    EntityPaintState() = default;
    ~EntityPaintState() override = default;

    void init(std::shared_ptr<Context>& p_context) override;
    void process(std::shared_ptr<Context>& p_context) override;
    void draw(std::shared_ptr<Context>& p_context) override;
    void clean(std::shared_ptr<Context>& p_context) override;

    Color ImVec4ToColor(ImVec4 p_color);
    bool CheckCollisionRayTriangle(const Ray& ray, Vector3 v0, Vector3 v1, Vector3 v2, Vector3* p_hitPos);
};

#endif //ENTITYPAINTSTATE_HPP
