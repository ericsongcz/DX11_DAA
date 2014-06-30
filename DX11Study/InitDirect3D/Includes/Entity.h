#pragma once
#include "IRenderable.h"

class Entity : public IRenderable
{
public:
	Entity();
	~Entity();

	RenderPackage getRenderPackage() const;
private:

};