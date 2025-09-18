#include "vulkan-mesh-model.h"

VulkanMeshModel::VulkanMeshModel(/*vector<VulkanMesh> newMeshList*/) { MeshList.clear(); }

size_t VulkanMeshModel::getMeshCount() { return MeshList.size(); }

VulkanMesh *VulkanMeshModel::getMesh(size_t index) {
    if (index >= MeshList.size()) {
        throw std::runtime_error("Attrmpted to access invalid Mesh index!");
    }

    return &MeshList[index];
}

void VulkanMeshModel::destroyModel() {
    if (getState())
        return;

    if (MODEL_INSTANCE) {
        this->~VulkanMeshModel();
        return;
    }

    for (auto &mesh : MeshList)
        mesh.destroyBuffers();

    DESTROYED = true;
}

vector<string> VulkanMeshModel::loadMaterials(const aiScene *scene) {
    // Create 1:1 sized list of textures
    vector<string> textures;
    textures.resize(scene->mNumMaterials);
    // vector<string> textureList(0);

    for (size_t i = 0; i < textures.size(); i++) {
        aiMaterial *material = scene->mMaterials[i];
        textures[i] = "";

        if (material->GetTextureCount(aiTextureType_DIFFUSE)) {
            aiString path;
            if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS) {
                size_t idx = string(path.data).rfind("\\");

                string fileName = string(path.data).substr(idx + 1);
                console.logLn(fileName);

                textures[i] = fileName;
            }
        }
    }
    return textures;
}

// Loads a tree of meshes (mesh parent with child meshes)
vector<VulkanMesh> VulkanMeshModel::loadNode(VkPhysicalDevice *newPhysicalDevice,
                                             VkDevice *newDevice,
                                             VkQueue *transferQueue,
                                             VkCommandPool *transferCommandPool,
                                             aiNode *node,
                                             const aiScene *scene,
                                             vector<uint16_t> matToTex) {
    // vector<VulkanMesh> meshList;
    //  Go through each mesh at this node and create it, then add it to our meshList
    for (size_t i = 0; i < node->mNumMeshes; i++) {
        // vkWaitSemaphores(*newDevice, renderFinished->at(*currentFrame), )

        MeshList.push_back(*LoadMesh(*newPhysicalDevice,
                                     *newDevice,
                                     *transferQueue,
                                     *transferCommandPool,
                                     scene->mMeshes[node->mMeshes[i]],
                                     scene,
                                     matToTex));
    }

    for (size_t i = 0; i < node->mNumChildren; i++) {
        vector<VulkanMesh> newList = loadNode(
            newPhysicalDevice, newDevice, transferQueue, transferCommandPool, node->mChildren[i], scene, matToTex);
        // meshList.insert(meshList.end(), newList.begin(), newList.end());
    }

    return MeshList;
}

VulkanMesh *VulkanMeshModel::LoadMesh(VkPhysicalDevice newPhysicalDevice,
                                      VkDevice newDevice,
                                      VkQueue transferQueue,
                                      VkCommandPool transferCommandPool,
                                      aiMesh *mesh,
                                      const aiScene *scene,
                                      vector<uint16_t> matToTex) {
    vector<Vertex> vertices;
    vector<uint32_t> indices;

    // Resize vertex list to hold all vertices for mesh
    vertices.resize(mesh->mNumVertices);

    for (size_t i = 0; i < mesh->mNumVertices; i++) {
        vertices[i].pos = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};

        if (mesh->mTextureCoords[0]) {
            vertices[i].tex = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};

        } else {
            vertices[i].tex = {0.0f, 0.0f};
        }

        if (mesh->mNormals) {
            vertices[i].norm = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
            // printf("x: %d, y: %d, z: %d\n", mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        }

        vertices[i].col = {1.0f, 1.0f, 1.0f};
    }

    for (size_t i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];

        // Go through face's indices and add to list
        for (size_t j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Create new mesh with details

    VulkanMesh *newMesh = new VulkanMesh(newPhysicalDevice,
                                         newDevice,
                                         transferQueue,
                                         transferCommandPool,
                                         &vertices,
                                         &indices,
                                         matToTex[mesh->mMaterialIndex]); //

    return newMesh;
}
