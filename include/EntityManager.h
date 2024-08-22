#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H

#include "Entity.h"

#include <map>
#include <vector>

typedef std::vector<std::shared_ptr<Entity>> EntityVec;
typedef std::map<std::string, EntityVec> EntityMap;

class EntityManager
{
public:
    EntityManager() = default;

    void update();

    std::shared_ptr<Entity> addEntity(const std::string& tag);
    [[nodiscard]] EntityVec& getEntities();
    [[nodiscard]] EntityVec& getEntities(const std::string& tag);
    [[nodiscard]] const EntityMap& getEntityMap();

private:
    void removeDeadEntities(EntityVec& vec);

    EntityVec m_entities;
    EntityVec m_entitiesToAdd;
    EntityMap m_entityMap;
    size_t m_totalEntities = 0;
};

#endif //ENTITYMANAGER_H
