#include "pch.h"
#include "SceneConvert.h"

#include "rapidjson/istreamwrapper.h"
#include "rapidjson/document.h"
#include "rapidjson/rapidjson.h"

namespace fs = std::filesystem;

namespace Ciao
{
    std::vector<SceneConfig> readConfigFile(const char* cfgFileName)
    {
        std::ifstream ifs(cfgFileName);
        if (!ifs.is_open())
        {
            CIAO_CORE_ERROR("Failed to load configuration file.");
            exit(EXIT_FAILURE);
        }

        rapidjson::IStreamWrapper isw(ifs);
        rapidjson::Document document;
        const rapidjson::ParseResult parseResult = document.ParseStream(isw);
        assert(!parseResult.IsError());
        assert(document.IsArray());

        std::vector<SceneConfig> configList;


        for (rapidjson::SizeType i = 0; i < document.Size(); i++)
        {
            configList.emplace_back(SceneConfig{
                .fileName = document[i]["input_scene"].GetString(),
                .outputMesh = document[i]["output_mesh"].GetString(),
                .outputScene = document[i]["output_scene"].GetString(),
                .outputMaterials = document[i]["output_materials"].GetString(),
                .scale = (float)document[i]["scale"].GetDouble(),
                .calculateLODs = document[i]["calculate_LODs"].GetBool(),
                .mergeInstances = document[i]["merge_instances"].GetBool()
            });
        }

        return configList;
    }

    void SceneConvert(const char* sceneConfigFile)
    {
    }
}
