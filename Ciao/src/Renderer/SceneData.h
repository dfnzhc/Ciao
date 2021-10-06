#pragma once
#include "Scene/Scene.h"
#include "Scene/VtxData.h"


namespace Ciao
{
	class Texture;
	struct MaterialDescription;
	struct DrawData;

	struct SceneData
	{
		SceneData(const char* meshFile, const char* sceneFile, const char* materialFile);
		void loadScene(const char* sceneFile);


		std::vector<Texture> allMaterialTextures_;

		MeshFileHeader header_;
		MeshData meshData_;

		Scene scene_;
		std::vector<MaterialDescription> materials_;
		std::vector<DrawData> shapes_;
	};
}
