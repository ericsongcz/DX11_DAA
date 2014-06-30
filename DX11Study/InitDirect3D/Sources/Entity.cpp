#include "stdafx.h"
#include "Entity.h"

Entity::Entity()
{
}

Entity::~Entity()
{
}

RenderPackage Entity::getRenderPackage() const
{
	return mRenderPackage;
}
