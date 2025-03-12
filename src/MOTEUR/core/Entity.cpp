#include "pch.h"
#include "Entity.h"

Entity::~Entity() {

	//for (auto& pair : mComponents)
	//	delete pair.second;
}

int Entity::GetId() const { return mId; }

void Entity::SetId(int id) { int mId = id; }

int Entity::IncreaseId() { return ++mId; }

//bool Entity::AddComponent(int ID, std::unique_ptr<Component> pComponent) {
//    if (!pComponent) {
//        return false; 
//    }
//
//    if (mComponents.find(ID) != mComponents.end()) {
//        return false; 
//    }
//
//    mComponents.emplace(ID, std::move(pComponent));
//    return true; 
//}
//
//Component* Entity::GetComponent(int ID) {
//    auto it = mComponents.find(ID);
//    if (it != mComponents.end()) {
//        return it->second.get();
//    }
//    return nullptr;
//}
//
//void Entity::PrintComponents() const {
//    if (mComponents.empty()) {
//        std::cout << "Entity " << mId << " has no components." << std::endl;
//        return;
//    }
//
//    std::cout << "Entity " << mId << " has the following components:" << std::endl;
//    for (const auto& pair : mComponents) {
//        std::cout << "- Component ID: " << pair.first << " (Type: " << pair.second->GetName() << ")" << std::endl;
//    }
//}

void Entity::Initialize() {

}

//// Créer une entité avec un InputComponent
//Entity* entity = CreateEntityWithComponent<InputComponent>();
//
//// Add PosComponent
//auto posComponent = std::make_unique<PosComponent>(10.0f, 20.0f); // Exemple de position
//if (!entity->AddComponent(posComponent->GetID(), std::move(posComponent))) {
//    std::cout << "Error: Failed to add PosComponent." << std::endl;
//}
//
//// Add MeshComponent
//auto meshComponent = std::make_unique<MeshComponent>();
//if (!entity->AddComponent(meshComponent->GetID(), std::move(meshComponent))) {
//    std::cout << "Error: Failed to add MeshComponent." << std::endl;
//}
//
//// Add InputComponent (Out "Error: ..." because entity has already this one)
//auto inputComponent = std::make_unique<InputComponent>();
//if (!entity->AddComponent(inputComponent->GetID(), std::move(inputComponent))) {
//    std::cout << "Error: Failed to add InputComponent" << std::endl;
//}
//
//// Add PhysicsComponent
//auto physicsComponent = std::make_unique<PhysicsComponent>();
//if (!entity->AddComponent(physicsComponent->GetID(), std::move(physicsComponent))) {
//    std::cout << "Error: Failed to add PhysicsComponent." << std::endl;
//}
//
//// Vérifier si l'entité a été créée avec succès
//if (entity) {
//    std::cout << "Entity created successfully !" << std::endl;
//
//    PosComponent* posComponent = dynamic_cast<PosComponent*>(entity->GetComponent(1));
//    if (posComponent) {
//        std::cout << "PosComponent found with ID: " << posComponent->GetID() << std::endl;
//    }
//    else {
//        std::cout << "Error: PosComponent not found" << std::endl;
//    }
//
//    MeshComponent* meshComponent = dynamic_cast<MeshComponent*>(entity->GetComponent(2));
//    if (meshComponent) {
//        std::cout << "MeshComponent found with ID: " << meshComponent->GetID() << std::endl;
//    }
//    else {
//        std::cout << "Error: MeshComponent not found" << std::endl;
//    }
//
//    InputComponent* inputComponent = dynamic_cast<InputComponent*>(entity->GetComponent(3));
//    if (inputComponent) {
//        std::cout << "InputComponent found with ID: " << inputComponent->GetID() << std::endl;
//    }
//    else {
//        std::cout << "Error: InputComponent not found" << std::endl;
//    }
//
//    PhysicsComponent* physComponent = dynamic_cast<PhysicsComponent*>(entity->GetComponent(4));
//    if (physComponent) {
//        std::cout << "PhysicsComponent found with ID: " << physComponent->GetID() << std::endl;
//    }
//    else {
//        std::cout << "Error: PhysicsComponent not found" << std::endl;
//    }
//
//}
//else {
//    std::cout << "Error: Failed to create entity." << std::endl;
//}
//
//entity->PrintComponents();
//
//// Nettoyer
//delete entity;