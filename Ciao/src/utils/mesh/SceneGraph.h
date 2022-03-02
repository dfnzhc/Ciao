#pragma once

namespace Ciao
{
    constexpr int MAX_NODE_LEVEL = 16;

    // 使用左孩子右兄弟的方法组织结构
    struct Hierarchy
    {
        // 父亲节点的索引，对根节点该值为 -1
        int parent_ = -1;
        
        // 第一个孩子的索引
        int firstChild_ = -1;
        
        // 下一个兄弟节点索引
        int nextSibling_ = -1;
        
        // 最后一个兄弟节点索引
        int lastSibling_ = -1;
        
        // 该节点所在层级
        int level_ = -1;
    };
    
    struct SceneGraph
    {
        // 所有节点的变换矩阵，分为本地和全局的
        std::vector<mat4> localTransform_;
        std::vector<mat4> globalTransform_;

        // 每个级别需要更新的节点
        std::vector<int> changedAtThisFrame_[MAX_NODE_LEVEL];

        // 层级结构组件
        std::vector<Hierarchy> hierarchy_;

        // 节点与 mesh 之间的映射
        std::unordered_map<uint32_t, uint32_t> meshes_;

        // 节点与材质间的映射
        std::unordered_map<uint32_t, uint32_t> materialForNode_;

        // 节点和命名之间的映射
        std::unordered_map<uint32_t, uint32_t> nameForNode_;

        // 场景图中节点的名字
        std::vector<std::string> names_;

        // 场景图中材质的名字
        std::vector<std::string> materialNames_;
    };

    inline std::string GetNodeName(const SceneGraph& scene, int node);

    inline void SetNodeName(SceneGraph& scene, int node, const std::string& name);

    // 添加一个节点
    int AddNode(SceneGraph& scene, int parent, int level);

    // 标记一个变化的节点
    void MarkAsChanged(SceneGraph& scene, int node);

    // 通过名字查找节点
    int FindNodeByName(const SceneGraph& scene, const std::string& name);

    // 重新计算全局变换
    void RecalculateGlobalTransforms(SceneGraph& scene);

    // 从文件中加载场景图
    void LoadSceneGraph(const char* fileName, SceneGraph& scene);
    // 保存一个场景图
    void SaveSceneGraph(const char* fileName, const SceneGraph& scene);

    void MergeScenes(SceneGraph& scene, const std::vector<SceneGraph*>& scenes, const std::vector<glm::mat4>& rootTransforms,
                     const std::vector<uint32_t>& meshCounts,
                     bool mergeMeshes = true, bool mergeMaterials = true);

    // Delete a collection of nodes from a scenegraph
    void DeleteSceneNodes(SceneGraph& scene, const std::vector<uint32_t>& nodesToDelete);
    
}
