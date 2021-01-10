#include "ImporterMesh.h"
#include "GameObject.h"

void ImporterMesh::Import(const aiMesh* _aimesh, GameObject* _parent)
{
	if (_parent->AddComponent(MESH))
	{
		CMesh* cmesh = _parent->GetComponent<CMesh>();
		unsigned int numVertices = _aimesh->mNumVertices;
		unsigned int numIndices = _aimesh->mNumFaces * 3u;
		LoadVBO(_aimesh, numVertices, cmesh->GetVBO(), cmesh);
		LoadEBO(_aimesh, numIndices, cmesh->GetEBO());
		CreateVAO(cmesh->GetVBO(), cmesh->GetEBO(), cmesh->GetVAO());
	}
}

