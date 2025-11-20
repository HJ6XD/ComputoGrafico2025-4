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
#include <DirectXMath.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h> // Para Microsoft::WRL::ComPtr
#include <d3dcompiler.h>

#include <map>
#include <string>
#include <stdexcept>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

struct Vertex {
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 normal;
};

struct Model {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indicies;
};

typedef struct
{
    DirectX::XMMATRIX model;      // 64 bytes
    DirectX::XMMATRIX view;       // 64 bytes
    DirectX::XMMATRIX projection; // 64 bytes

    DirectX::XMVECTOR eye;        // 16 bytes
    DirectX::XMVECTOR center;     // 16 bytes
    DirectX::XMVECTOR up;         // 16 bytes

    UINT triangleAngle;           // 4 bytes
    float padding[3];             // padding a 16 bytes
} SceneConstants;                 // 256 bytes (alineado para CBV)

class Application
{
private:
    // Helpers de errores
    void ThrowIfFailed(HRESULT hr, const std::string& msg);
    void ThrowIfFailed(HRESULT hr);

    // Etapas de setup
    void setupGeometry();              // Carga el modelo y crea VB/IB
    void setupShaders();               // Compila shaders y crea PSO
    void setupConstantBuffer();        // Crea CBV (UPLOAD) y mapea
    void setupDevice();                // Crea device
    void setupCommandQueue();          // Crea CommandQueue
    void setupCommandAllocator();      // Crea CommandAllocator
    void setupCommandList();           // Crea CommandList
    void setupSwapChain();             // Crea SwapChain
    void setupDescriptorHeaps();       // Crea RTV y DSV heaps
    void setupRenderTargetView();      // Crea RTVs de los back buffers
    void setupDepthBuffer();           // Crea DepthBuffer + DSV
    void setupSignature();             // Crea RootSignature (CBV b0)
	void uploadStaticBuffers();        // Sube VB/IB a memoria GPU
    void waitGPU();                    // Sincronización (Fence)

    // Estados fijos
    void setBlendState(D3D12_BLEND_DESC& blend_desc);
    void setRasterizerState(D3D12_RASTERIZER_DESC& rasterizer_desc);
    void setDepthStencilState(D3D12_DEPTH_STENCIL_DESC& depth_stencil_desc);

    // Utilidades
    std::string readFile(const std::string& filename);
    HWND GetWindowNativeHandler() const;

    Model load_model_from_obj(const std::string& path);

    static const UINT BUFFER_COUNT = 2; // Double buffering

    // --- DXGI / D3D12 ---
    Microsoft::WRL::ComPtr<IDXGIFactory4> factory;

    Microsoft::WRL::ComPtr<ID3D12Device> device;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
    Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain{};

    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;

    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;

    Microsoft::WRL::ComPtr<ID3D12Resource> renderTargets[BUFFER_COUNT];
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;
    Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilBuffer;

    UINT rtvIncrementSize = 0;
    UINT dsvIncrementSize = 0;

    // CBV
    Microsoft::WRL::ComPtr<ID3D12Resource> constantBuffer;
    void* mappedMemory = nullptr;
    SceneConstants sceneConstants{};

    // VB / IB
    Model modelData;
    Microsoft::WRL::ComPtr<ID3D12Resource> vbDefault;
    Microsoft::WRL::ComPtr<ID3D12Resource> vbUpload;
    Microsoft::WRL::ComPtr<ID3D12Resource> ibDefault;
    Microsoft::WRL::ComPtr<ID3D12Resource> ibUpload;
    D3D12_VERTEX_BUFFER_VIEW vbv{};
    D3D12_INDEX_BUFFER_VIEW  ibv{};

    // Fence para sincronización
    Microsoft::WRL::ComPtr<ID3D12Fence> fence;
    UINT64 fenceValue = 0;
    HANDLE fenceEvent = nullptr;

public:
    const int WINDOW_WIDTH = 1024;
    const int WINDOW_HEIGHT = 768;
    GLFWwindow* window = nullptr;

    void setup();
    void update();
    void draw();
    void keyCallback(int key, int scancode, int action, int mods);
};
