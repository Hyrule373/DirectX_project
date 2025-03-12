#pragma once

class Entity;

class Component {

private:
	Entity* mpEntity;

public:

	virtual ~Component();
	Component();

	virtual int GetID() = 0;
	virtual std::string GetName() const = 0;
};

