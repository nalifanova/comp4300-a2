#ifndef ENTITY_H
#define ENTITY_H

#include <memory>
#include <string>
#include "Components.h"

class Entity
{
public:
    // private member access functions
    [[nodiscard]] bool isActive() const;
    [[nodiscard]] const std::string& tag() const;
    [[nodiscard]] size_t id() const;
    void destroy();

//private:
    // constructor and destructor
    Entity(size_t id, const std::string& tag);
    ~Entity() = default;
private:
    // constructor and destructor should be private
    // but it causes the issue (TODO: find out)
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
