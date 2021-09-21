#pragma once

#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include <string>
#include <unordered_map>
#include <vector>

using glm::mat4;

namespace Ciao
{
    constexpr const int MAX_NODE_LEVEL = 16;

    struct Hierarchy
    {
        // parent for this node (or -1 for root)
        // 该节点的父亲索引，对根节点该值为 -1
        int parent_;
        // first child for a node (or -1)
        // 第一个孩子的索引
        int firstChild_;
        // next sibling for a node (or -1)
        // 下一个兄弟节点索引
        int nextSibling_;
        // last added node (or -1)
        // 最后一个兄弟节点索引
        int lastSibling_;
        // cached node level
        // 该节点所在层级
        int level_;
    };

    /*  This scene is converted into a descriptorSet(s) in MultiRenderer class
    *  这个场景被转换为 MultiRenderer 类中的描述符集
    This structure is also used as a storage type in SceneExporter tool
    同时也作为 SceneExporter 工具的存储类型
    */
    struct Scene
    {
        // local transformations for each node and global transforms
        // 所有节点的本地变换和全局变换
        // + an array of 'dirty/changed' local transforms
        // 
        std::vector<mat4> localTransform_;
        std::vector<mat4> globalTransform_;

        // list of nodes whose global transform must be recalculated
        // 必须重新计算全局变换的节点
        std::vector<int> changedAtThisFrame_[MAX_NODE_LEVEL];

        // Hierarchy component
        // 层级结构组件
        std::vector<Hierarchy> hierarchy_;

        // Mesh component: Which node corresponds to which node
        // Mesh 组件。表示哪个节点对应哪个 Mesh
        std::unordered_map<uint32_t, uint32_t> meshes_;

        // Material component: Which material belongs to which node
        // 材质: 保存材质和节点的对应信息
        std::unordered_map<uint32_t, uint32_t> materialForNode_;

        // Node name component: Which name is assigned to the node
        // 节点名称: 各节点被赋予的名字
        std::unordered_map<uint32_t, uint32_t> nameForNode_;

        // List of scene node names
        // 场景节点名字
        std::vector<std::string> names_;

        // Debug list of material names
        // 用于 Debug 的材质名称
        std::vector<std::string> materialNames_;
    };

    int addNode(Scene& scene, int parent, int level);

    void markAsChanged(Scene& scene, int node);

    int findNodeByName(const Scene& scene, const std::string& name);

    inline std::string getNodeName(const Scene& scene, int node)
    {
        int strID = scene.nameForNode_.find(node) != scene.nameForNode_.end() ? scene.nameForNode_.at(node) : -1;
        return (strID > -1) ? scene.names_[strID] : std::string();
    }

    inline void setNodeName(Scene& scene, int node, const std::string& name)
    {
        uint32_t stringID = (uint32_t)scene.names_.size();
        scene.names_.push_back(name);
        scene.nameForNode_[node] = stringID;
    }

    int getNodeLevel(const Scene& scene, int n);

    void recalculateGlobalTransforms(Scene& scene);

    void loadScene(const char* fileName, Scene& scene);
    void saveScene(const char* fileName, const Scene& scene);

    void dumpTransforms(const char* fileName, const Scene& scene);
    void printChangedNodes(const Scene& scene);

    void dumpSceneToDot(const char* fileName, const Scene& scene, int* visited = nullptr);

    void mergeScenes(Scene& scene, const std::vector<Scene*>& scenes, const std::vector<glm::mat4>& rootTransforms, const std::vector<uint32_t>& meshCounts,
            bool mergeMeshes = true, bool mergeMaterials = true);

    // Delete a collection of nodes from a scenegraph
    void deleteSceneNodes(Scene& scene, const std::vector<uint32_t>& nodesToDelete);
}