#ifndef ENTITY_H
#define ENTITY_H

#include <string>

#include "Components.h"

class EntityManager;

class Entity
{
public:
    ~Entity() = default;

    [[nodiscard]] bool isActive() const;
    [[nodiscard]] const std::string& tag() const;
    [[nodiscard]] size_t id() const;
    void destroy();

private:
    Entity(size_t id, const std::string& tag);

public:
    // component pointers
    std::shared_ptr<CTransform> cTransform;
    std::shared_ptr<CShape> cShape;
    std::shared_ptr<CCollision> cCollision;
    std::shared_ptr<CInput> cInput;
    std::shared_ptr<CScore> cScore;
    std::shared_ptr<CLifespan> cLifespan;

private:
    bool m_active = true;
    size_t m_id = 0;
    std::string m_tag = "default";

    friend class EntityManager;
};

#endif //ENTITY_H
