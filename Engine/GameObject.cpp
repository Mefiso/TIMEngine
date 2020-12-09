#include "GameObject.h"
#include "CMesh.h"
#include "CTransform.h"
#include "CMaterial.h"

int GameObject::objectCount = 0;

GameObject::GameObject()
{
	objectCount++;
}

GameObject::~GameObject()
{
	// Clean components
	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
	{
		RELEASE(*it);
	}
	components.clear();

	// Clean childs
	for (std::vector<GameObject*>::iterator it = children.begin(); it != children.end(); ++it)
	{
		RELEASE(*it);
	}
	children.clear();
}

void GameObject::Update()
{
	// update components
	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
	{
		(*it)->Update();
	}
	//update children accordingly
	for (std::vector<GameObject*>::iterator it = children.begin(); it != children.end(); ++it)
	{
		(*it)->Update();
	}
}

void GameObject::AddComponent(ComponentType _type, void* arg, const std::string& path)
{
	Component* newComp;
	//for (unsigned int i = 0; i< components.size())
	//check if already exists CMaterial or CTransform
	switch (_type)
	{
	case TRANSFORM:
		newComp = new CTransform(this);
		transform = (CTransform*) newComp;
		break;
	case MESH:
		newComp = new CMesh(this, (aiMesh*) arg);
		break;
	case MATERIAL:
		newComp = new CMaterial(this, (aiMaterial*) arg, path);
		break;
	}
	
	components.push_back(newComp);
}

void GameObject::RemoveComponent(unsigned int _cID)
{
	int toRemove = -1;
	unsigned int i;

	for (i = 0u; i<components.size(); ++i )
		if (components[i]->ID == _cID) {
			toRemove = i;
			break;
		}	
	if (toRemove >= 0) {
		RELEASE(components[i]);
		components.erase(components.begin() + i);
	}
}

void GameObject::AddChild(GameObject* _newChild)
{
	_newChild->SetParent(this);
	children.push_back(_newChild);
}

void GameObject::RemoveChild(int i)
{
	children.erase(children.begin()+i);
}

CMaterial* GameObject::GetMaterial()
{
	bool matFound = false;

	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
		if ((*it)->GetType() == MATERIAL)
			return (CMaterial*) (*it);
	return nullptr;
}

float4x4* GameObject::GetModelMatrix() const
{
	if (hasTransform)
	{
		if (parent && parent->hasTransform)
			return &(*parent->GetModelMatrix() * this->transform->GetTransformationMatrix());
		else
			return  &this->transform->GetTransformationMatrix();
	}
	else
		return nullptr;
}

void GameObject::SetProgram(unsigned int program)
{
	// update components
	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
	{
		if ((*it)->GetType() == MESH)
			((CMesh*)(*it))->SetProgram(program);
	}
	//update children accordingly
	for (std::vector<GameObject*>::iterator it = children.begin(); it != children.end(); ++it)
	{
		(*it)->SetProgram(program);
	}
}
