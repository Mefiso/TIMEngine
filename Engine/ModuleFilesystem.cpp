#include "ModuleFilesystem.h"

#include "ImporterScene.h"

#include "ModuleSceneManager.h" // Only for skybox loading. Should be removed when fixed hardcoding
#include "GL/glew.h"			// And this two should not be here
#include "ImporterMaterial.h"
#include <commdlg.h>
#include <direct.h>

ModuleFilesystem::ModuleFilesystem()
{
}

ModuleFilesystem::~ModuleFilesystem()
{
	CleanUp();
}

bool ModuleFilesystem::Start()
{
	//ImporterScene::Import("./resources/models/baker_house/BakerHouse.fbx");
	std::vector<std::string> faces =
	{
		".\\resources\\skybox\\default\\right.jpg",
		".\\resources\\skybox\\default\\left.jpg",
		".\\resources\\skybox\\default\\top.jpg",
		".\\resources\\skybox\\default\\bottom.jpg",
		".\\resources\\skybox\\default\\front.jpg",
		".\\resources\\skybox\\default\\back.jpg"
	};
	App->sceneMng->SetSkyboxTexture(ImporterMaterial::LoadCubemap(faces));
	return true;
}

bool ModuleFilesystem::CleanUp()
{
	for (std::vector<Texture*>::iterator it = loadedTextures.begin(), end = loadedTextures.end(); it != end; ++it)
	{
		glDeleteTextures(1, &((*it)->id));
		RELEASE(*it);
	}
	loadedTextures.clear();

	return true;
}

void ModuleFilesystem::DropFile(const std::string& _file)
{
	std::string ext = _file.substr(_file.find_last_of('.'), _file.size());

	// FBX or OBJ files -> Load a scene
	if (ext.compare(".fbx") == 0 || ext.compare(".FBX") == 0 || ext.compare(".obj") == 0 || ext.compare(".OBJ") == 0)
		ImporterScene::Import(_file);
}

std::string ModuleFilesystem::OpenDialog(const char* _filters, const char* _dir, int* _size) const
{
	char path[MAX_PATH];
	_fullpath(path, NULL, MAX_PATH);
	char filename[MAX_PATH];
	OPENFILENAME ofn;
	HANDLE hf;

	ZeroMemory(&filename, sizeof(filename));
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;  // If you have a window to center over, put its HANDLE here
	ofn.lpstrFile = filename;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = _filters;// "Text Files\0*.txt\0Any File\0*.*\0";
	ofn.lpstrTitle = "Select a File";
	ofn.lpstrInitialDir = _dir;
	ofn.Flags = OFN_DONTADDTORECENT | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileNameA(&ofn))
	{
		hf = CreateFile(ofn.lpstrFile, GENERIC_READ, 0,
			(LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
		LARGE_INTEGER lFileSize;
		GetFileSizeEx(hf, &lFileSize);
		*_size = lFileSize.QuadPart;
		CloseHandle(hf);
		_chdir(path);
		return filename;
	}
	else
		_chdir(path);
		return "\0";
}

