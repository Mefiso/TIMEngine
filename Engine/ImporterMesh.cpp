#include "ImporterMesh.h"
#include "GameObject.h"

bool ImporterMesh::Import(const aiMesh* _aimesh, GameObject* _parent)
{
	if (_parent->AddComponent(MESH))
	{
		CMesh* cmesh = _parent->GetComponent<CMesh>();
		unsigned int numVertices = _aimesh->mNumVertices;
		unsigned int numIndices = _aimesh->mNumFaces * 3u;
		cmesh->SetNumVertices(numVertices);
		cmesh->SetNumIndices(numIndices);

		// Load Buffer Objects
		cmesh->SetVBO(LoadVBO(_aimesh, numVertices, cmesh));
		cmesh->SetEBO(LoadEBO(_aimesh, numIndices));
		cmesh->SetVAO(CreateVAO(cmesh->GetVBO(), cmesh->GetEBO()));

		// Load vertex data
		float* vertices = new float[numVertices*3];
		for (unsigned int i = 0u; i < numVertices; i=i+3)
		{
			vertices[i] = _aimesh->mVertices[i].x;
			vertices[i+1] = _aimesh->mVertices[i].y;
			vertices[i+2] = _aimesh->mVertices[i].z;
		}
		cmesh->SetVertices(vertices);

		// Load normals data
		if (_aimesh->mNormals != NULL)
		{
			float* normals = new float[numVertices * 3];
			for (unsigned int i = 0u; i < numVertices; i=i+3)
			{
				normals[i] = _aimesh->mNormals[i].x;
				normals[i+1] = _aimesh->mNormals[i].y;
				normals[i+2] = _aimesh->mNormals[i].z;
			}
			cmesh->SetNormals(normals);
		}

		// Load texture coordinates data
		if (_aimesh->mTextureCoords[0] != NULL)
		{
			float* uvs = new float[numVertices * 2];
			for (unsigned int i = 0u; i < numVertices; i=i+2)
			{
				uvs[i] = _aimesh->mTextureCoords[0][i].x;
				uvs[i+1] = _aimesh->mTextureCoords[0][i].y;
			}
			cmesh->SetUVs(uvs);
		}

		// Load indices
		unsigned int* indices = new unsigned int[numIndices];
		for (unsigned int i = 0u; i < _aimesh->mNumFaces; ++i)
		{
			indices[i] = _aimesh->mFaces[i].mIndices[0];
			indices[i+1] = _aimesh->mFaces[i].mIndices[1];
			indices[i+2] = _aimesh->mFaces[i].mIndices[2];
		}
		cmesh->SetIndices(indices);

		return true;
	}
	return false;
}

unsigned int ImporterMesh::Save(CMesh* _mesh, const char* _filename)
{
	//							amount of indices /		amount of vertices /	HasNormals /				HasTexture_coords --------------------- / HasColors?
	unsigned int header[5] = { _mesh->GetNumIndices(), _mesh->GetNumVertices(), _mesh->GetNormals() ? 1u:0u, _mesh->GetUVs() ? 1u:0u, };
	//					header size /		indices values /							vertices values including:				vertices + normals	+			tex_coords /			AABB values
	unsigned int size = sizeof(header) + sizeof(unsigned int)*_mesh->GetNumIndices() + sizeof(float)*_mesh->GetNumVertices() * (3 + (_mesh->GetNormals() ? 3 : 1) + (_mesh->GetUVs() ? 2 : 0)) + sizeof(float)*6;

	char* fileBuffer = new char[size]; // Allocate
	char* cursor = fileBuffer;

	// First store header ranges
	unsigned int bytes = sizeof(header);
	memcpy(cursor, header, bytes);
	cursor += bytes;

	// Store indices
	bytes = sizeof(unsigned int) * _mesh->GetNumIndices();
	memcpy(cursor, _mesh->GetIndices(), bytes);
	cursor += bytes;

	// Store vertices
	bytes = sizeof(float) * _mesh->GetNumVertices() * 3;
	memcpy(cursor, _mesh->GetVertices(), bytes);
	cursor += bytes;

	// Store normals
	if (_mesh->GetNormals())
	{
		bytes = sizeof(float) * _mesh->GetNumVertices() * 3;
		memcpy(cursor, _mesh->GetNormals(), bytes);
		cursor += bytes;
	}

	// Store uvs
	if (_mesh->GetUVs())
	{
		bytes = sizeof(float) * _mesh->GetNumVertices() * 2;
		memcpy(cursor, _mesh->GetUVs(), bytes);
		cursor += bytes;
	}

	// Store AABB
	bytes = sizeof(float) * 6;
	float3 max = _mesh->GetAABBmax();
	float3 min = _mesh->GetAABBmin();
	float AABB[6] = { max.x, max.y, max.z, min.x, min.y, min.z };
	memcpy(cursor, AABB, bytes);
	cursor += bytes;

	// Write buffer to file
	FILE *f;
	f = fopen(_filename, "w");
	if (f == NULL)
	{
		LOG("[error] Could not store the mesh data into '%s' file", _filename);
		return -1;
	}
	else {
		fwrite(fileBuffer, sizeof(char), size, f);
		LOG("[info] Mesh data stored into '%s' file", _filename);
	}
	fclose(f);
	delete[] fileBuffer;

	return 0;
}

