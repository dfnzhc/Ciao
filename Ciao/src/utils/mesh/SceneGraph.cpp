#include "pch.h"
#include "SceneGraph.h"

#include "utils/ReadFile.h"

namespace Ciao
{
    inline std::string GetNodeName(const SceneGraph& scene, int node)
    {
        int nameID = scene.nameForNode.find(node) != scene.nameForNode.end() ? scene.nameForNode.at(node) : -1;
        return (nameID != -1) ? scene.names[nameID] : std::string();
    }

    inline void SetNodeName(SceneGraph& scene, int node, const std::string& name)
    {
        uint32_t nameID = (uint32_t)scene.names.size();
        scene.names.push_back(name);
        scene.nameForNode[node] = nameID;
    }

    
    int AddNode(SceneGraph& scene, int parent, int level)
    {
        // 新添加节点的索引值
        int node = (int)scene.hierarchy.size();
        {
            scene.localTransform.push_back(mat4{ 1.0f });
            scene.globalTransform.push_back(mat4{ 1.0f });
        }

        scene.hierarchy.push_back({ .parent = parent, .lastSibling = -1 });

        // 把节点加入到层次结构之后，需要更新节点间的关系
        if (parent != -1)
        {
            // s 表示当前节点的兄弟
            int s = scene.hierarchy[parent].firstChild;

            // 如果父节点没有孩子，需要更新的信息有：
            // 父节点第一个孩子 是 自己
            // 自己的最后一个兄弟 是 自己
            if (s == -1)
            {
                scene.hierarchy[parent].firstChild = node;
                scene.hierarchy[node].lastSibling = node;
            }
            // 如果存在孩子节点，则需要更新：
            // 孩子节点所在层级加入的最后节点的下一个兄弟 是 新增节点
            // 孩子节点最后一个兄弟 是 新增节点
            else
            {
                int dest = scene.hierarchy[s].lastSibling;
                // 如果不缓存最后加入的兄弟节点索引，就需要用 nextSibling 迭代找出相关索引
                if (dest <= -1)
                {
                    for (dest = s; scene.hierarchy[dest].nextSibling != -1; dest = scene.hierarchy[dest].nextSibling);
                }
                scene.hierarchy[dest].nextSibling = node;
                scene.hierarchy[s].lastSibling = node;
            }
        }

        scene.hierarchy[node].level = level;
        scene.hierarchy[node].nextSibling = -1;
        scene.hierarchy[node].firstChild = -1;
        return node;
    }

    void MarkAsChanged(SceneGraph& scene, int node)
    {
        // 找出节点的相关层级，并在该层级的记录数据中标记此节点
        int level = scene.hierarchy[node].level;
        scene.changedAtThisFrame[level].push_back(node);

        // 本节点变化，其所有的孩子节点也会发生变化
        for (int s = scene.hierarchy[node].firstChild; s != -1; s = scene.hierarchy[s].nextSibling)
            MarkAsChanged(scene, s);
    }

    // 通过名字查找节点
    int FindNodeByName(const SceneGraph& scene, const std::string& name)
    {
        tbb::parallel_for(0, static_cast<int>(scene.localTransform.size()), [&scene, &name](int i)
        {
            if (scene.nameForNode.contains(i))
            {
                int strID = scene.nameForNode.at(i);
                if (strID > -1)
                    if (scene.names[strID] == name)
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
        if (!scene.changedAtThisFrame[0].empty())
        {
            int c = scene.changedAtThisFrame[0][0];
            scene.globalTransform[c] = scene.localTransform[c];
            scene.changedAtThisFrame[0].clear();
        }

        // 节点在全局的变换是在它父亲全局的变换的基础上，在进行自身的本地变化
        for (int i = 1; i < MAX_NODE_LEVEL && (!scene.changedAtThisFrame[i].empty()); i++)
        {
            tbb::parallel_for(0, static_cast<int>(scene.changedAtThisFrame[i].size()), [&scene, i](int j)
            {
                int c = scene.changedAtThisFrame[i][j];
                int p = scene.hierarchy[c].parent;
                scene.globalTransform[c] = scene.globalTransform[p] * scene.localTransform[c];
            });
            scene.changedAtThisFrame[i].clear();
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

        scene.hierarchy.resize(sz);
        scene.globalTransform.resize(sz);
        scene.localTransform.resize(sz);
        fread(scene.localTransform.data(), sizeof(glm::mat4), sz, f);
        fread(scene.globalTransform.data(), sizeof(glm::mat4), sz, f);
        fread(scene.hierarchy.data(), sizeof(Hierarchy), sz, f);

        // Mesh for node [index to some list of buffers]
        LoadMap(f, scene.materialForNode);
        LoadMap(f, scene.meshes);

        if (!feof(f))
        {
            LoadMap(f, scene.nameForNode);
            LoadStringList(f, scene.names);

            LoadStringList(f, scene.materialNames);
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

        const uint32_t sz = (uint32_t)scene.hierarchy.size();
        fwrite(&sz, sizeof(sz), 1, f);

        fwrite(scene.localTransform.data(), sizeof(glm::mat4), sz, f);
        fwrite(scene.globalTransform.data(), sizeof(glm::mat4), sz, f);
        fwrite(scene.hierarchy.data(), sizeof(Hierarchy), sz, f);

        // Mesh for node [index to some list of buffers]
        SaveMap(f, scene.materialForNode);
        SaveMap(f, scene.meshes);

        if (!scene.names.empty() && !scene.nameForNode.empty())
        {
            SaveMap(f, scene.nameForNode);
            SaveStringList(f, scene.names);

            SaveStringList(f, scene.materialNames);
        }
        fclose(f);

        CIAO_CORE_TRACE("Saving scene graph successfully! Is costs {}", timer.elapsedString());
    }

    // 偏移场景图中某一范围内的所有节点
    void ShiftNodes(SceneGraph& scene, int startOffset, int nodeCount, int shiftAmount)
    {
        auto shiftNode = [shiftAmount](Hierarchy& node)
        {
            if (node.parent > -1)
                node.parent += shiftAmount;
            if (node.firstChild > -1)
                node.firstChild += shiftAmount;
            if (node.nextSibling > -1)
                node.nextSibling += shiftAmount;
            if (node.lastSibling > -1)
                node.lastSibling += shiftAmount;
            // node->level_ does not have to be shifted
            return node;
        };

        // 并行版本
        std::transform(std::execution::par,
            scene.hierarchy.begin() + startOffset,
            scene.hierarchy.begin() + startOffset + nodeCount,
            scene.hierarchy.begin() + startOffset,
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
        scene.hierarchy = {
            {
                .parent = -1,
                .firstChild = 1,
                .nextSibling = -1,
                .lastSibling = -1,
                .level = 0
            }
        };

        // 设置根节点信息
        scene.nameForNode[0] = 0;
        scene.names = { "Root" };

        scene.localTransform.push_back(glm::mat4(1.f));
        scene.globalTransform.push_back(glm::mat4(1.f));

        if (scenes.empty())
            return;

        // offs 代表节点的偏移
        int offs = 1;
        int meshOffs = 0;
        int nameOffs = (int)scene.names.size();
        int materialOffs = 0;
        auto meshCount = meshCounts.begin();
        int idx = 0;

        if (!mergeMaterials)
            scene.materialNames = scenes[0]->materialNames;

        
        for (const SceneGraph* s : scenes)
        {
            // 将各数据附加到新的的场景图中
            MergeVectors(scene.localTransform, s->localTransform);
            MergeVectors(scene.globalTransform, s->globalTransform);
            
            MergeVectors(scene.hierarchy, s->hierarchy);

            MergeVectors(scene.names, s->names);
            if (mergeMaterials)
                MergeVectors(scene.materialNames, s->materialNames);

            // 新场景图中节点的个数，以此作为偏移
            int nodeCount = (int)s->hierarchy.size();
            ShiftNodes(scene, offs, nodeCount, offs);

            MergeMaps(scene.meshes, s->meshes, offs, mergeMeshes ? meshOffs : 0);
            MergeMaps(scene.materialForNode, s->materialForNode, offs, mergeMaterials ? materialOffs : 0);
            MergeMaps(scene.nameForNode, s->nameForNode, offs, nameOffs);

            // 重新计算节点的 nextSibling_ 缓存
            bool isLast = (idx == scenes.size() - 1);
            int next = isLast ? -1 : offs + nodeCount;
            scene.hierarchy[offs].nextSibling = next;
            scene.hierarchy[offs].parent = 0;         // 合成的场景图之间没有从属关系

            // 施以矩阵变换
            if (!rootTransforms.empty())
                scene.localTransform[offs] = rootTransforms[idx] * scene.localTransform[offs];

            idx++;
            offs += nodeCount;

            materialOffs += (int)s->materialNames.size();
            nameOffs += (int)s->names.size();

            if (mergeMeshes)
            {
                meshOffs += *meshCount;
                meshCount++;
            }
        }

        // 位移根节点以下的所有节点
        tbb::parallel_for(1, static_cast<int>(scene.hierarchy.size()), [&scene](int i)
        {
            scene.hierarchy[i].level += 1;
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
        for (int n = scene.hierarchy[node].firstChild; n != -1; n = scene.hierarchy[n].nextSibling)
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

        return (newIndices[node] == -1) ? FindLastNonDeletedItem(scene, newIndices, scene.hierarchy[node].nextSibling) : newIndices[node];
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
        std::vector<int> nodes(scene.hierarchy.size());
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
                .parent = (h.parent != -1) ? newIndices[h.parent] : -1,
                .firstChild = FindLastNonDeletedItem(scene, newIndices, h.firstChild),
                .nextSibling = FindLastNonDeletedItem(scene, newIndices, h.nextSibling),
                .lastSibling = FindLastNonDeletedItem(scene, newIndices, h.lastSibling)
            };
        };
        std::transform(scene.hierarchy.begin(), scene.hierarchy.end(), scene.hierarchy.begin(), nodeMover);

        // 3) Finally throw away the hierarchy items
        EraseSelected(scene.hierarchy, indicesToDelete);

        // 4) As in mergeScenes() routine we also have to adjust all the "components" (i.e., meshes, materials, names and transformations)

        // 4a) Transformations are stored in arrays, so we just erase the items as we did with the scene.hierarchy_
        EraseSelected(scene.localTransform, indicesToDelete);
        EraseSelected(scene.globalTransform, indicesToDelete);

        // 4b) All the maps should change the key values with the newIndices[] array
        ShiftMapIndices(scene.meshes, newIndices);
        ShiftMapIndices(scene.materialForNode, newIndices);
        ShiftMapIndices(scene.nameForNode, newIndices);

        // 5) scene node names list is not modified, but in principle it can be (remove all non-used items and adjust the nameForNode_ map)
        // 6) Material names list is not modified also, but if some materials fell out of use
    }
}
