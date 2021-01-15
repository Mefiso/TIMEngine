#pragma once

#include "Geometry/AABB.h"
#include "Geometry/LineSegment.h"
#include <list>

class GameObject;

class OctreeNode
{
private:
	AABB box;
	std::list<GameObject*> objects;
	OctreeNode* children[8];
	OctreeNode* parent = nullptr;

public:
	OctreeNode(const AABB& box);
	~OctreeNode();

	void Insert(GameObject* go);
	bool Erase(GameObject* go);
	void OnErase(std::list<GameObject*>& childObjects);
	void CreateChildren();
	void ForwardToChildren();
	void Draw();
	void CollectFrustumIntersections(std::vector<GameObject*>& insideObjects, const float4 frustumPlanes[6], const float3 frustumPoints[8]);
	const void CollectLineIntersections(LineSegment _ray, std::list<GameObject*> &_intersectedObj) const;
	const AABB& GetBox() const { return box; }

private:
	bool BoxInFrustum(const AABB& box, const float4 frustumPlanes[6], const float3 frustumPoints[8]);
};

class Octree
{
private:
	OctreeNode* root = nullptr;
public:
	Octree();
	~Octree();
	void SetBoundaries(const AABB& box);

	void Insert(GameObject* go);
	void Erase(GameObject* go);
	void UpdateGO(GameObject* go);
	void CleanUp();
	void Draw();
	void CollectFrustumIntersections(std::vector<GameObject*>& insideObjects, const float4 frustumPlanes[6], const float3 frustumPoints[8]);

	const OctreeNode* GetRoot() const { return root; }
};
