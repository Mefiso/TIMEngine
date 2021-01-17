#include "ImporterMesh.h"
#include "Application.h"
#include "ModuleSceneManager.h"
#include "GameObject.h"

bool ImporterMesh::Import(const aiMesh* _aimesh, GameObject* _owner)
{
	if (_owner->AddComponent(MESH))
	{
		CMesh* cmesh = _owner->GetComponent<CMesh>();
		unsigned int numVertices = _aimesh->mNumVertices;
		unsigned int numIndices = _aimesh->mNumFaces * 3u;
		cmesh->SetNumVertices(numVertices);
		cmesh->SetNumIndices(numIndices);
		unsigned int vtxSize = 3u;
		if (_aimesh->mNormals != NULL)
			vtxSize += 3u;
		if (_aimesh->mTextureCoords[0] != NULL)
			vtxSize += 2u;
		cmesh->SetVtxSize(vtxSize);

		// Interleaved vertices
		float* interleaved = new float[numVertices * vtxSize];
		unsigned int j = 0; // auxiliar variable to write vertex data contiguously
		for (unsigned int i = 0u; i < numVertices; ++i)
		{
			//j = i * vtxSize;
			interleaved[j++] = _aimesh->mVertices[i].x;
			interleaved[j++] = _aimesh->mVertices[i].y;
			interleaved[j++] = _aimesh->mVertices[i].z;
			float vtx[3] = { _aimesh->mVertices[i].x, _aimesh->mVertices[i].y, _aimesh->mVertices[i].z };
			CheckMinMaxPoints(vtx, cmesh);	// Generate AABB min and max points

			if (_aimesh->mNormals != NULL)
			{
				interleaved[j++] = _aimesh->mNormals[i].x;
				interleaved[j++] = _aimesh->mNormals[i].y;
				interleaved[j++] = _aimesh->mNormals[i].z;
			}
			if (_aimesh->mTextureCoords[0] != NULL)
			{
				interleaved[j++] = _aimesh->mTextureCoords[0][i].x;
				interleaved[j++] = _aimesh->mTextureCoords[0][i].y;
			}
		}
		cmesh->SetVertices(interleaved);

		// Load indices
		unsigned int* indices = new unsigned int[numIndices];
		for (unsigned int i = 0u; i < _aimesh->mNumFaces; ++i)
		{
			j = i * 3;
			indices[j] = _aimesh->mFaces[i].mIndices[0];
			indices[j+1] = _aimesh->mFaces[i].mIndices[1];
			indices[j+2] = _aimesh->mFaces[i].mIndices[2];
		}
		cmesh->SetIndices(indices);

		// Load Buffer Objects
		cmesh->SetVBO(LoadVBO(cmesh, numVertices, interleaved, vtxSize));
		cmesh->SetEBO(LoadEBO(numIndices, indices));
		cmesh->SetVAO(CreateVAO(cmesh->GetVBO(), cmesh->GetEBO()));

		_owner->UpdateBoundingBoxes();
		App->sceneMng->octree.Insert(_owner);

		return true;
	}
	return false;
}

unsigned int ImporterMesh::Save(CMesh* _mesh, const char* _filename)
{
	// Open file to write
	FILE* f;
	errno_t err;
	if ((err = fopen_s(&f, _filename, "wb")) != 0) {
		// File could not be opened. FILE* was set to NULL. error code is returned in err.
		LOG("[error] File could not be opened: %s", _filename, strerror(err));
		return 0;
	}
	else {
		// amount of indices / amount of vertices / Size of Vertex
		unsigned int header[3] = { _mesh->GetNumIndices(), _mesh->GetNumVertices(), _mesh->GetVtxSize()};
		// header size + indices list size + vertices list size + AABB size + EOF
		unsigned int size = sizeof(header) + sizeof(unsigned int) * _mesh->GetNumIndices() + sizeof(float) * _mesh->GetNumVertices() * _mesh->GetVtxSize() + sizeof(float) * 6 + sizeof(char);

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
		bytes = sizeof(float) * _mesh->GetNumVertices() * _mesh->GetVtxSize();
		memcpy(cursor, _mesh->GetVertices(), bytes);
		cursor += bytes;

		// Store AABB
		bytes = sizeof(float) * 6;
		float3 max = _mesh->GetAABBmax();
		float3 min = _mesh->GetAABBmin();
		float AABB[6] = { max.x, max.y, max.z, min.x, min.y, min.z };
		memcpy(cursor, AABB, bytes);

		fwrite(fileBuffer, sizeof(char), size, f);
		LOG("[info] Mesh data stored into '%s' file", _filename);

		RELEASE_ARRAY(fileBuffer);
		fclose(f);
		return size;
	}
}

bool ImporterMesh::Load(const char* _filename, GameObject* _owner, unsigned int _filesize)
{
	// Open file to read
	FILE* f;
	errno_t err;
	if ((err = fopen_s(&f, _filename, "rb")) != 0) {
		// File could not be opened. FILE* was set to NULL. error code is returned in err.
		LOG("[error] File could not be opened: %s. %s", _filename, strerror(err));
		return false;
	}
	else {
			CMesh* cmesh = _owner->GetComponent<CMesh>();
			std::string fn = std::string(_filename);
			cmesh->filename = fn.substr(0, fn.find_last_of('/')).size() == fn.size() ? fn.substr(fn.find_last_of('\\') + 1, fn.size()) : fn.substr(fn.find_last_of('/') + 1, fn.size());
			cmesh->filesize = _filesize;

			char* fileBuffer = new char[_filesize]; // Allocate
			char* cursor = fileBuffer;
			fread_s(fileBuffer, sizeof(char) * _filesize, sizeof(char), _filesize, f);

			// amount of indices / amount of vertices / Size of Vertex
			unsigned int header[3];
			unsigned int bytes = sizeof(header);
			memcpy(header, cursor, bytes);
			cursor += bytes;

			cmesh->SetNumIndices(header[0]);
			cmesh->SetNumVertices(header[1]);
			cmesh->SetVtxSize(header[2]);

			// Load indices
			bytes = sizeof(unsigned int) * header[0];
			unsigned int* indices = new unsigned int[header[0]];
			memcpy(indices, cursor, bytes);
			cmesh->SetIndices(indices);
			cursor += bytes;

			// Load vertices
			bytes = sizeof(float) * header[1] * header[2];	//float * num_vertex * size_vertex
			float* vertices = new float[header[1] * header[2]];
			memcpy(vertices, cursor, bytes);
			cmesh->SetVertices(vertices);
			cursor += bytes;

			// Load AABB
			bytes = sizeof(float) * 6;
			float AABB[6];
			memcpy(AABB, cursor, bytes);
			float3 max = float3(AABB[0], AABB[1], AABB[2]);
			float3 min = float3(AABB[3], AABB[4], AABB[5]);
			cmesh->SetAABBmax(max);
			cmesh->SetAABBmin(min);

			// Load Buffer Objects
			cmesh->SetVBO(LoadVBO(cmesh, header[1], vertices, header[2]));
			cmesh->SetEBO(LoadEBO(header[0], indices));
			cmesh->SetVAO(CreateVAO(cmesh->GetVBO(), cmesh->GetEBO()));

			_owner->UpdateBoundingBoxes();
			App->sceneMng->octree.Insert(_owner);

			RELEASE_ARRAY(fileBuffer);
			fclose(f);
			LOG("[info] Mesh data loaded from '%s' file", _filename);
			return true;
	}
	return false;
}

