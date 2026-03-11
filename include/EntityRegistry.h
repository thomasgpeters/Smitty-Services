#ifndef ENTITY_REGISTRY_H
#define ENTITY_REGISTRY_H

#include <string>
#include <map>
#include <memory>
#include "Entity.h"

class EntityRegistry {
public:
    static EntityRegistry& instance();

    void registerEntity(std::shared_ptr<Entity> entity);
    std::shared_ptr<Entity> getEntity(const std::string& resourceName) const;
    std::vector<std::string> entityNames() const;

private:
    EntityRegistry();
    void initializeEntities();

    std::map<std::string, std::shared_ptr<Entity>> entities_;
};

#endif // ENTITY_REGISTRY_H
