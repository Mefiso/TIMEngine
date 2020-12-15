#pragma once
#include "Window.h"
#include "ModuleTexture.h"
#include <Math/float4x4.h>

class Model;
class CMesh;
class GameObject;

class WProperties : public Window
{
private:

	// Should it be a pointer to a vector?
	std::vector<Texture*> selectedTextures;					// Retrieves the Textures loaded in a model
	GameObject* selectedObject = nullptr;								// Retrieves the Meshes loaded in a model
	float3 scale = float3::zero;							// Gets the scale transform of a Model
	float3 rotation = float3::zero;							// Gets the rotation transform of a Model
	float3 translation = float3::zero;						// Gets the translation transform of a Model

public:

	WProperties(std::string _name);							// Constructor
	~WProperties();											// Destructor

	void Draw();											// Operations performed when Rendering this window
	void SetInspectedObject(GameObject* _object);			// Loads the information to display of a Model given

private:

	void TransformationHeader() const;						// Prints the Transformation information of the selected Model into the window (translation, rotation, scale)
	void MeshHeader(CMesh* mesh) const;						// Prints the Geometry information for each mesh in the Model into the window (Number of meshes, textures, triangles, faces...)
	void TexturesHeader() const;							// Prints the Texture information for each texture in the Model into the window (Size, wrapping, min and mag filters, preview)
};