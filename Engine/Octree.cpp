#include "Globals.h"
#include "Octree.h"
#include "GameObject.h"
#include "Leaks.h"
#include "debugdraw.h"

#define FUR 0
#define FDR 1
#define FDL 2
#define FUL 3
#define BUR 4
#define BDR 5
#define BDL 6
#define BUL 7

#define CAPACITY 2

OctreeNode::OctreeNode(const AABB& box) : box(box)
{
	for (int i = 0; i < 8; ++i)
		children[i] = nullptr;
}

OctreeNode::~OctreeNode()
{
	for (int i = 0; i < 8; ++i)
		if (children[i] != nullptr)
			RELEASE(children[i]);
}

void OctreeNode::Insert(GameObject* go)
{
	// If it isn't a leave
	if (children[0] != nullptr)
	{
		for (int i = 0; i < 8; ++i)
		{
			// If it intersects with any octree childs
			if (children[i]->box.Intersects(go->GetOBB().MinimalEnclosingAABB()))
			{
				children[i]->Insert(go);
			}
		}
	}
	else
	{
		objects.push_back(go);
		if (objects.size() > CAPACITY)
		{
			CreateChildren();
			ForwardToChildren();
		}

	}
}

bool OctreeNode::Erase(GameObject* go)
{
	bool ret = false;
	std::list<GameObject*>::iterator it = std::find(objects.begin(), objects.end(), go);
	if (it != objects.end())
	{
		objects.erase(it);
		ret = true;
	}
	// Check if it's not a leaf
	if (children[0] != nullptr)
	{
		for (int i = 0; i < 8; ++i)
			ret = children[i]->Erase(go) ? true : ret;
		if (ret)
			OnChildrenErase();
	}

	return ret;
}

void OctreeNode::OnChildrenErase()
{
	bool allEmpty = true;
	for (int i = 0; i < 8; ++i)
	{
		if (!children[i]->objects.empty())
			allEmpty = false;
	}
	if (allEmpty)
	{
		for (int i = 0; i < 8; ++i)
		{
			RELEASE(children[i])
		}
	}
}

void OctreeNode::CreateChildren()
{
	// Divide this node
	float3 newSize(box.HalfSize());
	float3 center(box.CenterPoint());

	AABB newBox;
	float3 newCenter;

	// Front up right
	newCenter.x = center.x + newSize.x * 0.5f;
	newCenter.y = center.y + newSize.y * 0.5f;
	newCenter.z = center.z + newSize.z * 0.5f;
	newBox.SetFromCenterAndSize(newCenter, newSize);
	children[FUR] = new OctreeNode(newBox);
	children[FUR]->parent = this;

	// Front down right
	newCenter.x = center.x + newSize.x * 0.5f;
	newCenter.y = center.y - newSize.y * 0.5f;
	newCenter.z = center.z + newSize.z * 0.5f;
	newBox.SetFromCenterAndSize(newCenter, newSize);
	children[FDR] = new OctreeNode(newBox);
	children[FDR]->parent = this;

	// Front down left
	newCenter.x = center.x - newSize.x * 0.5f;
	newCenter.y = center.y - newSize.y * 0.5f;
	newCenter.z = center.z + newSize.z * 0.5f;
	newBox.SetFromCenterAndSize(newCenter, newSize);
	children[FDL] = new OctreeNode(newBox);
	children[FDL]->parent = this;

	// Front up left
	newCenter.x = center.x - newSize.x * 0.5f;
	newCenter.y = center.y + newSize.y * 0.5f;
	newCenter.z = center.z + newSize.z * 0.5f;
	newBox.SetFromCenterAndSize(newCenter, newSize);
	children[FUL] = new OctreeNode(newBox);
	children[FUL]->parent = this;

	// Back up right
	newCenter.x = center.x + newSize.x * 0.5f;
	newCenter.y = center.y + newSize.y * 0.5f;
	newCenter.z = center.z - newSize.z * 0.5f;
	newBox.SetFromCenterAndSize(newCenter, newSize);
	children[BUR] = new OctreeNode(newBox);
	children[BUR]->parent = this;

	// Back down right
	newCenter.x = center.x + newSize.x * 0.5f;
	newCenter.y = center.y - newSize.y * 0.5f;
	newCenter.z = center.z - newSize.z * 0.5f;
	newBox.SetFromCenterAndSize(newCenter, newSize);
	children[BDR] = new OctreeNode(newBox);
	children[BDR]->parent = this;

	// Back down left
	newCenter.x = center.x - newSize.x * 0.5f;
	newCenter.y = center.y - newSize.y * 0.5f;
	newCenter.z = center.z - newSize.z * 0.5f;
	newBox.SetFromCenterAndSize(newCenter, newSize);
	children[BDL] = new OctreeNode(newBox);
	children[BDL]->parent = this;

	// Back up left
	newCenter.x = center.x - newSize.x * 0.5f;
	newCenter.y = center.y + newSize.y * 0.5f;
	newCenter.z = center.z - newSize.z * 0.5f;
	newBox.SetFromCenterAndSize(newCenter, newSize);
	children[BUL] = new OctreeNode(newBox);
	children[BUL]->parent = this;
}

void OctreeNode::ForwardToChildren()
{
	for (std::list<GameObject*>::iterator it = objects.begin(), end = objects.end(); it != end;)
	{
		GameObject* go = *it;

		bool intersects[8];
		bool intersectsAll = true;
		for (int i = 0; i < 8; ++i)
		{
			intersects[i] = children[i]->box.Intersects(go->GetOBB().MinimalEnclosingAABB());
			if (!intersects[i])
				intersectsAll = false;
		}

		if (intersectsAll)
			++it;
		else
		{
			it = objects.erase(it);
			for (int i = 0; i < 8; ++i)
				if (intersects[i])
					children[i]->Insert(go);

		}
	}
}

void OctreeNode::Draw()
{
	dd::aabb(box.minPoint, box.maxPoint, float3(0.1f, 1.0f, 0.3f));
	if (children[0] != nullptr)
	{
		for (int i = 0; i < 8; ++i)
			children[i]->Draw();
	}
}

Octree::Octree()
{
}

Octree::~Octree()
{
	CleanUp();
}

void Octree::SetBoundaries(const AABB& box)
{
	CleanUp();
	root = new OctreeNode(box);
}

void Octree::Insert(GameObject* go)
{
	if (root)
	{
		if (go->GetOBB().MinimalEnclosingAABB().Intersects(root->GetBox()))
			root->Insert(go);
	}
}

void Octree::Erase(GameObject* go)
{
	if (root)
		root->Erase(go);
}

void Octree::UpdateGO(GameObject* go)
{
	Erase(go);
	Insert(go);
}

void Octree::CleanUp()
{
	RELEASE(root);
}

void Octree::Draw()
{
	root->Draw();
}
