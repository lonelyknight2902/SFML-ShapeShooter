#include "EntityManager.h"

EntityManager::EntityManager()
{

}

void EntityManager::update() {
  for(auto e : m_entitiesToAdd) 
  {
    m_entities.push_back(e);
    m_entityMap[e->tag()].push_back(e);
  }
  m_entitiesToAdd.clear();

  removeDeadEntities(m_entities);

  for (auto& [tag, entityVec] : m_entityMap)
  {
    removeDeadEntities(entityVec);
  }
}

void EntityManager::removeDeadEntities(EntityVec& vec)
{
  for (auto e : vec)
  {
    if(!e->isActive())
    {
      // std::remove_if();
    }
  }
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag)
{
  auto e = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));
  m_entitiesToAdd.push_back(e);
  // m_entities.push_back(e);
  // m_entityMap[tag].push_back(e);
  return e;
}

EntityVec& EntityManager::getEntities()
{
  return m_entities;
}

EntityVec& EntityManager::getEntities(const std::string& tag)
{
  return m_entityMap[tag];
}
