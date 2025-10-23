#pragma once

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
// Para obtener el HWND de la ventana de GLFW
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

#include <iostream>
#include <vector>
#include <windows.h>


// Librerías de DirectX 12
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h> // Para Microsoft::WRL::ComPtr


#include <map>
#include <string>


//https://github.com/microsoft/DirectX-Headers/tree/main



class Application
{	
private:
	void ThrowIfFailed(HRESULT hr, const std::string& msg);
	void ThrowIfFailed(HRESULT hr);
	void setupGeometry();
	void setupShaders();
	void setupDevice();
	void setupCommandQueue();
	void setupSwapChain();
	void setupDescriptorHeap();
	void setupRenderTargetView();
	void setupCommandAllocator();
	void setupCommandList();
	void swapBuffers();

	std::string readFile(const std::string& filename);

	// --- Configuración y Constantes ---
	static const UINT BUFFER_COUNT = 2; // Double buffering
	const int WINDOW_WIDTH = 1024;
	const int WINDOW_HEIGHT = 768;

	// --- Variables Globales de DX12 ---
	Microsoft::WRL::ComPtr<IDXGIFactory4> factory;

	Microsoft::WRL::ComPtr<ID3D12Device> g_device;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> g_commandQueue;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> g_swapChain;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> g_rtvHeap;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> g_commandAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> g_commandList;


	Microsoft::WRL::ComPtr<ID3D12Resource> g_renderTargets[BUFFER_COUNT];
	UINT g_frameIndex;
	UINT g_rtvDescriptorSize;
	


public:
	const int Width{ 1280 };
	const int Height{ 1024 };
	GLFWwindow* window;
	HWND GetWindowNativeHandler() const;
	void setup();
	void update();
	void draw();
	void clearColorBuffer(const float& r, const float& g, const float& b, const float& a);
	void keyCallback(int key, int scancode, int action, int mods);
};