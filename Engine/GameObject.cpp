#include "Application.h"
#include "GameObject.h"
#include "ModuleSceneManager.h"
#include "ModuleRender.h"
#include "CMesh.h"
#include "CTransform.h"
#include "CMaterial.h"
#include "CCamera.h"
#include "debugdraw.h"
#include "Leaks.h"


LCG GameObject::randomGen = LCG();
bool GameObject::drawOBB = false;

GameObject::GameObject() : UUID(randomGen.Int())
{
	aabb.SetNegativeInfinity();
	obb.SetNegativeInfinity();
}

GameObject::GameObject(const std::string& _name) : name(_name), UUID(randomGen.Int())
{
	aabb.SetNegativeInfinity();
	obb.SetNegativeInfinity();
}

GameObject::GameObject(const std::string& _name, const int _UUID) : name(_name), UUID(_UUID)
{
	aabb.SetNegativeInfinity();
	obb.SetNegativeInfinity();
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

	App->renderer->RemoveObjectFromDrawList(this);
}

void GameObject::Draw()
{
	if (name.compare("Scene 1") != 0 && drawOBB)
	{
		dd::box(obbPoints, float3(0.9f));
	}
	// draw components
	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
	{
		(*it)->Draw();
	}
}

bool GameObject::AddComponent(ComponentType _type, const int _UUID)
{
	bool createdComp = false;
	Component* newComp;
	switch (_type)
	{
	case TRANSFORM:
		if (!this->GetTransform())
		{
			if (_UUID != -1)
				newComp = new CTransform(this, _UUID);
			else
				newComp = new CTransform(this);
			transform = (CTransform*)newComp;
			components.push_back(newComp);
			createdComp = true;
		}
		break;
	case MESH:
		if (!this->GetComponent<CMesh>())
		{
			if (_UUID != -1)
				newComp = new CMesh(this, _UUID);
			else
				newComp = new CMesh(this);
			components.push_back(newComp);
			createdComp = true;
		}
		break;
	case MATERIAL:
		if (!this->GetComponent<CMaterial>())
		{
			if (_UUID != -1)
				newComp = new CMaterial(this, _UUID);
			else
				newComp = new CMaterial(this);
			components.push_back(newComp);
			createdComp = true;
		}
		break;
	case CAMERA:
		if (!this->GetComponent<CCamera>())
		{
			if (_UUID != -1)
				newComp = new CCamera(this, _UUID);
			else
				newComp = new CCamera(this);
			components.push_back(newComp);
			createdComp = true;
		}
		break;
	default:
		newComp = new Component(INVALID, this);
		components.push_back(newComp);		// TODO: Should we really be doing this?
		createdComp = true;
	}

	return createdComp;
}

void GameObject::RemoveComponent(int _cID)
{
	int toRemove = -1;

	for (unsigned int i = 0u; i < components.size(); ++i)
	{
		if (components[i]->UUID == _cID)
		{
			toRemove = (int)i;
			break;
		}
	}
	if (toRemove >= 0)
	{
		if (components[toRemove]->GetType() == TRANSFORM)
			transform = nullptr;
		if (components[toRemove]->GetType() == MESH)
			App->renderer->RemoveObjectFromDrawList(this);
		RELEASE(components[toRemove]);
		components.erase(components.begin() + toRemove);
		GetComponent<CMaterial>();
	}
}

void GameObject::SetParent(GameObject* _newParent)
{
	// Check if _newParent if a child of this GameObject
	bool isChild = false;
	for (unsigned int i = 0; i < this->GetChildren().size(); ++i)
	{
		if (this->children[i]->GetUUID() == _newParent->GetUUID())
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
			parent->RemoveChild(this->UUID);
		}
		parent = _newParent;
	}
}

void GameObject::AddChild(GameObject* _newChild)
{
	if (_newChild->transform && _newChild->parent != nullptr)
	{
		float4x4 worldTransform = _newChild->GetModelMatrix();
		float4x4 thisTransform = this->transform ? this->GetModelMatrix() : float4x4::identity;
		float3 thisScale = float3(thisTransform.Col3(0).Length(), thisTransform.Col3(1).Length(), thisTransform.Col3(2).Length());

		// Inverts the matrix with the fastest possible method
		if (thisScale.Equals(float3::one))					// No scaling
			thisTransform.InverseOrthonormal();
		else if (thisScale.xxx().Equals(thisScale))			// Uniform scaling
			thisTransform.InverseOrthogonalUniformScale();
		else												// Non uniform scaling
			thisTransform.InverseColOrthogonal();

		_newChild->SetTransform(thisTransform * worldTransform);
	}
	children.push_back(_newChild);
}

void GameObject::RemoveChild(int childID)
{
	int toRemove = -1;
	for (unsigned int i = 0u; i < children.size(); ++i)
	{
		if (children[i]->UUID == childID)
		{
			toRemove = (int)i;
			break;
		}
	}
	if (toRemove >= 0)
		children.erase(children.begin() + toRemove);
}

GameObject* GameObject::SearchChild(int childID)
{
	for (int i = 0; i < children.size(); ++i)
	{
		if (children[i]->UUID == childID)
		{
			return children[i];
		}
	}
	for (int i = 0; i < children.size(); ++i)
	{
		GameObject* go = children[i]->SearchChild(childID);
		if (go)
			return go;
	}
	return nullptr;
}

float4x4 GameObject::GetModelMatrix() const
{
	if (parent)
		return parent->GetModelMatrix() * (transform ? this->transform->GetTransformationMatrix() : float4x4::identity);
	else
		return  transform ? this->transform->GetTransformationMatrix() : float4x4::identity;
}

const float3 GameObject::GetAccumulatedScale() const
{
	if (parent)
		return parent->GetAccumulatedScale().Mul(transform ? this->transform->GetScale() : float3::one);
	else
		return transform ? this->transform->GetScale() : float3::one;
}

void GameObject::SetTransform(float3& _scale, float3& _rotation, float3& _translation)
{
	transform->SetPos(_translation);
	transform->SetRotation(_rotation);
	transform->SetScale(_scale);
	transform->UpdateTransformMatrix();
}

void GameObject::SetTransform(float4x4& _newTransform)
{
	transform->SetPos((float3)(_newTransform.Col3(3)));

	transform->SetScale(float3(_newTransform.Col3(0).Length(), _newTransform.Col3(1).Length(), _newTransform.Col3(2).Length()));

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
	if(GetComponent<CMesh>())
		GetComponent<CMesh>()->SetProgram(program);

	// update children accordingly
	for (std::vector<GameObject*>::iterator it = children.begin(); it != children.end(); ++it)
	{
		(*it)->SetProgram(program);
	}
}

void GameObject::UpdateBoundingBoxes()
{
	aabb.SetNegativeInfinity();
	CMesh* mesh = GetComponent<CMesh>();
	bool nonuniformScaling = false;
	if (mesh)
	{
		aabb.Enclose(mesh->GetAABBmin(), mesh->GetAABBmax());
		if (GetAccumulatedScale().MaxElement() != GetAccumulatedScale().MinElement()) // Non uniform scaling
		{
			nonuniformScaling = true;
			aabb.Scale(aabb.CenterPoint(), GetAccumulatedScale());
		}
	}

	for (GameObject* child : children)
		child->UpdateBoundingBoxes();

	obb = aabb.Transform(GetModelMatrix());
	if (nonuniformScaling)
		obb.Scale(obb.CenterPoint(), 1.0 / GetAccumulatedScale().x); // Correct scaling

	//Added, calculate obb vertices only when obb is updated
	ddVec3 points[8];
	obb.GetCornerPoints(points);
	ddVec3 points2[8] = { points[0], points[1], points[3], points[2], points[4], points[5], points[7], points[6] };
	memcpy(obbPoints, points2, sizeof(ddVec3) * 8);
}

void GameObject::UpdateOctreePosition()
{
	if (GetComponent<CMesh>())
		App->sceneMng->octree.UpdateGO(this);
	for (GameObject* child : children)
	{
		child->UpdateOctreePosition();
	}

}

float4 GameObject::ComputeCenterAndDistance() const
{
	if (GetComponent<CMesh>() != nullptr)
	{
		return float4(GetModelMatrix().Col3(3), obb.Size().Length() * 2.f);
	}
	else
	{
		float3 minPoint = float3::inf, maxPoint = -float3::inf;
		for (GameObject* child : children)
		{
			if (child->GetComponent<CMesh>() != nullptr)
			{
				minPoint = minPoint.Min(child->obb.MinimalEnclosingAABB().minPoint);
				maxPoint = maxPoint.Max(child->obb.MinimalEnclosingAABB().maxPoint);
			}
		}
		if (minPoint.Equals(float3::inf) || maxPoint.Equals(-float3::inf))
		{
			float4 ret = float4::zero;
			for (GameObject* child : children)
			{
				ret = child->ComputeCenterAndDistance();
			}
			return ret;
		}
		else
			return float4(GetModelMatrix().Col3(3), (maxPoint - minPoint).Length() * 2.f);
	}
}