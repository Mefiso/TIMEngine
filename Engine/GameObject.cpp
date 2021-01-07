#include "GameObject.h"
#include "CMesh.h"
#include "CTransform.h"
#include "CMaterial.h"
#include "CCamera.h"


int GameObject::objectCount = 0;

GameObject::GameObject()
{
	aabb.SetNegativeInfinity();
	obb.SetNegativeInfinity();
	++objectCount;
}

GameObject::GameObject(const std::string& _name) : name(_name)
{
	aabb.SetNegativeInfinity();
	obb.SetNegativeInfinity();
	++objectCount;
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
		RELEASE(*it);
	}
	children.clear();
}

void GameObject::Draw()
{
	//dd::aabb(aabb.minPoint, aabb.maxPoint, float3(0.9f));
	if (name.compare("Scene 1") != 0)
	{
		dd::box(obbPoints, float3(0.9f));
	}
	// update components
	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
	{
		(*it)->Draw();
	}
	//update children accordingly
	for (std::vector<GameObject*>::iterator it = children.begin(); it != children.end(); ++it)
	{
		(*it)->Draw();
	}
}

void GameObject::AddComponent(ComponentType _type, void* arg, const std::string& path)
{
	Component* newComp;
	switch (_type)
	{
	case TRANSFORM:
		if (!this->GetTransform())
		{
			newComp = new CTransform(this);
			transform = (CTransform*)newComp;
			components.push_back(newComp);
		}
		break;
	case MESH:
		if (!this->GetComponent<CMesh>())
		{
			newComp = new CMesh(this, (aiMesh*)arg);
			components.push_back(newComp);
			UpdateBoundingBoxes();
		}
		break;
	case MATERIAL:
		if (!this->GetComponent<CMaterial>())
		{
			newComp = new CMaterial(this, (aiMaterial*)arg, path);
			components.push_back(newComp);
		}
		break;
	case CAMERA:
		if (!this->GetComponent<CCamera>())
		{
			newComp = new CCamera(this);
			components.push_back(newComp);
		}
		break;
	default:
		newComp = new Component(INVALID, this);
		components.push_back(newComp);		// TODO: Should we really be doing this?
	}
}

void GameObject::RemoveComponent(int _cID)
{
	int toRemove = -1;

	for (unsigned int i = 0u; i < components.size(); ++i)
		if (components[i]->ID == _cID)
		{
			toRemove = (int)i;
			break;
		}
	if (toRemove >= 0)
	{
		if (components[toRemove]->GetType() == TRANSFORM)
			transform = nullptr;
		RELEASE(components[toRemove]);
		components.erase(components.begin() + toRemove);
	}
}

void GameObject::SetParent(GameObject* _newParent)
{
	// Check if _newParent if a child of this GameObject
	bool isChild = false;
	for (unsigned int i = 0; i < this->GetChildren().size(); ++i)
	{
		if (this->children[i]->GetUID() == _newParent->GetUID())
		{
			isChild = true;
			break;
		}
	}
	// Change parents (only if the parent was not one of its childs)
	if (!isChild)
	{
		_newParent->AddChild(this);
		if (parent)
		{
			parent->RemoveChild(this->uID);
			UpdateBoundingBoxes();
		}
		parent = _newParent;
		UpdateBoundingBoxes();
	}
}

void GameObject::AddChild(GameObject* _newChild)
{
	if (_newChild->transform && _newChild->parent != nullptr)
	{
		float4x4 worldTransform = _newChild->GetModelMatrix();
		float4x4 thisTransform = this->transform ? this->GetModelMatrix() : float4x4::identity;
		thisTransform.InverseOrthonormal();
		_newChild->SetTransform(thisTransform * worldTransform, this);
	}
	children.push_back(_newChild);
}

void GameObject::RemoveChild(int childID)
{
	int toRemove = -1;
	for (unsigned int i = 0u; i < children.size(); ++i)
	{
		if (children[i]->uID == childID)
		{
			toRemove = (int)i;
			break;
		}
	}
	if (toRemove >= 0)
		children.erase(children.begin() + toRemove);
}

float4x4 GameObject::GetModelMatrix() const
{
	if (parent && parent->transform)
		return (parent->GetModelMatrix() * this->transform->GetTransformationMatrix());
	else
		return  this->transform->GetTransformationMatrix();
}

void GameObject::SetTransform(float3& _scale, float3& _rotation, float3& _translation)
{
	transform->SetPos(_translation);
	transform->SetRotation(_rotation);
	transform->SetScale(_scale);
	transform->UpdateTransformMatrix();

	UpdateBoundingBoxes();
}

void GameObject::SetTransform(float4x4& _newTransform, GameObject* _newParent)
{
	transform->SetPos((float3)(_newTransform.Col3(3)));

	float3 oldParentScale = parent->transform ? parent->transform->GetScale() : float3::one;
	float3 oldScaleChild = transform->GetScale();
	float3 totalScale = oldParentScale.Mul(oldScaleChild);
	float3 newChildScale = totalScale.Div(_newParent->transform ? _newParent->transform->GetScale() : float3::one);
	transform->SetScale(newChildScale);

	float3 rotation;
	rotation.x = -atan2(_newTransform.Col3(2)[1], _newTransform.Col3(2)[2]);
	rotation.y = -atan2(-_newTransform.Col3(2)[0], sqrt(_newTransform.Col3(2)[1] * _newTransform.Col3(2)[1] + _newTransform.Col3(2)[2] * _newTransform.Col3(2)[2]));
	rotation.z = -atan2(_newTransform.Col3(1)[0], _newTransform.Col3(0)[0]);
	transform->SetRotation(rotation);

	transform->UpdateTransformMatrix();
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

void GameObject::UpdateBoundingBoxes()
{
	aabb.SetNegativeInfinity();
	CMesh* mesh = GetComponent<CMesh>();
	if (mesh)
		aabb.Enclose(mesh->AABBmin, mesh->AABBmax);

	for (GameObject* child : children)
	{
		child->UpdateBoundingBoxes();

		if (transform)
		{
			float4x4 inverseTransform = GetModelMatrix();
			transform->GetScale().Equals(float3::one) ? inverseTransform.InverseOrthonormal() : inverseTransform.Inverse();
			child->obb.Transform(inverseTransform);
		}
		aabb.Enclose(child->obb);
		if (transform)
			child->obb.Transform(GetModelMatrix());
	}
	obb = transform ? aabb.Transform(GetModelMatrix()) : aabb;

	//Added, calculate obb vertices only when obb is updated
	ddVec3 points[8];
	obb.GetCornerPoints(points);
	ddVec3 points2[8] = { points[0], points[1], points[3], points[2], points[4], points[5], points[7], points[6] };
	memcpy(obbPoints, points2, sizeof(ddVec3) * 8);

	if (this->parent)
		parent->UpdateBoundingBoxesRecursive();
}

void GameObject::UpdateBoundingBoxesRecursive()
{
	aabb.SetNegativeInfinity();
	CMesh* mesh = GetComponent<CMesh>();
	if (mesh)
		aabb.Enclose(mesh->AABBmin, mesh->AABBmax);

	for (GameObject* child : children)
	{
		if (transform)
		{
			float4x4 inverseTransform = GetModelMatrix();
			transform->GetScale().Equals(float3::one) ? inverseTransform.InverseOrthonormal() : inverseTransform.Inverse();
			child->obb.Transform(inverseTransform);
		}
		aabb.Enclose(child->obb);
		if (transform)
			child->obb.Transform(GetModelMatrix());
	}
	obb = transform ? aabb.Transform(GetModelMatrix()) : aabb;

	//Added, calculate obb vertices only when obb is updated
	ddVec3 points[8];
	obb.GetCornerPoints(points);
	ddVec3 points2[8] = { points[0], points[1], points[3], points[2], points[4], points[5], points[7], points[6] };
	memcpy(obbPoints, points2, sizeof(ddVec3) * 8);

	if (this->parent)
		parent->UpdateBoundingBoxesRecursive();
}