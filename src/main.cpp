#include "includes.h"
#include "offsets.h"
#include "globals.h"
//#include "cheats.h"
#include <cstdio>
#include <thread>
#include <vector>
#include "Font.hpp"
#include "Logo.hpp"
#include <D3DX11tex.h>
#pragma comment (lib, "D3DX11.lib")

#include "BHOP/bhop.h"
#include "ESP/esp.h"


HMODULE hModule;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Present oPresent;
HWND window = NULL;
WNDPROC oWndProc;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* mainRenderTargetView;

static void unload_module()
{
	Sleep(2000);
	FreeLibrary(hModule);
}


void InitImGui()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(pDevice, pContext);
	ImFontConfig font;
	font.FontDataOwnedByAtlas = false;
	ImFont* pFont = io.Fonts->AddFontFromMemoryTTF((void*)rudaFont, sizeof(rudaFont), 16.0f, &font);
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

ID3D11ShaderResourceView* ImageResource = nullptr;

inline bool LoadImageByMemory(ID3D11Device* device, unsigned char* image, size_t image_size, ID3D11ShaderResourceView** result)
{
	D3DX11_IMAGE_LOAD_INFO information;
	ID3DX11ThreadPump* thread = nullptr;

	auto hres = D3DX11CreateShaderResourceViewFromMemory(device, image, image_size, &information, thread, result, 0);
	return (hres == S_OK);

}

bool init = false;
HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	if (!init)
	{
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)& pDevice)))
		{
			pDevice->GetImmediateContext(&pContext);
			DXGI_SWAP_CHAIN_DESC sd;
			pSwapChain->GetDesc(&sd);
			window = sd.OutputWindow;
			ID3D11Texture2D* pBackBuffer;
			pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)& pBackBuffer);
			pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
			pBackBuffer->Release();
			oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
			InitImGui();
			init = true;
		}

		else
			return oPresent(pSwapChain, SyncInterval, Flags);
	}


	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();

	static bool image_load = false;
	if (!image_load)
	{
		LoadImageByMemory(pDevice, KotatsuLogo, sizeof(KotatsuLogo), &ImageResource);
		image_load = true;
	}

	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_WindowBg] = ImVec4(47 / 255.0f, 47 / 255.0f, 61 / 255.0f, 255 / 255.0f);
	style.Colors[ImGuiCol_Button] = ImVec4(20 / 255.0f, 20 / 255.0f, 20 / 255.0f, 170 / 255.0f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(180 / 255.0f, 90 / 255.0f, 220 / 255.0f, 255 / 255.0f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(20 / 255.0f, 20 / 255.0f, 20 / 255.0f, 170 / 255.0f);

	style.FrameBorderSize = 1.0f;
	style.FrameRounding = 4.0f;

	auto buttonOnColor = ImVec4(180 / 255.0f, 90 / 255.0f, 220 / 255.0f, 255 / 255.0f);
	auto buttonOffColor = ImVec4(20 / 255.0f, 20 / 255.0f, 20 / 255.0f, 170 / 255.0f);
	auto tabButtonColor = ImVec4(130 / 255.0f, 90 / 255.0f, 220 / 255.0f, 255 / 255.0f);


	ImGuiIO& io = ImGui::GetIO();
	if (io.KeysDown[73])
	{
		if (!Globals::menuKeyPressed)
		{
			Globals::menuOpened = !Globals::menuOpened;
			Globals::menuKeyPressed = true;
		}
	}
	else
	{
		Globals::menuKeyPressed = false;
	}


	ImGui::NewFrame();

	if (Globals::menuOpened)
	{
		ImGui::SetNextWindowSize(ImVec2(450, 600));
		ImGui::Begin(
			("Kotatsu Internal"), 
			NULL, 
			ImGuiWindowFlags_NoResize | 
			ImGuiWindowFlags_NoCollapse | 
			//ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoTitleBar 
		);
		
		ImVec2 image_size{ 450, 100 };
		ImVec2 window_size{ ImGui::GetWindowSize() };
		ImVec2 window_pos{ ImGui::GetWindowPos() };
		ImVec2 cursor_pos{ ImGui::GetCursorPos() };

		ImGui::Image((void*)ImageResource, image_size);

		{ImGui::NewLine();
		ImGui::PushStyleColor(ImGuiCol_Button, tabButtonColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, tabButtonColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, tabButtonColor);
		auto mainTabButton = ImGui::Button(("Main"), ImVec2(130, 35));
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		if (mainTabButton)
		{
			Globals::tab = 0;
		}


		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, tabButtonColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, tabButtonColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, tabButtonColor);
		auto miscTabButton = ImGui::Button(("Misc"), ImVec2(130, 35));
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		if (miscTabButton)
		{
			Globals::tab = 1;
		}
		}
		ImGui::Separator();


		if (Globals::tab == 0)
		{
			ImGui::NewLine();
			ImVec4 bhopButtonColor = Globals::bhop ? buttonOnColor : buttonOffColor;
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, bhopButtonColor);
			ImGui::PushStyleColor(ImGuiCol_Button, bhopButtonColor);
			auto bhopButton = ImGui::Button(("BHop"), ImVec2(130, 40));
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			if (bhopButton)
			{
				Globals::bhop = !Globals::bhop;
			}

			ImGui::NewLine();
			ImVec4 espButtonColor = Globals::esp ? buttonOnColor : buttonOffColor;
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, espButtonColor);
			ImGui::PushStyleColor(ImGuiCol_Button, espButtonColor);
			auto espButton = ImGui::Button(("ESP"), ImVec2(130, 40));
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			if (espButton)
			{
				Globals::esp = !Globals::esp;
			}
			ImGui::SameLine();
			ImVec4 allTeamsButtonColor = Globals::allTeams ? buttonOnColor : buttonOffColor;
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, allTeamsButtonColor);
			ImGui::PushStyleColor(ImGuiCol_Button, allTeamsButtonColor);
			auto allTeamsButton = ImGui::Button(("All Teams ESP"), ImVec2(130, 40));
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			if (allTeamsButton)
			{
				Globals::allTeams = !Globals::allTeams;
			}
		}

		// const auto client = reinterpret_cast<uintptr_t>(GetModuleHandle("client.dll"));
		// *reinterpret_cast<int*>(client + Offsets::dwForceAttack) = 256;

		if (Globals::tab == 1)
		{
			ImGui::NewLine();
			if (ImGui::Button(("Clear console"), ImVec2(130, 40)))
			{
				system("cls");
				printf("Kotatsu internal\n");
			}
			ImGui::NewLine();
			if (ImGui::Button(("Deattach"), ImVec2(130, 40)))
			{
				kiero::shutdown();
				std::thread(unload_module).detach();
			}
			ImGui::NewLine();
			if (ImGui::Button(("Fire"), ImVec2(130, 40)))
			{
				// const auto client = reinterpret_cast<uintptr_t>(GetModuleHandle("client.dll"));
				// *reinterpret_cast<int*>(client + Offsets::dwForceAttack) = 65537;
			}
		}
		ImGui::End();
	}


	if (Globals::bhop)
	{
		auto bhop = BHOP();
		bhop.BHop();
	}


	if (Globals::esp)
	{
		ImDrawList* drawList = ImGui::GetBackgroundDrawList();

		auto esp = ESP();

		auto espData = esp.GetESPData();
		for (auto& data : espData)
		{
			float height = data.feetY - data.headY;
			float width = height / 3.5f;
			float hpPercent = data.health / 100.f;

			ImVec2 point1(data.feetX + width, data.feetY);
			ImVec2 point2(data.headX - width, data.headY);
			ImVec2 healthPoint1(data.feetX - width, data.feetY);
			ImVec2 healthPoint2(data.feetX - width, data.headY + height - hpPercent * height);

			ImVec2 headPoint1(data.headX + width / 3, data.headY + height / 5);
			ImVec2 headPoint2(data.headX - width / 3, data.headY);

			ImVec4 greenColor(0.0f, 1.0f, 0.0f, 1.0f);
			ImVec4 redColor(1.0f, 0.0f, 0.0f, 1.0f);
			ImVec4 interpolatedColor;
			interpolatedColor.x = redColor.x + (greenColor.x - redColor.x) * hpPercent;
			interpolatedColor.y = redColor.y + (greenColor.y - redColor.y) * hpPercent;
			interpolatedColor.z = redColor.z + (greenColor.z - redColor.z) * hpPercent;
			interpolatedColor.w = redColor.w + (greenColor.w - redColor.w) * hpPercent;

			drawList->AddRect(point1, point2, IM_COL32(180, 90, 220, 255));
			drawList->AddRect(headPoint1, headPoint2, IM_COL32(255, 0, 0, 255));
			drawList->AddLine(healthPoint1, healthPoint2, ImGui::ColorConvertFloat4ToU32(interpolatedColor), 2.f);
		}
	}


	ImGui::Render();

	pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	return oPresent(pSwapChain, SyncInterval, Flags);
}


DWORD64 WINAPI MainThread(LPVOID lpReserved)
{
	AllocConsole();
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	bool init_hook = false;
	do
	{
		if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
		{
			kiero::bind(8, (void**)& oPresent, hkPresent);
			init_hook = true;
		}
	} while (!init_hook);
	printf("Kotatsu internal\n");
	return TRUE;
}

int __stdcall DllMain(const HMODULE hMod, DWORD64 dwReason, LPVOID lpReserved)
{
	hModule = hMod;
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hMod);
		CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(MainThread), hMod, 0, nullptr);
		break;
	case DLL_PROCESS_DETACH:
		kiero::shutdown();
		break;
	}
	return TRUE;
}
