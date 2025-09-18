#pragma once

#include <codeframe>

#include <backends/imgui_impl_glfw.h>
#include <imgui.h>

#include "../core/common.h"

#ifdef VULKAN
#include <backends/imgui_impl_vulkan.h>
#include "../asset-loading/vulkan-assets/vulkan-assets.h"
#elif OPENGL
#include <backends/imgui_impl_opengl3.h>
#include "../asset-loading/opengl-assets/opengl-assets.h"
#endif

class GUI_Renderer {
  public:
    GUI_Renderer() = default;
#ifdef VULKAN
    GUI_Renderer(GLFWwindow &window,
                 VkInstance &instance,
                 VkDevice &device,
                 VkPhysicalDevice &physicalDevice,
                 VkQueue &queue,
                 uint32_t graphicsFamily,
                 VkRenderPass &renderPass,
                 VkCommandPool &graphicsCommandPool,
                 VkSampleCountFlagBits &msaaSampleCount,
                 bool &applyGraphics,
                 GraphicSettings &currentGraphicSettings,
                 Vulkan_Assets &assetManager);

#elif OPENGL
    GUI_Renderer(GLFWwindow &window,
                 bool &applyGraphics,
                 /*GraphicSettings& CurrentGraphicSettings,*/

                 OpenglAssets &assetManager);
#endif
    ~GUI_Renderer() = default;

    ImVec4 clearColor = {0.15f, 0.15f, 0.15f, 1.0f};

    void createImGuiInstance();
    void init_imGUI_Vulkan();

    void CleanUpGUI();
    void CleanUpGuiComponents();

  private:
#pragma region // Initialization Info

#ifdef VULKAN
    // Initialization Info
    struct IgInitInfo {
        // CREATION
        GLFWwindow *mainWindow{};
        VkInstance *instance{};
        VkDevice *Device{};
        VkPhysicalDevice *PhysicalDevice{};
        VkQueue *Queue{};
        uint32_t GraphicsFamily{};
        VkRenderPass *RenderPass{};
        VkCommandPool *GraphicsCommandPool{};

        // SETTINGS
        VkSampleCountFlagBits *MsaaSampleCount{};
        bool *ApplyGraphics{};
        GraphicSettings *CurrentGraphicSettings{};

        // OBJECTS
        Vulkan_Assets *AssetManager{};
    } IgInitInfo;

    VkDescriptorPool imguiPool{};

#elif OPENGL
    struct IgInitInfo {
        // CREATION
        GLFWwindow *mainWindow{};

        // SETTINGS
        bool *ApplyGraphics{};
        // GraphicSettings* CurrentGraphicSettings{};

        ////OBJECTS
        OpenglAssets *AssetManager{};
    } IgInitInfo;
#endif

#pragma endregion

#pragma region // Menu Info Structs

    struct SharedInfo {

        int *selectedObject = nullptr;
#ifdef VULKAN
        const Vulkan_Assets::AllAssets *allAssets = nullptr;
#elif OPENGL
        const OpenglAssets::AllAssets *allAssets = nullptr;
#endif

    } sharedInfo; // Shared by more than one menu

    struct MenuOutlinerInfo {
        bool ShowMenu = true;

    } outlinerInfo; // World Outliner

    struct MenuDetailsInfo {
        bool ShowMenu = true;

        bool *LocalTransform;
        glm::vec3 *position;
        glm::vec3 *rotation;
        glm::vec3 *scale;

    } detailsInfo; // Details Menu

    struct MenuContentBrowserInfo {
        bool ShowMenu = true;
        // std::thread* AssetLoadThread;
        // std::reference_wrapper<std::thread> AssetLoadThread;
    } browserInfo; // Content Browser

    struct MenuStatsOverlayInfo {
        bool ShowMenu = true;
        // std::thread* AssetLoadThread;
        // std::reference_wrapper<std::thread> AssetLoadThread;
    } statsInfo; // Stats Overlay

#pragma endregion

#pragma region // Render Menu Functions

    void ShowFileMenu();
    void MainMenuBar();

    void renderOutlinerMenu();
    void showOutlinerTree(const char *prefix, int uid);

    void renderDetailsMenu();

    void renderContentMenu();

    void renderStatsOverlay();

#pragma endregion

  public:
    template <class T>
    void RenderMenus(bool &LocalTransform,
                     glm::vec3 &translation,
                     glm::vec3 &rotation,
                     glm::vec3 &scale,
                     int *selectedObject,
                     const T assetList) {

#pragma region // Hotkeys

        // Hide and unhide Content Browser
        // [ Left_Ctrl + Space ]
        if (ImGui::IsKeyPressed(ImGuiKey_Space) && ImGui::GetIO().KeyCtrl) {
            browserInfo.ShowMenu = !browserInfo.ShowMenu;
        }

#pragma endregion

        // Set Info
        // sharedInfo
        sharedInfo.selectedObject = selectedObject;
        // sharedInfo.modelNames = AssetList->ModelNames;
        sharedInfo.allAssets = assetList;

        // menu 'OBJECT DETAILS'
        detailsInfo.LocalTransform = &LocalTransform;
        detailsInfo.position = &translation;
        detailsInfo.rotation = &rotation;
        detailsInfo.scale = &scale;

        // menu 'WORLD OUTLINER'
        // NON

        // menu 'Content Browser'

#pragma region // ImGUI Render Function

// Start the Dear ImGui frame
#ifdef VULKAN
        ImGui_ImplVulkan_NewFrame();
#elif OPENGL
        ImGui_ImplOpenGL3_NewFrame();
#endif
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // imgui commands
        // ImGui::ShowDemoWindow();

        MainMenuBar();

        if (outlinerInfo.ShowMenu)
            renderOutlinerMenu();

        if (detailsInfo.ShowMenu)
            renderDetailsMenu();

        if (browserInfo.ShowMenu)
            renderContentMenu();

        if (statsInfo.ShowMenu)
            renderStatsOverlay();

        // Rendering
        ImGui::Render();
        ImDrawData *draw_data = ImGui::GetDrawData();
        const bool is_minimized = (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);
        if (!is_minimized) {
        }

#pragma endregion
    }
};
