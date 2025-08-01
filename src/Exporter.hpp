#ifndef EXPORTER_HPP
#define EXPORTER_HPP

#include <memory>
#include <string>
#include "entities/Entity.hpp"

namespace Engine {

class Exporter {
public:
    Exporter() = default;
    ~Exporter() = default;

    bool ExportOBJ(std::unique_ptr<Entity>& entity, std::string path);
};

};

#endif //EXPORTER_HPP
