#include "pch.h"
#include "SceneGraph.h"

#include "utils/ReadFile.h"

namespace Ciao
{
    inline std::string GetNodeName(const SceneGraph& scene, int node)
    {
        int nameID = scene.nameForNode_.find(node) != scene.nameForNode_.end() ? scene.nameForNode_.at(node) : -1;
        return (nameID != -1) ? scene.names_[nameID] : std::string();
    }

    inline void SetNodeName(SceneGraph& scene, int node, const std::string& name)
    {
        uint32_t nameID = (uint32_t)scene.names_.size();
        scene.names_.push_back(name);
        scene.nameForNode_[node] = nameID;
    }

    
    int AddNode(SceneGraph& scene, int parent, int level)
    {
        // 新添加节点的索引值
        int node = (int)scene.hierarchy_.size();
        {
            scene.localTransform_.push_back(mat4{ 1.0f });
            scene.globalTransform_.push_back(mat4{ 1.0f });
        }

        scene.hierarchy_.push_back({ .parent_ = parent, .lastSibling_ = -1 });

        // 把节点加入到层次结构之后，需要更新节点间的关系
        if (parent != -1)
        {
            // s 表示当前节点的兄弟
            int s = scene.hierarchy_[parent].firstChild_;

            // 如果父节点没有孩子，需要更新的信息有：
            // 父节点第一个孩子 是 自己
            // 自己的最后一个兄弟 是 自己
            if (s == -1)
            {
                scene.hierarchy_[parent].firstChild_ = node;
                scene.hierarchy_[node].lastSibling_ = node;
            }
            // 如果存在孩子节点，则需要更新：
            // 孩子节点所在层级加入的最后节点的下一个兄弟 是 新增节点
            // 孩子节点最后一个兄弟 是 新增节点
            else
            {
                int dest = scene.hierarchy_[s].lastSibling_;
                // 如果不缓存最后加入的兄弟节点索引，就需要用 nextSibling 迭代找出相关索引
                if (dest <= -1)
                {
                    for (dest = s; scene.hierarchy_[dest].nextSibling_ != -1; dest = scene.hierarchy_[dest].nextSibling_);
                }
                scene.hierarchy_[dest].nextSibling_ = node;
                scene.hierarchy_[s].lastSibling_ = node;
            }
        }

        scene.hierarchy_[node].level_ = level;
        scene.hierarchy_[node].nextSibling_ = -1;
        scene.hierarchy_[node].firstChild_ = -1;
        return node;
    }

    void MarkAsChanged(SceneGraph& scene, int node)
    {
        // 找出节点的相关层级，并在该层级的记录数据中标记此节点
        int level = scene.hierarchy_[node].level_;
        scene.changedAtThisFrame_[level].push_back(node);

        // 本节点变化，其所有的孩子节点也会发生变化
        for (int s = scene.hierarchy_[node].firstChild_; s != -1; s = scene.hierarchy_[s].nextSibling_)
            MarkAsChanged(scene, s);
    }

    // 通过名字查找节点
    int FindNodeByName(const SceneGraph& scene, const std::string& name)
    {
        tbb::parallel_for(0, static_cast<int>(scene.localTransform_.size()), [&scene, &name](int i)
        {
            if (scene.nameForNode_.contains(i))
            {
                int strID = scene.nameForNode_.at(i);
                if (strID > -1)
                    if (scene.names_[strID] == name)
                        return i;
            }
        });

        return -1;
    }

    // 重新计算全局变换
    void RecalculateGlobalTransforms(SceneGraph& scene)
    {
        // 第 0 层是整个场景的根节点
        // 如果这个节点改变，之后所有节点的变换都是在此基础发生的上
        if (!scene.changedAtThisFrame_[0].empty())
        {
            int c = scene.changedAtThisFrame_[0][0];
            scene.globalTransform_[c] = scene.localTransform_[c];
            scene.changedAtThisFrame_[0].clear();
        }

        // 节点在全局的变换是在它父亲全局的变换的基础上，在进行自身的本地变化
        for (int i = 1; i < MAX_NODE_LEVEL && (!scene.changedAtThisFrame_[i].empty()); i++)
        {
            tbb::parallel_for(0, static_cast<int>(scene.changedAtThisFrame_[i].size()), [&scene, i](int j)
            {
                int c = scene.changedAtThisFrame_[i][j];
                int p = scene.hierarchy_[c].parent_;
                scene.globalTransform_[c] = scene.globalTransform_[p] * scene.localTransform_[c];
            });
            scene.changedAtThisFrame_[i].clear();
        }
    }

    void LoadMap(FILE* f, std::unordered_map<uint32_t, uint32_t>& map)
    {
        std::vector<uint32_t> ms;

        uint32_t sz = 0;
        fread(&sz, 1, sizeof(sz), f);

        ms.resize(sz);
        fread(ms.data(), sizeof(int), sz, f);
        for (size_t i = 0; i < (sz / 2); i++)
            map[ms[i * 2 + 0]] = ms[i * 2 + 1];
    }
    
    // 从文件中加载场景图
    void LoadSceneGraph(const char* fileName, SceneGraph& scene)
    {
        Timer timer;
        
        FILE* f = fopen(fileName, "rb");

        if (!f)
        {
            CIAO_CORE_ERROR("Cannot open scene file '{}'.", fileName);
            return;
        }

        uint32_t sz = 0;
        fread(&sz, sizeof(sz), 1, f);

        scene.hierarchy_.resize(sz);
        scene.globalTransform_.resize(sz);
        scene.localTransform_.resize(sz);
        fread(scene.localTransform_.data(), sizeof(glm::mat4), sz, f);
        fread(scene.globalTransform_.data(), sizeof(glm::mat4), sz, f);
        fread(scene.hierarchy_.data(), sizeof(Hierarchy), sz, f);

        // Mesh for node [index to some list of buffers]
        LoadMap(f, scene.materialForNode_);
        LoadMap(f, scene.meshes_);

        if (!feof(f))
        {
            LoadMap(f, scene.nameForNode_);
            LoadStringList(f, scene.names_);

            LoadStringList(f, scene.materialNames_);
        }

        fclose(f);

        CIAO_CORE_TRACE("Loading scene graph successfully! Is costs {}", timer.elapsedString());
    }

    void SaveMap(FILE* f, const std::unordered_map<uint32_t, uint32_t>& map)
    {
        std::vector<uint32_t> ms;
        ms.reserve(map.size() * 2);
        for (const auto& m : map)
        {
            ms.push_back(m.first);
            ms.push_back(m.second);
        }
        const uint32_t sz = static_cast<uint32_t>(ms.size());
        fwrite(&sz, sizeof(sz), 1, f);
        fwrite(ms.data(), sizeof(int), ms.size(), f);
    }
    
    // 保存一个场景图
    void SaveSceneGraph(const char* fileName, const SceneGraph& scene)
    {
        Timer timer;
        
        FILE* f = fopen(fileName, "wb");

        const uint32_t sz = (uint32_t)scene.hierarchy_.size();
        fwrite(&sz, sizeof(sz), 1, f);

        fwrite(scene.localTransform_.data(), sizeof(glm::mat4), sz, f);
        fwrite(scene.globalTransform_.data(), sizeof(glm::mat4), sz, f);
        fwrite(scene.hierarchy_.data(), sizeof(Hierarchy), sz, f);

        // Mesh for node [index to some list of buffers]
        SaveMap(f, scene.materialForNode_);
        SaveMap(f, scene.meshes_);

        if (!scene.names_.empty() && !scene.nameForNode_.empty())
        {
            SaveMap(f, scene.nameForNode_);
            SaveStringList(f, scene.names_);

            SaveStringList(f, scene.materialNames_);
        }
        fclose(f);

        CIAO_CORE_TRACE("Saving scene graph successfully! Is costs {}", timer.elapsedString());
    }

    // 偏移场景图中某一范围内的所有节点
    void ShiftNodes(SceneGraph& scene, int startOffset, int nodeCount, int shiftAmount)
    {
        auto shiftNode = [shiftAmount](Hierarchy& node)
        {
            if (node.parent_ > -1)
                node.parent_ += shiftAmount;
            if (node.firstChild_ > -1)
                node.firstChild_ += shiftAmount;
            if (node.nextSibling_ > -1)
                node.nextSibling_ += shiftAmount;
            if (node.lastSibling_ > -1)
                node.lastSibling_ += shiftAmount;
            // node->level_ does not have to be shifted
            return node;
        };

        // 并行版本
        std::transform(std::execution::par,
            scene.hierarchy_.begin() + startOffset,
            scene.hierarchy_.begin() + startOffset + nodeCount,
            scene.hierarchy_.begin() + startOffset,
            shiftNode);

        // 串行版本
        // for (int i = 0; i < nodeCount; i++)
        //     shiftNode(scene.hierarchy_[i + startOffset]);
    }

    using ItemMap = std::unordered_map<uint32_t, uint32_t>;

    // 以一定的偏移量，从别的 Map 中加入数据
    void MergeMaps(ItemMap& m, const ItemMap& otherMap, int indexOffset, int itemOffset)
    {
        for (const auto& i : otherMap)
            m[i.first + indexOffset] = i.second + itemOffset;
    }

    void MergeScenes(SceneGraph& scene, const std::vector<SceneGraph*>& scenes, const std::vector<glm::mat4>& rootTransforms,
                     const std::vector<uint32_t>& meshCounts,
                     bool mergeMeshes, bool mergeMaterials)
    {
        // Create new root node
        scene.hierarchy_ = {
            {
                .parent_ = -1,
                .firstChild_ = 1,
                .nextSibling_ = -1,
                .lastSibling_ = -1,
                .level_ = 0
            }
        };

        // 设置根节点信息
        scene.nameForNode_[0] = 0;
        scene.names_ = { "Root" };

        scene.localTransform_.push_back(glm::mat4(1.f));
        scene.globalTransform_.push_back(glm::mat4(1.f));

        if (scenes.empty())
            return;

        // offs 代表节点的偏移
        int offs = 1;
        int meshOffs = 0;
        int nameOffs = (int)scene.names_.size();
        int materialOffs = 0;
        auto meshCount = meshCounts.begin();
        int idx = 0;

        if (!mergeMaterials)
            scene.materialNames_ = scenes[0]->materialNames_;

        
        for (const SceneGraph* s : scenes)
        {
            // 将各数据附加到新的的场景图中
            MergeVectors(scene.localTransform_, s->localTransform_);
            MergeVectors(scene.globalTransform_, s->globalTransform_);
            
            MergeVectors(scene.hierarchy_, s->hierarchy_);

            MergeVectors(scene.names_, s->names_);
            if (mergeMaterials)
                MergeVectors(scene.materialNames_, s->materialNames_);

            // 新场景图中节点的个数，以此作为偏移
            int nodeCount = (int)s->hierarchy_.size();
            ShiftNodes(scene, offs, nodeCount, offs);

            MergeMaps(scene.meshes_, s->meshes_, offs, mergeMeshes ? meshOffs : 0);
            MergeMaps(scene.materialForNode_, s->materialForNode_, offs, mergeMaterials ? materialOffs : 0);
            MergeMaps(scene.nameForNode_, s->nameForNode_, offs, nameOffs);

            // 重新计算节点的 nextSibling_ 缓存
            bool isLast = (idx == scenes.size() - 1);
            int next = isLast ? -1 : offs + nodeCount;
            scene.hierarchy_[offs].nextSibling_ = next;
            scene.hierarchy_[offs].parent_ = 0;         // 合成的场景图之间没有从属关系

            // 施以矩阵变换
            if (!rootTransforms.empty())
                scene.localTransform_[offs] = rootTransforms[idx] * scene.localTransform_[offs];

            idx++;
            offs += nodeCount;

            materialOffs += (int)s->materialNames_.size();
            nameOffs += (int)s->names_.size();

            if (mergeMeshes)
            {
                meshOffs += *meshCount;
                meshCount++;
            }
        }

        // 位移根节点以下的所有节点
        tbb::parallel_for(1, static_cast<int>(scene.hierarchy_.size()), [&scene](int i)
        {
            scene.hierarchy_[i].level_ += 1;
        });
    }

    // 场景图删除节点的几个辅助函数

    // 向一个排序的数组添加索引
    void AddUniqueIdx(std::vector<uint32_t>& v, uint32_t index)
    {
        if (!std::binary_search(v.begin(), v.end(), index))
            v.push_back(index);
    }

    // 递归记录所有要被删除的节点
    void CollectNodesToDelete(const SceneGraph& scene, int node, std::vector<uint32_t>& nodes)
    {
        for (int n = scene.hierarchy_[node].firstChild_; n != -1; n = scene.hierarchy_[n].nextSibling_)
        {
            AddUniqueIdx(nodes, n);
            CollectNodesToDelete(scene, n, nodes);
        }
    }

    int FindLastNonDeletedItem(const SceneGraph& scene, const std::vector<int>& newIndices, int node)
    {
        // we have to be more subtle:
        //   if the (newIndices[firstChild_] == -1), we should follow the link and extract the last non-removed item
        //   ..
        if (node == -1)
            return -1;

        return (newIndices[node] == -1) ? FindLastNonDeletedItem(scene, newIndices, scene.hierarchy_[node].nextSibling_) : newIndices[node];
    }

    void ShiftMapIndices(std::unordered_map<uint32_t, uint32_t>& items, const std::vector<int>& newIndices)
    {
        std::unordered_map<uint32_t, uint32_t> newItems;
        for (const auto& m : items)
        {
            int newIndex = newIndices[m.first];
            if (newIndex != -1)
                newItems[newIndex] = m.second;
        }
        items = newItems;
    }
    
    // 从场景图中删除选定的节点
    // 复杂度大约为 O( N * Log(N) * Log(M))(N = 场景图大小, M = 要删除的节点数)
    void DeleteSceneNodes(SceneGraph& scene, const std::vector<uint32_t>& nodesToDelete)
    {
        // 0) Add all the nodes down below in the hierarchy
        auto indicesToDelete = nodesToDelete;
        for (auto i : indicesToDelete)
            CollectNodesToDelete(scene, i, indicesToDelete);

        // aux array with node indices to keep track of the moved ones [moved = [](node) { return (node != nodes[node]); ]
        std::vector<int> nodes(scene.hierarchy_.size());
        std::iota(nodes.begin(), nodes.end(), 0);

        // 1.a) Move all the indicesToDelete to the end of 'nodes' array (and cut them off, a variation of swap'n'pop for multiple elements)
        auto oldSize = nodes.size();
        EraseSelected(nodes, indicesToDelete);

        // 1.b) Make a newIndices[oldIndex] mapping table
        std::vector<int> newIndices(oldSize, -1);
        for (int i = 0; i < nodes.size(); i++)
            newIndices[nodes[i]] = i;

        // 2) Replace all non-null parent/firstChild/nextSibling pointers in all the nodes by new positions
        auto nodeMover = [&scene, &newIndices](Hierarchy& h)
        {
            return Hierarchy{
                .parent_ = (h.parent_ != -1) ? newIndices[h.parent_] : -1,
                .firstChild_ = FindLastNonDeletedItem(scene, newIndices, h.firstChild_),
                .nextSibling_ = FindLastNonDeletedItem(scene, newIndices, h.nextSibling_),
                .lastSibling_ = FindLastNonDeletedItem(scene, newIndices, h.lastSibling_)
            };
        };
        std::transform(scene.hierarchy_.begin(), scene.hierarchy_.end(), scene.hierarchy_.begin(), nodeMover);

        // 3) Finally throw away the hierarchy items
        EraseSelected(scene.hierarchy_, indicesToDelete);

        // 4) As in mergeScenes() routine we also have to adjust all the "components" (i.e., meshes, materials, names and transformations)

        // 4a) Transformations are stored in arrays, so we just erase the items as we did with the scene.hierarchy_
        EraseSelected(scene.localTransform_, indicesToDelete);
        EraseSelected(scene.globalTransform_, indicesToDelete);

        // 4b) All the maps should change the key values with the newIndices[] array
        ShiftMapIndices(scene.meshes_, newIndices);
        ShiftMapIndices(scene.materialForNode_, newIndices);
        ShiftMapIndices(scene.nameForNode_, newIndices);

        // 5) scene node names list is not modified, but in principle it can be (remove all non-used items and adjust the nameForNode_ map)
        // 6) Material names list is not modified also, but if some materials fell out of use
    }
}
