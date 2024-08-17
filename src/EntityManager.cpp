#include "EntityManager.h"

void EntityManager::update()
{
    // TODO: add entities from m_entitiesToAdd to the proper location(s)
    //      - add them to the vector of all entities
    //      - add them to the vector inside the map, with the tag as a key

    // remove dead entities from the vector of all entities
    removeDeadEntities(m_entities);

    // remove dead entities from each vector in the entity map
    // C++20 way of iterating through [key, value] pairs in a map
    for (auto& [tag, entityVec]: m_entityMap)
    {
        removeDeadEntities(entityVec);
    }
}

void EntityManager::removeDeadEntities(EntityVec& vec)
{
    // TODO: remove all dead entities from the input vector
    //       this is called by the update() function
    // Note: think of iteration invalidation!
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag)
{
    // initially
    // auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));
    auto entity = std::make_shared<Entity>(m_totalEntities++, tag);

    m_entitiesToAdd.push_back(entity);

    return entity;
}

const EntityVec& EntityManager::getEntities()
{
    return m_entities;
}

const EntityVec& EntityManager::getEntities(const std::string& tag)
{
    return m_entityMap[tag];
}

const EntityMap& EntityManager::getEntityMap()
{
    return m_entityMap;
}



