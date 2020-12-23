#pragma once
#include "Window.h"
#include "ModuleTexture.h"
#include "GL/glew.h"
#include <Math/float4x4.h>

class Component;
class CMaterial;
class CMesh;
class GameObject;

class WProperties : public Window
{
private:
	GameObject* selectedObject = nullptr;					// Retrieves the Meshes loaded in a model
	Component* toDelete = nullptr;							// Pointer to a Component to which the 'Remove Component' option has been chosen, from the Options Menu
	float3 scale = float3::zero;							// Gets the scale transform of a Model
	float3 rotation = float3::zero;							// Gets the rotation transform of a Model
	float3 translation = float3::zero;						// Gets the translation transform of a Model

	const std::vector<GLint> wrapmode = { GL_REPEAT, GL_CLAMP, GL_CLAMP_TO_BORDER, GL_MIRRORED_REPEAT };																	 // Wrap mode options for the current texture.
	const std::vector<GLint> filtermode = { GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_LINEAR, GL_NEAREST }; // Filter mode options for the current texture.

	char RenameBuf[30];										// Buffer that stores the name of the selected GameObject, used by an InputText allowing the user to change the name of that GameObject

public:
	WProperties(std::string _name);							// Constructor
	~WProperties();											// Destructor

	void SetInspectedObject(GameObject* _object);			// Loads the information to display of a Model given
	void Draw();											// Operations performed when Rendering this window

	// ---------- Getters ---------- //
	Component* GetToDelete() const { return toDelete; }

	// ---------- Setters ---------- //
	void SetToDelete(Component* _component) { toDelete = _component; }

private:
	void DrawComponentHeader(Component* _component);		// Draws the Components information of the Selected GameObject
	void DrawTransformationBody();							// Subfunction of DrawComponentHeader. Draws the Transformation information of the selected GameObject into the window (translation, rotation, scale)
	void DrawMeshBody(CMesh* mesh);							// Subfunction of DrawComponentHeader. Draws the Geometry information for each mesh in the GameObject into the window (Number of meshes, textures, triangles, faces...)
	void DrawMaterialBody(CMaterial* material);				// Subfunction of DrawComponentHeader. Draws the Texture information for each texture in the GameObject into the window (Size, wrapping, min and mag filters, preview)
};