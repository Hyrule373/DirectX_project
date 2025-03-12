#pragma once
#include "Entity.h"

//template<typename T, typename... Args>
//Entity* CreateEntityWithComponent(Args&&... args) {
//    static_assert(std::is_base_of<Component, T>::value, "T must be derived from Component");
//
//    Entity* newEntity = new Entity();
//    newEntity->SetId(newEntity->IncraseId());
//
//    std::unique_ptr<T> newComponent = std::make_unique<T>(std::forward<Args>(args)...);
//
//    if (!newEntity->AddComponent(newComponent->GetID(), std::move(newComponent))) {
//        delete newEntity;
//        throw std::runtime_error("Failed to add component to entity");
//    }
//
//    return newEntity;
//}


// Ajouter un composant à l'entité
template<typename T, typename... Args>
void Entity::AddComponent(Args&&... args) {
    components[typeid(T)] = std::make_shared<T>(std::forward<Args>(args)...);
}

// Récupérer un composant
template<typename T>
std::shared_ptr<T> Entity::GetComponent() {
    auto it = components.find(typeid(T));
    if (it != components.end()) {
        return std::static_pointer_cast<T>(it->second);
    }
    return nullptr;
}

// Vérifier si un composant existe
template<typename T>
bool Entity::HasComponent() {
    return components.find(typeid(T)) != components.end();
}