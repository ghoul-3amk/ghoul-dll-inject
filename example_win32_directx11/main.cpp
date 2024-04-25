#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"
#include "imgui_freetype.h"
#include <d3d11.h>
#include <tchar.h>
#include <Windows.h>
#include <iostream>
#include <D3DX11tex.h>
#pragma comment(lib, "D3DX11.lib")
#include <dwmapi.h>
#include <string>
#include "imgui_settings.h"
#include "font.h"
#include "image.h"
#include "config.h"
#include "chrono"

#include "globals.h"
#include "MemorySR.h"
#include <ShObjIdl_core.h>

#pragma comment(lib,"Wininet.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "ntdll.lib")


static ID3D11Device*            g_pd3dDevice = NULL;
static ID3D11DeviceContext*     g_pd3dDeviceContext = NULL;
static IDXGISwapChain*          g_pSwapChain = NULL;
static ID3D11RenderTargetView*  g_mainRenderTargetView = NULL;
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


HWND hwnd;
RECT rc;
bool authenticed = false;

static int64_t eptime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

namespace font
{
    ImFont* poppins_medium = nullptr;
    ImFont* poppins_medium_low = nullptr;
    ImFont* tab_icon = nullptr;
    ImFont* chicons = nullptr;
    ImFont* tahoma_bold = nullptr;
    ImFont* tahoma_bold2 = nullptr;
}
namespace image
{
    ID3D11ShaderResourceView* bg = nullptr;
    ID3D11ShaderResourceView* logo = nullptr;
    ID3D11ShaderResourceView* logo_general = nullptr;

    ID3D11ShaderResourceView* arrow = nullptr;
    ID3D11ShaderResourceView* bell_notify = nullptr;
    ID3D11ShaderResourceView* roll = nullptr;


}
D3DX11_IMAGE_LOAD_INFO info; ID3DX11ThreadPump* pump{ nullptr };
DWORD picker_flags = ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreview;
bool aimbot, aimneck, scope2x , scope4x , track , medkit, cross, aimbothalf, chams, espchams, chamswallhack, report, mode, revive, aimbotlobby,full,esparrow,rightleft,updown, aimbotdrag, aimbotjump, antenahand, antenahead, laser, box, npc, sniperfov, sniperscope,aimbotscope, sniperswitch,whitebody, sniperrelaod, magic1, magic2,  fastfire, norecoil, antiblack, fuckgarena, camera, aimfov, ghost, speed, wallhack, fast, reducerisk, teleport, optimize,  internet, teleport1, guest, vision, function, pause, destruct, bypass;
namespace var {
    bool aimbot, aimneck, scope2x, scope4x, track, medkit, cross, chams, espchams, chamswallhack, aimbothalf, mode, revive, report, aimbotlobby, full, esparrow, rightleft, updown, aimbotdrag, aimbotjump, antenahand, antenahead, laser,  box, npc, sniperfov, sniperscope,  aimbotscope, sniperswitch, whitebody, sniperrelaod, magic1, magic2, fastfire, fuckgarena, antiblack, norecoil, camera, aimfov, ghost, speed, wallhack, fast, reducerisk, optimize, teleport,  internet,guest , pause, vision, function, teleport1, destruct, bypass;

}

static bool windows_log = true;
static bool antidump = false;
float tab_size = 0.f;
float arrow_roll = 0.f;
bool tab_opening = true;
int rotation_start_index;
static float boxtk = 1.f;
static float hptk = 1.f;
static float hdtk = 1.f;
static float bonetk = 1.f;

static bool testlog = false;
static bool Dcheckbox = false;
static bool Headcheckbox = false;
static bool Healthcheckbox = false;
static bool Namecheckbox = false;
static bool Distancecheckbox = false;
static bool weaponcheckbox = false;
static bool Filterteams = false;
static bool Filterteams_map = false;
static bool Bonecheckbox = false;

static bool Flogs[2] = { false, false };
const char* Flogss[2] = { "Male", "Female" };

void ImRotateStart()
{
    rotation_start_index = ImGui::GetWindowDrawList()->VtxBuffer.Size;
}
ImVec2 ImRotationCenter()
{
    ImVec2 l(FLT_MAX, FLT_MAX), u(-FLT_MAX, -FLT_MAX); // bounds

    const auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
    for (int i = rotation_start_index; i < buf.Size; i++)
        l = ImMin(l, buf[i].pos), u = ImMax(u, buf[i].pos);

    return ImVec2((l.x + u.x) / 2, (l.y + u.y) / 2); // or use _ClipRectStack?
}
void ImRotateEnd(float rad, ImVec2 center = ImRotationCenter())
{
    float s = sin(rad), c = cos(rad);
    center = ImRotate(center, s, c) - center;

    auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
    for (int i = rotation_start_index; i < buf.Size; i++)
        buf[i].pos = ImRotate(buf[i].pos, s, c) - center;
}
void Particles()
{
    ImVec2 screen_size = { (float)GetSystemMetrics(SM_CXSCREEN), (float)GetSystemMetrics(SM_CYSCREEN) };

    static ImVec2 partile_pos[100];
    static ImVec2 partile_target_pos[100];
    static float partile_speed[130];
    static float partile_radius[100];


    for (int i = 1; i < 50; i++)
    {
        if (partile_pos[i].x == 0 || partile_pos[i].y == 0)
        {
            partile_pos[i].x = rand() % (int)screen_size.x + 1;
            partile_pos[i].y = 15.f;
            partile_speed[i] = 1 + rand() % 35;
            partile_radius[i] = rand() % 4;

            partile_target_pos[i].x = rand() % (int)screen_size.x;
            partile_target_pos[i].y = screen_size.y * 2;
        }

        partile_pos[i] = ImLerp(partile_pos[i], partile_target_pos[i], ImGui::GetIO().DeltaTime * (partile_speed[i] / 60));

        if (partile_pos[i].y > screen_size.y)
        {
            partile_pos[i].x = 0;
            partile_pos[i].y = 0;
        }

        ImGui::GetWindowDrawList()->AddCircleFilled(partile_pos[i], partile_radius[i], ImColor(92, 217, 228, 255 / 2));
    }

}
void move_window() {

    ImGui::SetCursorPos(ImVec2(0, 0));
    if (ImGui::InvisibleButton("Move_detector", ImVec2(c::bg::size) + ImVec2(tab_size, 0)));
    if (ImGui::IsItemActive()) {

        GetWindowRect(hwnd, &rc);
        MoveWindow(hwnd, rc.left + ImGui::GetMouseDragDelta().x, rc.top + ImGui::GetMouseDragDelta().y, 1500, 800, TRUE);
    }
}

namespace ImGui
{
    int rotation_start_index;
    void ImRotateStart()
    {
        rotation_start_index = ImGui::GetWindowDrawList()->VtxBuffer.Size;
    }

    ImVec2 ImRotationCenter()
    {
        ImVec2 l(FLT_MAX, FLT_MAX), u(-FLT_MAX, -FLT_MAX); // bounds

        const auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
        for (int i = rotation_start_index; i < buf.Size; i++)
            l = ImMin(l, buf[i].pos), u = ImMax(u, buf[i].pos);

        return ImVec2((l.x + u.x) / 2, (l.y + u.y) / 2); // or use _ClipRectStack?
    }


    void ImRotateEnd(float rad, ImVec2 center = ImRotationCenter())
    {
        float s = sin(rad), c = cos(rad);
        center = ImRotate(center, s, c) - center;

        auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
        for (int i = rotation_start_index; i < buf.Size; i++)
            buf[i].pos = ImRotate(buf[i].pos, s, c) - center;
    }
}

void Trinage_background()
{

    ImVec2 screen_size = { (float)GetSystemMetrics(SM_CXSCREEN), (float)GetSystemMetrics(SM_CYSCREEN) };

    static ImVec2 partile_pos[100];
    static ImVec2 partile_target_pos[100];
    static float partile_speed[100];
    static float partile_size[100];
    static float partile_radius[100];
    static float partile_rotate[100];

    for (int i = 1; i < 50; i++)
    {
        if (partile_pos[i].x == 0 || partile_pos[i].y == 0)
        {
            partile_pos[i].x = rand() % (int)screen_size.x + 1;
            partile_pos[i].y = 15.f;
            partile_speed[i] = 1 + rand() % 25;
            partile_radius[i] = rand() % 4;
            partile_size[i] = rand() % 8;

            partile_target_pos[i].x = rand() % (int)screen_size.x;
            partile_target_pos[i].y = screen_size.y * 2;
        }

        partile_pos[i] = ImLerp(partile_pos[i], partile_target_pos[i], ImGui::GetIO().DeltaTime * (partile_speed[i] / 60));
        partile_rotate[i] += ImGui::GetIO().DeltaTime;

        if (partile_pos[i].y > screen_size.y)
        {
            partile_pos[i].x = 0;
            partile_pos[i].y = 0;
            partile_rotate[i] = 0;
        }

        ImGui::ImRotateStart();
        ImGui::GetWindowDrawList()->AddCircleFilled(partile_pos[i], partile_size[i], ImColor(c::accent_color), 1);
        ImGui::GetWindowDrawList()->AddShadowCircle(partile_pos[i], 1.f, ImColor(c::accent_color), 45.f + partile_size[i], ImVec2(0, 0), 0, 1);
        ImGui::ImRotateEnd(partile_rotate[i]);
    }
}
void notifications(const char* name, float speed_anim) {

    radius = ImLerp(radius, active_notify ? 250 : 0.f, ImGui::GetIO().DeltaTime * speed_anim);

    ImGui::SetCursorPos(ImVec2(s::bg::size.x - radius, 20.f));

    if (radius > 2.f) ImGui::Notify(name, cfg::label_notify, ImVec2(300, 60));


    if (radius > 240) {

        timer += 6.f * ImGui::GetIO().DeltaTime;

        if (timer > 5.f) {
            active_notify = false;
            timer = 0.f;
        }
    }

}


int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
  

    WNDCLASSEXW wc;
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_CLASSDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = NULL;
    wc.cbWndExtra = NULL;
    wc.hInstance = nullptr;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = L"ImGui";
    wc.lpszClassName = L"Streetxd";
    wc.hIconSm = LoadIcon(0, IDI_APPLICATION);

    RegisterClassExW(&wc);
    hwnd = CreateWindowExW(NULL, wc.lpszClassName, L"Streetxd 4.1", WS_POPUP, (GetSystemMetrics(SM_CXSCREEN) / 2) - (950 / 2), (GetSystemMetrics(SM_CYSCREEN) / 2) - (640 / 2), 950, 640, 0, 0, 0, 0);


    MARGINS margins = { -1 };
    DwmExtendFrameIntoClientArea(hwnd, &margins);

    POINT mouse;
    rc = { 0 };
    GetWindowRect(hwnd, &rc);

    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;    
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;     

    ImFontConfig cfg;
    cfg.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_ForceAutoHint | ImGuiFreeTypeBuilderFlags_LightHinting | ImGuiFreeTypeBuilderFlags_LoadColor;

    font::poppins_medium = io.Fonts->AddFontFromMemoryTTF(poppins_medium, sizeof(poppins_medium), 17.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    font::poppins_medium_low = io.Fonts->AddFontFromMemoryTTF(poppins_medium, sizeof(poppins_medium), 15.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    font::tab_icon = io.Fonts->AddFontFromMemoryTTF(tabs_icons, sizeof(tabs_icons), 24.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    font::tahoma_bold = io.Fonts->AddFontFromMemoryTTF(tahoma_bold, sizeof(tahoma_bold), 17.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    font::tahoma_bold2 = io.Fonts->AddFontFromMemoryTTF(tahoma_bold, sizeof(tahoma_bold), 28.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    font::chicons = io.Fonts->AddFontFromMemoryTTF(chicon, sizeof(chicon), 20.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());

        
    //if (image::bg == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, background_image, sizeof(background_image), &info, pump, &image::bg, 0);
    if (image::logo == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, logo, sizeof(logo), &info, pump, &image::logo, 0);
    if (image::logo_general == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, logo_general, sizeof(logo_general), &info, pump, &image::logo_general, 0);


    if (image::arrow == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, arrow, sizeof(arrow), &info, pump, &image::arrow, 0);
    if (image::bell_notify == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, bell_notify, sizeof(bell_notify), &info, pump, &image::bell_notify, 0);
    if (image::roll == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, roll, sizeof(roll), &info, pump, &image::roll, 0);


    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

    bool done = false;
    while (!done)
    {

        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;


        tab_size = ImLerp(tab_size, tab_opening ? 160.f : 0.f, ImGui::GetIO().DeltaTime * 12.f);
        arrow_roll = ImLerp(arrow_roll, tab_opening && (tab_size >= 159) ? 1.f : -1.f, ImGui::GetIO().DeltaTime * 12.f);

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();

        ImGui::NewFrame();
        {   

            ImGuiStyle* s = &ImGui::GetStyle();

            s->WindowPadding = ImVec2(0, 0), s->WindowBorderSize = 0;
            s->ItemSpacing = ImVec2(20, 20);

            s->ScrollbarSize = 8.f;


            //ImGui::GetBackgroundDrawList()->AddImage(image::bg, ImVec2(0, 0), ImVec2(1920, 1080), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 255));
            
            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(ImVec2(c::bg::size) + ImVec2(tab_size, 0));
            ImGui::Begin("Street FF", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus);
            {
                    const ImVec2& pos = ImGui::GetWindowPos();
                    const auto& p = ImGui::GetWindowPos();
                    const ImVec2 spacing = ImGui::GetStyle().ItemSpacing;

                    ImGui::GetBackgroundDrawList()->AddRectFilled(pos, pos + ImVec2(c::bg::size) + ImVec2(tab_size, 0), ImGui::GetColorU32(c::bg::background), c::bg::rounding);
                    ImGui::GetBackgroundDrawList()->AddRect(pos, pos + ImVec2(c::bg::size) + ImVec2(tab_size, 0), ImGui::GetColorU32(c::bg::outline_background), c::bg::rounding);

                    const int vtx_idx_1 = ImGui::GetWindowDrawList()->VtxBuffer.Size;
                    ImGui::GetWindowDrawList()->AddRect(ImVec2(1, 1), ImVec2(c::bg::size) + ImVec2(tab_size, 0), ImGui::GetColorU32(c::bg::background), c::bg::rounding, ImDrawFlags_None, 2.f);
                    ImGui::ShadeVertsLinearColorGradientKeepAlpha(ImGui::GetWindowDrawList(), vtx_idx_1, ImGui::GetWindowDrawList()->VtxBuffer.Size, ImVec2(1, 1), ImVec2(c::bg::size) + ImVec2(tab_size, 0), ImGui::GetColorU32(c::general_color), ImColor(14, 14, 15, 255));
                   
                   

                    ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(c::accent_text_color));

                //   ImGui::PushFont(font::tahoma_bold2); ImGui::RenderTextClipped(pos + ImVec2(55, 0) + spacing, pos + spacing + ImVec2(55, 60) + ImVec2(tab_size + (spacing.x / 2) - 30, 0), "UC", NULL, NULL, ImVec2(0.5f, 0.5f), NULL); ImGui::PopFont();

                    ImGui::RenderTextClipped(pos + ImVec2(60 + spacing.x, c::bg::size.y - 60 * 2), pos + spacing + ImVec2(60, c::bg::size.y) + ImVec2(tab_size, 0), "Status:", NULL, NULL, ImVec2(0.0f, 0.43f), NULL);
                    ImGui::RenderTextClipped(pos + ImVec2(60 + spacing.x, c::bg::size.y - 60 * 2), pos + spacing + ImVec2(60, c::bg::size.y) + ImVec2(tab_size, 0), globals.MemoryLogs.c_str(), NULL, NULL, ImVec2(0.0f, 0.57f), NULL);

                   ImGui::PushFont(font::tahoma_bold2); ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(250, 255, 255, 255)); ImGui::RenderTextClipped(pos + ImVec2(0, 0) + spacing, pos + spacing + ImVec2(60, 40) + ImVec2(tab_size + (spacing.x / 2) + 199, 0), "GHOUL CHEATS", NULL, NULL, ImVec2(1.f, 0.5f), NULL); ImGui::PopFont(); ImGui::PopStyleColor();

                    // ImGui::GetBackgroundDrawList()->AddImage(image::logo, pos+ImVec2(10,10), pos + ImVec2(10,10),ImVec2(100,100),ImVec2(100,100),ImColor(255,255,255,255));



                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(90, 93, 100, 255)); ImGui::RenderTextClipped(pos + ImVec2(30, 20) + spacing, pos + spacing + ImVec2(60, 80) + ImVec2(tab_size + (spacing.x / 2) + 108, -20), "Dev By ghoul_on", NULL, NULL, ImVec2(1.f, 0.5f), NULL); ImGui::PopStyleColor();
                    /*
                    ImGui::SetCursorPos(ImVec2(245 + tab_size, -20) + (s->ItemSpacing * 2));
                    ImGui::BeginChild("", " ", ImVec2((c::bg::size.x - 60 - s->ItemSpacing.x * 22.5) / 2, 60));
                    ImGui::PushFont(font::tab_icon);
                    //ImGui::Text("I"); ImGui::SameLine();
                    //ImGui::Text("H"); ImGui::SameLine();
                    //ImGui::Text("G"); ImGui::SameLine();
                  //  ImGui::GetWindowDrawList()->AddText(pos + ImVec2(460, 38), ImColor(90, 93, 100, 255), "I");
                //    ImGui::GetWindowDrawList()->AddText(pos + ImVec2(495, 38), ImColor(90, 93, 100, 255), "H");
                //    ImGui::GetWindowDrawList()->AddText(pos + ImVec2(530, 38), ImColor(90, 93, 100, 255), "G");
                    ImGui::PopFont();
                    ImGui::EndChild();*/


                    ImGui::SetCursorPos(ImVec2(385 + tab_size, -20) + (s->ItemSpacing * 2));
                    ImGui::BeginChild("", "  ", ImVec2((c::bg::size.x - 60 - s->ItemSpacing.x * 4) / 2, 60));
                    ImGui::PushFont(font::poppins_medium);
                    static int select1 = 0;
                    const char* items1[8]{ "Bluestacks 4", "Bluestacks 5", "MSI App Player" ,"LD Player","Smart Gaga","Game Loop","Nox Player","MEmu Play" };
                    ImGui::Combo("Emulator Select", &select1, items1, IM_ARRAYSIZE(items1), 8);
                    ImGui::PopFont();
                    ImGui::EndChild();
                    ImGui::PopStyleColor(1);



                      ImDrawList* draw_list = ImGui::GetWindowDrawList();
                      float border_thickness = 5.0f; // Adjust as needed
                      float rounding = 10.0f; // Adjust as needed
                      ImVec2 min = ImGui::GetWindowPos();
                      ImVec2 max = ImGui::GetWindowPos() + ImGui::GetWindowSize();
                      ImU32 border_color = ImGui::GetColorU32(c::accent_color);
                      draw_list->AddRect(min, max, border_color, rounding, ImDrawCornerFlags_All, border_thickness);
                      ImGui::GetBackgroundDrawList()->AddRectFilled(pos, pos + ImVec2(c::bg::size) + ImVec2(tab_size, 0), ImGui::GetColorU32(c::bg::background), c::bg::rounding);
                      ImGui::GetBackgroundDrawList()->AddRect(pos, pos + ImVec2(c::bg::size) + ImVec2(tab_size, 0), ImGui::GetColorU32(c::bg::outline_background), c::bg::rounding);

                    const char* nametab_array1[5] = { "E", "D", "A", "B","C" };
                    const char* nametab_array[5] = { "Aimbot", "Visuals", "Plugins", "Protect","Settings" };
                    const char* nametab_hint_array[5] = { "Target & assist", "Location & view", "Advanced & misc", "Bypass & undetected","Save your settings" };


                    static int tabs = 0;

                    ImGui::SetCursorPos(ImVec2(spacing.x + (60 - 22) / 2, 60 + (spacing.y * 2) + 22));
                    ImGui::BeginGroup();
                    {
                        for (int i = 0; i < sizeof(nametab_array1) / sizeof(nametab_array1[0]); i++)
                            if (ImGui::Tab(i == tabs, nametab_array1[i], nametab_array[i], nametab_hint_array[i], ImVec2(35 + tab_size, 35))) tabs = i;
                    }
                    ImGui::EndGroup();

                    ImGui::SetCursorPos(ImVec2(8, 9) + spacing);

                    ImRotateStart();
                    if (ImGui::CustomButton(1, image::roll, ImVec2(35, 35), ImVec2(0, 0), ImVec2(1, 1), ImGui::GetColorU32(c::logo_color)))
                        tab_opening = !tab_opening;
                    ImRotateEnd(1.57f * arrow_roll);


                    ImGui::GetBackgroundDrawList()->AddRectFilled(pos + ImVec2(0, -20 + spacing.y) + spacing, pos + spacing + ImVec2(60, c::bg::size.y - 60 - spacing.y * 3) + ImVec2(tab_size, 0), ImGui::GetColorU32(c::child::background), c::child::rounding);
                    ImGui::GetBackgroundDrawList()->AddRect(pos + ImVec2(0, -20 + spacing.y) + spacing, pos + spacing + ImVec2(60, c::bg::size.y - 60 - spacing.y * 3) + ImVec2(tab_size, 0), ImGui::GetColorU32(c::child::outline_background), c::child::rounding);

                    ImGui::GetBackgroundDrawList()->AddRectFilled(pos + ImVec2(0, c::bg::size.y - 60 - spacing.y * 2) + spacing, pos + spacing + ImVec2(60, c::bg::size.y - spacing.y * 2) + ImVec2(tab_size, 0), ImGui::GetColorU32(c::child::background), c::child::rounding);
                    ImGui::GetBackgroundDrawList()->AddRect(pos + ImVec2(0, c::bg::size.y - 60 - spacing.y * 2) + spacing, pos + spacing + ImVec2(60, c::bg::size.y - spacing.y * 2) + ImVec2(tab_size, 0), ImGui::GetColorU32(c::child::outline_background), c::child::rounding);

                    ImGui::GetWindowDrawList()->AddImage(image::logo, pos + ImVec2(0, c::bg::size.y - 60 - spacing.y * 2) + spacing + ImVec2(15, 15), pos + spacing + ImVec2(60, c::bg::size.y - spacing.y * 2) - ImVec2(15, 15), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 255));

                    //ImGui::GetWindowDrawList()->AddCircleFilled(pos + ImVec2(63, c::bg::size.y - (spacing.y * 2) + 3), 10.f, ImGui::GetColorU32(c::child::background), 100.f);
                    //ImGui::GetWindowDrawList()->AddCircleFilled(pos + ImVec2(63, c::bg::size.y - (spacing.y * 2) + 3), 6.f, ImColor(0, 255, 0, 255), 100.f);




                    static float tab_alpha = 0.f; /* */ static float tab_add; /* */ static int active_tab = 0;

                    tab_alpha = ImClamp(tab_alpha + (4.f * ImGui::GetIO().DeltaTime * (tabs == active_tab ? 1.f : -1.f)), 0.f, 1.f);
                    tab_add = ImClamp(tab_add + (std::round(350.f) * ImGui::GetIO().DeltaTime * (tabs == active_tab ? 1.f : -1.f)), 0.f, 1.f);

                    if (tab_alpha == 0.f && tab_add == 0.f) active_tab = tabs;

                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, tab_alpha * s->Alpha);



                    /////// FUNCTION
                    if (tabs == 0)
                    {


                        ImGui::SetCursorPos(ImVec2(60 + tab_size, 60) + (s->ItemSpacing * 2));
                        ImGui::BeginGroup();
                        {
                            ImGui::BeginChild("A", "AIMBOT", ImVec2((c::bg::size.x - 60 - s->ItemSpacing.x * 4) / 2, 190));
                            {

                              //  if (ImGui::Checkbox("Aimbot - Neck [Login]", &Dcheckbox));
  
                                 aimneck = ImGui::Checkbox("Aimbot - Neck [Login]", &var::aimneck);
                                 if (aimneck) {
                                     if (var::aimneck) {

                                         notify_active("Injecting - MagicBulletx1");
                                     }

                                 }
                                aimbotlobby = ImGui::Checkbox("Aimbot - Neck [Lobby]", &var::aimbotlobby);  
                                aimbotdrag = ImGui::Checkbox("Aimbot - Drag [Lobby]", &var::aimbotdrag);
                                aimbotscope = ImGui::Checkbox("Aimbot - Scope [Login]", &var::aimbotscope);

         
                            }
                            ImGui::EndChild();
                            ImGui::SetCursorPos(ImVec2(60 + tab_size, 270) + (s->ItemSpacing * 2));
                            ImGui::BeginChild("C", "AIM SCOPE", ImVec2((c::bg::size.x - 60 - s->ItemSpacing.x * 4) / 2, 190));
                            {
                                
                                scope2x = ImGui::Checkbox("Aim Scope 2X", &var::scope2x);
                                scope4x = ImGui::Checkbox("Aim Scope 4X", &var::scope4x);
                                aimbotjump = ImGui::Checkbox("Jump Aimbot", &var::aimbotjump);
                                track = ImGui::Checkbox("Aim Scope Tracking", &var::track);

;
                            }
/*
                            ImGui::EndChild();
                            ImGui::SetCursorPos(ImVec2(60 + tab_size, 480) + (s->ItemSpacing * 2));
                            ImGui::BeginChild("D", "DRAWING", ImVec2((c::bg::size.x - 60 - s->ItemSpacing.x * 4) / 2, 100));
                            {
                                ImGui::Checkbox("Enable Crosshair", &Filterteams);
                                static int select1 = 0;
                                const char* items1[4]{ "None", "Circle", "Cross" ,"X-Aim" };
                                ImGui::Combo("Crosshair Type", &select1, items1, IM_ARRAYSIZE(items1), 4);
                            }*/
                            ImGui::EndChild();
                        }
                        ImGui::EndGroup();
                        ImGui::SameLine();

                        ImGui::BeginGroup();
                        {
                            ImGui::BeginChild("C", "AIM SNIPER", ImVec2((c::bg::size.x - 60 - s->ItemSpacing.x * 4) / 2, 150));
                            {
                                sniperfov = ImGui::Checkbox("Aim All Sniper [New]", &var::sniperfov);
                                sniperswitch = ImGui::Checkbox("Quick Change Switch [New]", &var::sniperswitch);
                                sniperscope = ImGui::Checkbox("Sniper Scope [New]", &var::sniperscope);
                   
                               
                            }
                            ImGui::EndChild();

                            ImGui::BeginChild("A", "AIM RAGE", ImVec2((c::bg::size.x - 60 - s->ItemSpacing.x * 4) / 2, 190));
                            {
                               

                                    full = ImGui::Checkbox("Aim Fov [Full]", &var::full);
                                rightleft = ImGui::Checkbox("Aim Fov [Right and left]", &var::rightleft);
                                updown = ImGui::Checkbox("Aim Fov [Up and down]", &var::updown);


                            }
                            ImGui::EndChild();

/*
                            ImGui::BeginChild("C", "AIM LEGIT", ImVec2((c::bg::size.x - 60 - s->ItemSpacing.x * 4) / 2, 190));
                            {
                                ImGui::Checkbox("Aim Lock ++", &Filterteams);
                                ImGui::Checkbox("Aim Lock ++++", &Filterteams_map);
                                ImGui::Checkbox("Aim Assist Passive", &Filterteams);
                                ImGui::Checkbox("Aim Assist Aggressive", &Filterteams_map);
                            }
                            ImGui::EndChild();*/
                        }
                        ImGui::EndGroup();
                    }
                    //////
                    if (tabs == 1)
                    {


                        ImGui::SetCursorPos(ImVec2(60 + tab_size, 60) + (s->ItemSpacing * 2));
                        ImGui::BeginGroup();
                        {
                            ImGui::BeginChild("A", "ESP PLAYER", ImVec2((c::bg::size.x - 60 - s->ItemSpacing.x * 4) / 2, 190));
                            {

                                npc = ImGui::Checkbox("Esp Name", &var::npc);
                                box = ImGui::Checkbox("Esp Box", &var::box);
                                esparrow = ImGui::Checkbox("Esp Arrow", &var::esparrow);


                            }
                            ImGui::EndChild();
/*
                            ImGui::SetCursorPos(ImVec2(60 + tab_size, 270) + (s->ItemSpacing * 2));
                            ImGui::BeginChild("C", "ESP LOOT", ImVec2((c::bg::size.x - 60 - s->ItemSpacing.x * 4) / 2, 190));
                            {
                                ImGui::Checkbox("Esp Coin", &Dcheckbox);
                                ImGui::Checkbox("Esp Armor", &Dcheckbox);
                                ImGui::Checkbox("Esp Bullet", &Dcheckbox);
                                ImGui::Checkbox("Gloo Shield", &Dcheckbox);
                            }
                            ImGui::EndChild();*/

                            ImGui::SetCursorPos(ImVec2(60 + tab_size, 270) + (s->ItemSpacing * 2));
                            ImGui::BeginChild("D", "ANTENA", ImVec2((c::bg::size.x - 60 - s->ItemSpacing.x * 4) / 2, 200));
                            {

                                antenahand = ImGui::Checkbox("Antena - Hand", &var::antenahand);
                                antenahead = ImGui::Checkbox("Antena - Head", &var::antenahead);
                                ImGui::Checkbox("Antena - Neck", &Dcheckbox);
                                ImGui::MultiCombo("Personage", Flogs, Flogss, 2);
                            }
                            ImGui::EndChild();




                        }
                        ImGui::EndGroup();
                        ImGui::SameLine();

                        ImGui::BeginGroup();
                        {


                            ImGui::BeginChild("C", "VIEW", ImVec2((c::bg::size.x - 60 - s->ItemSpacing.x * 4) / 2, 100));
                            {
                                vision = ImGui::Checkbox("Increase Vision", &var::vision);
                          
                            }
                            ImGui::EndChild();

                            ImGui::BeginChild("C", "MIXED", ImVec2((c::bg::size.x - 60 - s->ItemSpacing.x * 4) / 2, 310));
                            {
                                medkit = ImGui::Checkbox("Night Mode", &var::medkit);
                                cross = ImGui::Checkbox("Cross Hair HD", &var::cross);
                                revive = ImGui::Checkbox("Reivie Fast", &var::revive);
                                camera = ImGui::Checkbox("Camera Hack", &var::camera);
                                whitebody = ImGui::Checkbox("White Body", &var::whitebody);



                  

                            }
                            ImGui::EndChild();
                        }
                        ImGui::EndGroup();
                    }
                    ///////
                    if (tabs == 2)
                    {


                        ImGui::SetCursorPos(ImVec2(60 + tab_size, 60) + (s->ItemSpacing * 2));
                        ImGui::BeginGroup();
                        {
                            ImGui::BeginChild("A", "ADVANCED", ImVec2((c::bg::size.x - 60 - s->ItemSpacing.x * 4) / 2, 160));
                            {
                                norecoil = ImGui::Checkbox("No Recoil", &var::norecoil);
                                sniperrelaod = ImGui::Checkbox("Fast Reload", &var::sniperrelaod);

                            }
                            ImGui::EndChild();
                            ImGui::BeginChild("D", "MAGIC BULLETS", ImVec2((c::bg::size.x - 60 - s->ItemSpacing.x * 4) / 2, 160));
                            {

                                magic1 = ImGui::Checkbox("MagicBullet v1", &var::magic1);
                                magic2 = ImGui::Checkbox("MagicBullet v2", &var::magic2);

                            }
                            ImGui::EndChild();

/*                            ImGui::BeginChild("C", "MISC - BETA", ImVec2((c::bg::size.x - 60 - s->ItemSpacing.x * 4) / 2, 160));
                            {
                                ImGui::Checkbox("Medkit 3S ", &Filterteams);
                                ImGui::Checkbox("Speed Fire", &Filterteams);
                                ImGui::Checkbox("Climb Wall", &Dcheckbox);
                            }
                            ImGui::EndChild();*/
                        }
                        ImGui::EndGroup();
                        ImGui::SameLine();

                        ImGui::BeginGroup();
                        {
                            ImGui::BeginChild("C", "HOTKEY - HAX", ImVec2((c::bg::size.x - 60 - s->ItemSpacing.x * 4) / 2, 360));
                            {
                                static int wallhack;
                                static int speedhack;
                                static int ghosthack;
                                static int teleporthack;
                                ImGui::Keybind("Wall Hack", &wallhack, true);
                                ImGui::Keybind("Speed Hack", &speedhack, true);
                                ImGui::Keybind("Ghost Hack", &ghosthack, true);
                                ImGui::Keybind("Teleport Hack", &teleporthack, true);
                            }
                            ImGui::EndChild();

                        }
                        ImGui::EndGroup();
                    }
                    ///////
                    if (tabs == 3)
                    {
                        ImGui::SetCursorPos(ImVec2(60 + tab_size, 60) + (s->ItemSpacing * 2));
                        ImGui::BeginGroup();
                        {
                            ImGui::BeginChild("D", "SECURITY", ImVec2((c::bg::size.x - 60 - s->ItemSpacing.x * 4) / 2, 190));
                            {

                                report = ImGui::Checkbox("Antiban Login", &var::report);
                                reducerisk = ImGui::Checkbox("Antiblack Login", &var::reducerisk);
                                function = ImGui::Checkbox("Function Bypass", &var::function);


                            }
                            ImGui::EndChild();
                            ImGui::BeginChild("C", "OPEN MOBILE", ImVec2((c::bg::size.x - 60 - s->ItemSpacing.x * 4) / 2, 100));
                            {



                                bypass = ImGui::Checkbox("Bypass Emulator [V2]", &var::bypass);
                            }
                            ImGui::EndChild();


                        }
                        ImGui::EndGroup();
                        ImGui::SameLine();

                        ImGui::BeginGroup();
                        {

                            ImGui::BeginChild("C", "ANTI DETECT", ImVec2((c::bg::size.x - 60 - s->ItemSpacing.x * 4) / 2, 190));
                            {




                                guest = ImGui::Checkbox("Guest Reset [New]", &var::guest);

                                ImVec4 newColor = c::accent_color; // Initialize newColor with the current accent color

                                ImGui::Spacing();

                                if (ImGui::ColorEdit4("Change Theme Color", &newColor.x, ImGuiColorEditFlags_NoInputs)) {

                                    c::accent_color = newColor;
                                }

                            }
                            ImGui::EndChild();


/*
* 
* 
* 
* 
                            ImGui::EndChild();
                            ImGui::BeginChild("C", "TIPS", ImVec2((c::bg::size.x - 60 - s->ItemSpacing.x * 4) / 2, 160));
                            {
                                ImGui::Checkbox("Bypass Data", &Filterteams);
                                ImGui::Checkbox("Bypass FireWall", &Filterteams);
                            }*/

                        
                        }
                        ImGui::EndGroup();
                    }
                    ///////
                    if (tabs == 4)
                    {
                        ImGui::SetCursorPos(ImVec2(60 + tab_size, 60) + (s->ItemSpacing * 2));
                        ImGui::BeginGroup();
                        {
                            ImGui::BeginChild("D", "GENERAL", ImVec2((c::bg::size.x - 60 - s->ItemSpacing.x * 4) / 2, 190));
                            {
                         

                                mode = ImGui::Checkbox("Stream Mode", &var::mode);
                                ImGui::Checkbox("Hide Icon", &Dcheckbox);
                                ImGui::Checkbox("Top Most", &Dcheckbox);


                            }
                            ImGui::EndChild();
                            ImGui::BeginChild("D", "HOT KEY", ImVec2((c::bg::size.x - 60 - s->ItemSpacing.x * 4) / 2, 100));
                            {
/*
                                ImGui::PushFont(font::poppins_medium);
                                ImGui::Text("Press Insert : [ Hide/Show ]");
                                ImGui::PopFont();*/
                                ImGui::Checkbox("Press Insert : [ Hide/Show ]", &Dcheckbox);
                                ImGui::Checkbox("Press Delete : [ Close App ]", &Dcheckbox);

                            }
                            ImGui::EndChild();

                        }
                        ImGui::EndGroup();
                        ImGui::SameLine();

                        ImGui::BeginGroup();
                        {
                            ImGui::BeginChild("C", "ADVANCED", ImVec2((c::bg::size.x - 60 - s->ItemSpacing.x * 4) / 2, 190));
                            {

                                internet = ImGui::Checkbox("Block Internet", &var::internet);
                                pause = ImGui::Checkbox("Pause/unpause Emulator", &var::pause);
                                destruct = ImGui::Checkbox("Safe Exit [ App & Emulator ]", &var::destruct);
;

                            }
                            ImGui::EndChild();
                        }
                        ImGui::EndGroup();
                    }
                    /////



                    ImGui::PopStyleVar();
                    Trinage_background();
                 //   Particles();
                    move_window();
                    //DiscordRPCStart();
                }
            ImGui::End();
            

        }
        ImGui::Render();
        const float clear_color_with_alpha[4] = { 0 };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        g_pSwapChain->Present(1, 0); 
    }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}


bool CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
        res = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_WARP, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

