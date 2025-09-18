#pragma once
#include "../entity.h"
#include "vulkan-mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

class VulkanMeshModel : public Entity {
  public:
    VulkanMeshModel();
    ~VulkanMeshModel() = default;

    size_t getMeshCount();

    VulkanMesh *getMesh(size_t index);

    vector<string> loadMaterials(const aiScene *scene);

    vector<VulkanMesh> loadNode(VkPhysicalDevice *newPhysicalDevice,
                                VkDevice *newDevice,
                                VkQueue *transferQueue,
                                VkCommandPool *transferCommandPool,
                                aiNode *node,
                                const aiScene *scene,
                                vector<uint16_t> matToTex);

    VulkanMesh *LoadMesh(VkPhysicalDevice newPhysicalDevice,
                         VkDevice newDevice,
                         VkQueue transferQueue,
                         VkCommandPool transferCommandPool,
                         aiMesh *mesh,
                         const aiScene *scene,
                         vector<uint16_t> matToTex);

    vector<string> *getChildren() { return &Children; }
    bool getState() { return DESTROYED; }
    void makeInstance() { MODEL_INSTANCE = true; }

    void destroyModel();

  private:
    vector<string> Children;

    bool MODEL_INSTANCE = false;
    bool DESTROYED = false;

    vector<VulkanMesh> MeshList;

    vector<int> TextureMap;
};
