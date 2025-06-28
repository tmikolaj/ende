#ifndef SHADERMANAGER_HPP
#define SHADERMANAGER_HPP

#include "bEngineComponent.hpp"
#include "raylib.h"

namespace Engine {

class ShaderManager : public Engine::bEngineComponent<Shader> {
private:
    Shader materialPreviewShader;
    Shader renderShader;
    Shader solidShader;
public:
    ShaderManager() = default;
    ~ShaderManager() override = default;

    void init() override;
    Shader set(const Shader& prev) override;
    void clean() override;
};

}

#endif //SHADERMANAGER_HPP
