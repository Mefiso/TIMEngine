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
	CleanUp();
}

void GameObject::CleanUp()
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
		(*it)->CleanUp();
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
		transform = (CTransform*)newComp;
		break;
	case MESH:
		newComp = new CMesh(this, (aiMesh*)arg);
		break;
	case MATERIAL:
		newComp = new CMaterial(this, (aiMaterial*)arg, path);
		break;
	}

	components.push_back(newComp);
}

void GameObject::RemoveComponent(int _cID)
{
	int toRemove = -1;

	for (unsigned int i = 0u; i < components.size(); ++i)
		if (components[i]->ID == _cID) {
			toRemove = (int)i;
			break;
		}
	if (toRemove >= 0) {
		RELEASE(components[toRemove]);
		components.erase(components.begin() + toRemove);
	}
}

void GameObject::AddChild(GameObject* _newChild)
{
	if (_newChild->hasTransform && _newChild->parent != nullptr)
	{
		float4x4 worldTransform = _newChild->GetModelMatrix(); // antic pare * antiga tranform del fill =  nou pare * nova fill
		float4x4 thisTransform = this->hasTransform ? this->GetModelMatrix() : float4x4::identity;
		thisTransform.InverseOrthonormal();
		_newChild->SetTransform(thisTransform * worldTransform, this);
	}
	children.push_back(_newChild);
}

void GameObject::RemoveChild(int childID)
{
	int toRemove = -1;
	for (unsigned int i = 0u; i < children.size(); ++i) {
		if (children[i]->uID == childID) {
			toRemove = (int)i;
			break;
		}
	}
	if (toRemove >= 0)
		children.erase(children.begin() + toRemove);
}

CMaterial* GameObject::GetMaterial() const
{
	for (unsigned int i = 0u; i < components.size(); ++i) {
		if (components[i]->GetType() == MATERIAL)
			return (CMaterial*)components[i];
	}

	return nullptr;
}

float4x4 GameObject::GetModelMatrix() const
{
	if (parent && parent->hasTransform)
		return (parent->GetModelMatrix() * this->transform->GetTransformationMatrix());
	else
		return  this->transform->GetTransformationMatrix();
}

CTransform* GameObject::GetTransform() const
{
	return transform;
}

void GameObject::SetTransform(float3& _scale, float3& _rotation, float3& _translation)
{
	transform->SetPos(_translation);
	transform->SetRotation(_rotation);
	transform->SetScale(_scale);
}

void GameObject::SetTransform(float4x4& _newTransform, GameObject* _newParent)
{
	transform->SetPos((float3)(_newTransform.Col3(3)));

	float3 oldParentScale = parent->hasTransform ? parent->transform->GetScale() : float3::one;
	float3 oldScaleChild = transform->GetScale();
	float3 totalScale = oldParentScale.Mul(oldScaleChild);
	float3 newChildScale = totalScale.Div(_newParent->transform->GetScale());
	transform->SetScale(newChildScale);

	float3 rotation;
	rotation.x = -atan2(_newTransform.Col3(2)[1], _newTransform.Col3(2)[2]);
	rotation.y = -atan2(-_newTransform.Col3(2)[0], sqrt(_newTransform.Col3(2)[1] * _newTransform.Col3(2)[1] + _newTransform.Col3(2)[2] * _newTransform.Col3(2)[2]));
	rotation.z = -atan2(_newTransform.Col3(1)[0], _newTransform.Col3(0)[0]);
	transform->SetRotation(rotation);
}

void GameObject::SetParent(GameObject* _newParent)
{
	_newParent->AddChild(this);
	if (parent)
		parent->RemoveChild(this->uID);
	parent = _newParent;
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