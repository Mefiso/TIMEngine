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

		// Load vertex data
		float* vertices = new float[numVertices * 3];
		unsigned int j = 0; // auxiliar variable to write vertex xyz values contiguously in the 'vertices' array. Used also for normals, uvs...
		for (unsigned int i = 0u; i < numVertices; ++i)
		{
			j = i * 3;
			vertices[j] = _aimesh->mVertices[i].x;
			vertices[j+1] = _aimesh->mVertices[i].y;
			vertices[j+2] = _aimesh->mVertices[i].z;
		}
		cmesh->SetVertices(vertices);

		// Load normals data
		float* normals = nullptr;
		if (_aimesh->mNormals != NULL)
		{
			normals = new float[numVertices * 3];
			for (unsigned int i = 0u; i < numVertices; ++i)
			{
				j = i * 3;
				normals[j] = _aimesh->mNormals[i].x;
				normals[j + 1] = _aimesh->mNormals[i].y;
				normals[j + 2] = _aimesh->mNormals[i].z;
			}
			cmesh->SetNormals(normals);
		}

		// Load texture coordinates data
		float* uvs = nullptr;
		if (_aimesh->mTextureCoords[0] != NULL)
		{
			uvs = new float[numVertices * 2];
			for (unsigned int i = 0u; i < numVertices; ++i)
			{
				j = i * 2;
				uvs[j] = _aimesh->mTextureCoords[0][i].x;
				uvs[j + 1] = _aimesh->mTextureCoords[0][i].y;
			}
			cmesh->SetUVs(uvs);
		}

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
		cmesh->SetVBO(LoadVBO(cmesh, numVertices, vertices, normals, uvs));
		cmesh->SetEBO(LoadEBO(numIndices, indices));
		cmesh->SetVAO(CreateVAO(cmesh->GetVBO(), cmesh->GetEBO()));

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
		//							amount of indices /		amount of vertices /	HasNormals /				HasTexture_coords --------------------- / HasColors?
		unsigned int header[4] = { _mesh->GetNumIndices(), _mesh->GetNumVertices(), _mesh->GetNormals() ? 1u : 0u, _mesh->GetUVs() ? 1u : 0u };
		//					/header size	/ indices values								/ vertices values including:					vertices + normals	+				tex_coords				/ AABB values		/ EOF
		unsigned int size = sizeof(header) + sizeof(unsigned int) * _mesh->GetNumIndices() + sizeof(float) * _mesh->GetNumVertices() * (3 + (_mesh->GetNormals() ? 3 : 1) + (_mesh->GetUVs() ? 2 : 0)) + sizeof(float) * 6 + sizeof(char);

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
		//cursor += bytes;

		fwrite(fileBuffer, sizeof(char), size, f);
		LOG("[info] Mesh data stored into '%s' file", _filename);

		RELEASE_ARRAY(fileBuffer);
		fclose(f);
		return size;
	}
}

bool ImporterMesh::Load(const char* _filename, GameObject* _parent, unsigned int _filesize)
{
	if (_parent->AddComponent(MESH))
	{
		// Open file to read
		FILE* f;
		errno_t err;
		if ((err = fopen_s(&f, _filename, "rb")) != 0) {
			// File could not be opened. FILE* was set to NULL. error code is returned in err.
			LOG("[error] File could not be opened: %s", _filename, strerror(err));
			return false;
		}
		else {
			CMesh* cmesh = _parent->GetComponent<CMesh>();
			char* fileBuffer = new char[_filesize]; // Allocate
			char* cursor = fileBuffer;
			fread_s(fileBuffer, sizeof(char)* _filesize, sizeof(char), _filesize, f);
			
			// amount of indices / amount of vertices / HasNormals / HasTexture_coords
			unsigned int header[4];
			unsigned int bytes = sizeof(header);
			memcpy(header, cursor, bytes);
			cursor += bytes;

			cmesh->SetNumIndices(header[0]);
			cmesh->SetNumVertices(header[1]);
			bool hasNormals = header[2];
			bool hasTexCoords = header[3];

			// Load indices
			bytes = sizeof(unsigned int) * header[0];
			unsigned int* indices = new unsigned int[header[0]];
			memcpy(indices, cursor, bytes);
			cmesh->SetIndices(indices);
			cursor += bytes;

			// Load vertices
			bytes = sizeof(float) * header[1] * 3;
			float* vertices = new float[header[1] * 3];
			memcpy(vertices, cursor, bytes);
			cmesh->SetVertices(vertices);
			cursor += bytes;

			// Load normals
			float* normals = nullptr;
			if (hasNormals)
			{
				bytes = sizeof(float) * header[1] * 3;
				normals = new float[header[1] * 3];
				memcpy(normals, cursor, bytes);
				cmesh->SetNormals(normals);
				cursor += bytes;
			}
			
			// Load Texture coordinates
			float* uvs = nullptr;
			if (hasTexCoords)
			{
				bytes = sizeof(float) * header[1] * 2;
				uvs = new float[header[1] * 2];
				memcpy(uvs, cursor, bytes);
				cmesh->SetUVs(uvs);
				cursor += bytes;
			}

			// Load AABB
			bytes = sizeof(float) * 6;
			float AABB[6];
			memcpy(AABB, cursor, bytes);
			float3 max = float3(AABB[0], AABB[1], AABB[2]);
			float3 min = float3(AABB[3], AABB[4], AABB[5]);
			cmesh->SetAABBmax(max);
			cmesh->SetAABBmin(min);
			//cursor += bytes;

			// Load Buffer Objects
			// TODO: La AABB no fa falta pq ja la creem en el VBO?
			cmesh->SetVBO(LoadVBO(cmesh, header[1], vertices, normals, uvs));
			cmesh->SetEBO(LoadEBO(header[0], indices));
			cmesh->SetVAO(CreateVAO(cmesh->GetVBO(), cmesh->GetEBO()));

			RELEASE_ARRAY(fileBuffer);
			fclose(f);
			LOG("[info] Mesh data loaded from '%s' file", _filename);
			return true;
		}
	}
	return false;
}

