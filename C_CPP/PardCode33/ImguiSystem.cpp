#include "ImguiSystem.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "ImGuizmo.h"

//Systems
#include "EngineSystem.h"
#include "ECSSystem.h"
#include "InputSystem.h"
#include "CameraSystem.h"

ImguiSystem::ImguiSystem()
{   
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT __stdcall ImguiSystem::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
}

void ImguiSystem::Init(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup scaling
    /*
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)
    */
    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(pDevice, pDeviceContext);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //style.FontSizeBase = 20.0f;
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf");
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf");
    //IM_ASSERT(font != nullptr);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
}

void ImguiSystem::Frame(float deltatime)
{
    if (g_fTime_Log >= 1.0f)
        std::cout << "Frame : " << "ImguiSystem" << " Class" << '\n';
    // Our state, 수정필요
    ImGuiIO& io = ImGui::GetIO();
    _EngineSystem.bMouseOnGUI = io.WantCaptureMouse;    //마우스의 위치상황

    bool show_demo_window = true;
    bool show_another_window = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    // 1. 기즈모 시작 선언 (매 프레임)
    ImGuizmo::BeginFrame();

    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

       // ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
    }
    
    //// 3. Show another simple window.
    //if (show_another_window)
    //{
    //    ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
    //    ImGui::Text("Hello from another window!");
    //    if (ImGui::Button("Close Me"))
    //        show_another_window = false;
    //    ImGui::End();
    //}
   
    Editor_Transform();
}

void ImguiSystem::Render()
{
    if (g_fTime_Log >= 1.0f)
        std::cout << "Render : " << "ImguiSystem" << " Class" << '\n';
    // Rendering
    ImGui::Render();
    /*
    const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
    g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
    g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
    */
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ImguiSystem::Editor_Transform()
{
    size_t lookup_maincam = _CameraSystem.lookup_maincam;
    const auto& c_cam_main = _ECSSystem.GetComponent<C_Camera>(lookup_maincam);
    const auto& c_cam_proj = _ECSSystem.GetComponent<C_Projection>(lookup_maincam);
    const auto& c_cam_ortho = _ECSSystem.GetComponent<C_Orthographic>(lookup_maincam);
    const Matrix4x4& matView = c_cam_main.matView;
    const Matrix4x4& matProj = c_cam_proj.matProj;
    const Matrix4x4& matOrtho = c_cam_ortho.matOrtho;

    std::function<void()> ResetTransform = [&]() 
        {  
            m_fEditTF_Pos[0] = m_fEditTF_Pos[1] = m_fEditTF_Pos[2] = 0.0f;
            m_fEditTF_Rot[0] = m_fEditTF_Rot[1] = m_fEditTF_Rot[2] = 0.0f;
            m_fEditTF_Sca[0] = m_fEditTF_Sca[1] = m_fEditTF_Sca[2] = 1.0f;
        };
    size_t lookup = _EngineSystem.m_hash_pickingLookup;
    C_Transform* entityTransform = nullptr;
    if(lookup != _HashNotInitialize)
    {
        const Entity* entity = _ECSSystem.GetEntity(lookup);
        Archetype* archetype = _ECSSystem.QueryArchetype(entity->m_Key);

        size_t row = entity->m_IdxRow;
        size_t col = entity->m_IdxCol;

        auto& infos = archetype->GetComponents<C_Info>(row);
        auto& transforms = archetype->GetComponents<C_Transform>(row);
        entityTransform = &transforms[col];
        Vector3 vPosition = entityTransform->vPosition;
        Vector3 vRotate = entityTransform->qRotate.ToRotate();
        Vector3 vScale = entityTransform->vScale;
        m_fEditTF_Pos[0] = vPosition.GetX(); m_fEditTF_Pos[1] = vPosition.GetY(); m_fEditTF_Pos[2] = vPosition.GetZ();
        m_fEditTF_Rot[0] = vRotate.GetX(); m_fEditTF_Rot[1] = vRotate.GetY(); m_fEditTF_Rot[2] = vRotate.GetZ();
        m_fEditTF_Sca[0] = vScale.GetX(); m_fEditTF_Sca[1] = vScale.GetY(); m_fEditTF_Sca[2] = vScale.GetZ();
        m_szEntityName = _towm(infos[col].szName);
    }
    else
    {
        ResetTransform();
        m_szEntityName = NotPicking;
    }
    
 
    // -------------------------------------------------------------------------
    // [2] ImGui UI 구성
    // -------------------------------------------------------------------------
    ImGui::Begin("Inspector");

    static ImGuizmo::OPERATION mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    static ImGuizmo::MODE mCurrentGizmoMode = ImGuizmo::WORLD;

    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Gizmo Operation");
        if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE)) mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE)) mCurrentGizmoOperation = ImGuizmo::ROTATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE)) mCurrentGizmoOperation = ImGuizmo::SCALE;

        ImGui::Text("Gizmo Mode");
        if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL)) mCurrentGizmoMode = ImGuizmo::LOCAL;
        ImGui::SameLine();
        if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD)) mCurrentGizmoMode = ImGuizmo::WORLD;

        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.7f); // 라벨 영역 확보
        std::string outputName = "EntityName : " + m_szEntityName;
        ImGui::Text(outputName.c_str());
        // Position 조절 (DragFloat은 마우스 드래그로 미세조정 가능)
        if (ImGui::DragFloat3("Position", m_fEditTF_Pos, 0.1f) && entityTransform)
        {
            entityTransform->vPosition.Set(m_fEditTF_Pos[0], m_fEditTF_Pos[1], m_fEditTF_Pos[2]);
        }

        // Rotation 조절 (각도 단위)
        if (ImGui::DragFloat3("Rotation", m_fEditTF_Rot, 0.5f) && entityTransform)
        {
            entityTransform->qRotate = Quaternion(m_fEditTF_Rot[0], m_fEditTF_Rot[1], m_fEditTF_Rot[2]);
        }

        // Scale 조절
        if (ImGui::DragFloat3("Scale", m_fEditTF_Sca, 0.001f) && entityTransform)
        {
            entityTransform->vScale.Set(m_fEditTF_Sca[0], m_fEditTF_Sca[1], m_fEditTF_Sca[2]);
        }

        ImGui::PopItemWidth();

        // 초기화 버튼 (우측 정렬 예시)
        ImGui::Spacing();
        if (ImGui::Button("Reset Transform") && entityTransform)
        {
            ResetTransform();
            entityTransform->vPosition.Set(m_fEditTF_Pos[0], m_fEditTF_Pos[1], m_fEditTF_Pos[2]);
            entityTransform->qRotate = Quaternion(m_fEditTF_Rot[0], m_fEditTF_Rot[1], m_fEditTF_Rot[2]);
            entityTransform->vScale.Set(m_fEditTF_Sca[0], m_fEditTF_Sca[1], m_fEditTF_Sca[2]);
        }
    }
    ImGui::End();

    // Setting, Render gizmo
    if (lookup == _HashNotInitialize) return;
    // 2. 기즈모를 그릴 좌표 평면 설정 (화면 전체 혹은 특정 창)
    ImGuiIO& io = ImGui::GetIO();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

    // 3. 카메라 행렬 준비 (float* 로 변환), xmmatrix를 이용하므로 memcpy로 이동할 수 있다
    // 4. 대상 물체의 월드 행렬 가져오기
    Matrix4x4 matWorld = entityTransform ? GetMat_World(entityTransform->vScale, entityTransform->qRotate, entityTransform->vPosition) : GetMat_Identity();
    float model[16], view[16], proj[16];
    memcpy(view, &matView, sizeof(Matrix4x4));
    memcpy(proj, &matProj, sizeof(Matrix4x4));
    memcpy(model, &matWorld, sizeof(Matrix4x4));

    // 5. 조작 및 렌더링 (Manipulate)
    // Operation: TRANSLATE, ROTATE, SCALE 중 선택 (보통 단축키로 제어)
    // Mode: LOCAL 또는 WORLD
    ImGuizmo::Manipulate(view, proj, mCurrentGizmoOperation, mCurrentGizmoMode, model);

    // 6. 사용자가 기즈모를 조작했다면 엔진 데이터 갱신
    if (ImGuizmo::IsUsing() && entityTransform)
    {
        // 수정된 model 행렬을 다시 Matrix 객체로 복사
        Matrix4x4 newWorld;
        memcpy(&newWorld, model, sizeof(float) * 16);

        //수정된 행렬을 Decompose(분해)해서 Transform에 세팅
        Vector3 pos, scale;
        Quaternion rot;
        DecomposeFromWorld(newWorld, &scale, &rot, &pos);
        entityTransform->vScale = scale;
        entityTransform->qRotate = rot;
        entityTransform->vPosition = pos;
    }
}

ImguiSystem::~ImguiSystem()
{
    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}
