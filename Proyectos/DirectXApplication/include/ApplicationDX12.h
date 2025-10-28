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
#include <d3dcompiler.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

//https://github.com/microsoft/DirectX-Headers/tree/main

class ApplicationDX12
{
private:
	void ThrowIfFailed(HRESULT hr, const std::string& msg);
	void ThrowIfFailed(HRESULT hr);
	void setupGeometry();
	void setupShaders();
	void setupDevice();
	void setupCommandQueue();
	void setupSwapChain();
	void SetUpSignature();
	void setupDescriptorHeap();
	void setupRenderTargetView();
	void setupCommandAllocator();
	void setupCommandList();

	void setBlendState(D3D12_BLEND_DESC& blend_desc);
	void setRasterizerState(D3D12_RASTERIZER_DESC& rasterizer_desc);
	void setDepthStencilState(D3D12_DEPTH_STENCIL_DESC& depth_stencil_desc);

	LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	std::string readFile(const std::string& filename);

	// --- Configuración y Constantes ---
	static const UINT BUFFER_COUNT = 2; // Double buffering
	const int WINDOW_WIDTH = 1024;
	const int WINDOW_HEIGHT = 768;

	// --- Variables Globales de DX12 ---
	Microsoft::WRL::ComPtr<IDXGIFactory4> factory;

	Microsoft::WRL::ComPtr<ID3D12Device> device;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;


	Microsoft::WRL::ComPtr<ID3D12Resource> renderTargets[BUFFER_COUNT];
	UINT frameIndex;
	UINT rtvDescriptorSize;

	ID3D12DescriptorHeap* rtv_heap;
	UINT rtv_increment_size;
	ID3D12RootSignature* root_signature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> g_pipelineState;
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