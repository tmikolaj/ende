#ifndef ENTITYPAINT_HPP
#define ENTITYPAINT_HPP

#include "bStateTemplate.hpp"

class EntityPaint : public bStateTemplate {
private:

    // brush related variables
    float brushSize;
    ImVec4 paintColor;
    RenderTexture2D paintCanvas;
    int textureWidth;
    int textureHeight;
    bool validHit;
    Vector3 hitPos;
    Vector3 hitNormal;

    // render/draw variables
    int currentShader;
    int selectedEntity;
public:
    EntityPaint() = default;
    ~EntityPaint() override = default;

    void init(std::shared_ptr<Context> &m_context) override;
    void process(std::shared_ptr<Context> &m_context) override;
    void draw(std::shared_ptr<Context> &m_context) override;
    void clean(std::shared_ptr<Context> &m_context) override;

    Color ImVec4ToColor(ImVec4 p_color);
    bool CheckCollisionRayTriangle(const Ray& ray, Vector3 v0, Vector3 v1, Vector3 v2, Vector3* p_hitPos);
};

#endif //ENTITYPAINT_HPP
