#pragma once
#include "Component.h"

class Entity {

private:
	std::unordered_map<std::type_index, std::shared_ptr<void>> components;
	int mId;

public:

	virtual ~Entity();
	Entity() : mId(0) {}

	int GetId() const;
	//Component* GetComponent(int ID);

	void SetId(int id);

	//template<typename T, typename... Args>
	//Entity* CreateEntityWithComponent(Args&&... args);

	int IncreaseId();
	//bool AddComponent(int ID, std::unique_ptr<Component> pComponent);
	//void PrintComponents() const;
	void Initialize();

    // Ajouter un composant à l'entité
    template<typename T, typename... Args>
    void AddComponent(Args&&... args);

    // Récupérer un composant
    template<typename T>
	std::shared_ptr<T> GetComponent();

    // Vérifier si un composant existe
    template<typename T>
	bool HasComponent();
};

#include "Entity.inl"