#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

#include "Application.h"

Application app;

void MiCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    app.keyCallback(key, scancode, action, mods);
}

int main(void)
{

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    /* Create a windowed mode window and its OpenGL context */
    app.window = glfwCreateWindow(app.WINDOW_WIDTH, app.WINDOW_HEIGHT, "Hello D3D11", nullptr, nullptr);
    if (!app.window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(app.window);


    app.setup();

    //seccion de asignacion de callbacks
    glfwSetKeyCallback(app.window, MiCallback);


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(app.window))
    {
        /* Render here */

        /* Poll for and process events */
        glfwPollEvents();
        app.update();
        //app.clearColorBuffer( 1.0, 1.0, 0.0, 1.0 );
        app.draw();
        /* Swap front and back buffers */
        //glfwSwapBuffers(app.window);
    }

    glfwTerminate();
    return 0;
}

/*
#include <iostream>
#include <vector>
#include <windows.h>
#include <stdexcept>
#include <comdef.h> // Para _com_error

// Incluir GLFW
#include <glfw/glfw3.h>

// Para obtener el HWND de la ventana de GLFW
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw/glfw3native.h>

// Librerías de DirectX 12
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h> // Para Microsoft::WRL::ComPtr

// --- Configuración y Constantes ---
const UINT FRAME_COUNT = 2; // Doble buffering
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const UINT DESCRIPTOR_COUNT = FRAME_COUNT; // Descriptor heap solo para los RTVs

// --- Variables Globales de DX12 ---
Microsoft::WRL::ComPtr<ID3D12Device> g_device;
Microsoft::WRL::ComPtr<ID3D12CommandQueue> g_commandQueue;
Microsoft::WRL::ComPtr<IDXGISwapChain4> g_swapChain;
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> g_rtvHeap;
Microsoft::WRL::ComPtr<ID3D12CommandAllocator> g_commandAllocator;
Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> g_commandList;

Microsoft::WRL::ComPtr<ID3D12Resource> g_renderTargets[FRAME_COUNT];
UINT g_frameIndex;
UINT g_rtvDescriptorSize;

// --- Funciones de Ayuda ---
// Función helper para manejo simple de errores HRESULT
void ThrowIfFailed(HRESULT hr) {
    if (FAILED(hr)) {
        // En una aplicación real, se usaría una mejor gestión de errores.
        throw std::runtime_error("DirectX HRESULT failed.");
    }
}

// ---------------------------------------------
//              1. Inicialización
// ---------------------------------------------

void InitWindow(GLFWwindow** window) {
    if (!glfwInit()) throw std::runtime_error("GLFW failed to initialize!");

    // Deshabilitar la creación de un contexto OpenGL
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "DX12 Clear Nativo", nullptr, nullptr);
    if (!*window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window.");
    }
}

void InitDirectX(HWND hwnd) {
    // 1. Crear el DXGI Factory
    Microsoft::WRL::ComPtr<IDXGIFactory4> factory;
    ThrowIfFailed(CreateDXGIFactory2(0, IID_PPV_ARGS(&factory)));

    // 2. Crear el Device (Buscando el adaptador de hardware)
    Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
    for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex) {
        DXGI_ADAPTER_DESC1 desc;
        adapter->GetDesc1(&desc);
        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
        if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&g_device)))) {
            break;
        }
    }

    // 3. Crear Command Queue
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    ThrowIfFailed(g_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&g_commandQueue)));

    // 4. Crear Swap Chain
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = FRAME_COUNT;
    swapChainDesc.Width = WINDOW_WIDTH;
    swapChainDesc.Height = WINDOW_HEIGHT;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain;
    ThrowIfFailed(factory->CreateSwapChainForHwnd(
        g_commandQueue.Get(),
        hwnd,
        &swapChainDesc,
        nullptr,
        nullptr,
        &swapChain
    ));
    ThrowIfFailed(swapChain.As(&g_swapChain));
    g_frameIndex = g_swapChain->GetCurrentBackBufferIndex();

    // 5. Crear Descriptor Heap para RTVs
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = DESCRIPTOR_COUNT;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    ThrowIfFailed(g_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&g_rtvHeap)));

    g_rtvDescriptorSize = g_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    // 6. Crear Render Target Views (RTVs)
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = g_rtvHeap->GetCPUDescriptorHandleForHeapStart();
    for (UINT i = 0; i < FRAME_COUNT; i++) {
        ThrowIfFailed(g_swapChain->GetBuffer(i, IID_PPV_ARGS(&g_renderTargets[i])));
        g_device->CreateRenderTargetView(g_renderTargets[i].Get(), nullptr, rtvHandle);
        // Correcto: Asegurar que el cálculo sea SIZE_T para punteros
        rtvHandle.ptr += (SIZE_T)g_rtvDescriptorSize;
    }

    // 7. Crear Command Allocator y Command List
    ThrowIfFailed(g_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_commandAllocator)));
    ThrowIfFailed(g_device->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        g_commandAllocator.Get(),
        nullptr, // No necesitamos PSO para solo limpiar
        IID_PPV_ARGS(&g_commandList)
    ));
    g_commandList->Close(); // Cerrar la lista de comandos inicial
}

// ---------------------------------------------
//              2. Renderizado (Clear)
// ---------------------------------------------

void Render() {
    // 1. Resetear el Command Allocator y la Command List
    ThrowIfFailed(g_commandAllocator->Reset());
    ThrowIfFailed(g_commandList->Reset(g_commandAllocator.Get(), nullptr));

    // 2. Obtener el handle al RTV actual
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = g_rtvHeap->GetCPUDescriptorHandleForHeapStart();
    // CORRECCIÓN CLAVE: Asegurar que la aritmética de punteros se realice usando SIZE_T
    // para evitar la violación de acceso si los tipos de datos no son coherentes
    // en la multiplicación (UINT * UINT) antes de la suma al puntero (SIZE_T).
    rtvHandle.ptr += (SIZE_T)g_frameIndex * g_rtvDescriptorSize;

    // 3. Transición del Back Buffer: Present -> Render Target
    // *** USANDO ESTRUCTURAS NATIVAS DE D3D12 (SIN d3dx12.h) ***
    D3D12_RESOURCE_BARRIER transitionToRender = {};
    transitionToRender.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    transitionToRender.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    transitionToRender.Transition.pResource = g_renderTargets[g_frameIndex].Get();
    transitionToRender.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    transitionToRender.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    transitionToRender.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    g_commandList->ResourceBarrier(1, &transitionToRender);

    // 4. Establecer RTV
    g_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

    // 5. ¡El Clear! (ID3D12GraphicsCommandList::ClearRenderTargetView)
    const float clearColor[] = { 0.4f, 0.1f, 0.4f, 1.0f }; // Color: Púrpura
    g_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

    // 6. Transición del Back Buffer: Render Target -> Present
    // *** USANDO ESTRUCTURAS NATIVAS DE D3D12 ***
    D3D12_RESOURCE_BARRIER transitionToPresent = {};
    transitionToPresent.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    transitionToPresent.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    transitionToPresent.Transition.pResource = g_renderTargets[g_frameIndex].Get();
    transitionToPresent.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    transitionToPresent.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    transitionToPresent.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    g_commandList->ResourceBarrier(1, &transitionToPresent);

    // 7. Cerrar y Ejecutar Command List
    ThrowIfFailed(g_commandList->Close());

    ID3D12CommandList* commandLists[] = { g_commandList.Get() };
    g_commandQueue->ExecuteCommandLists(1, commandLists);

    // 8. Presentar el frame y actualizar el índice
    ThrowIfFailed(g_swapChain->Present(1, 0)); // Sincronizado con VSync (1)
    g_frameIndex = g_swapChain->GetCurrentBackBufferIndex();
}

// ---------------------------------------------
//              3. Bucle Principal
// ---------------------------------------------

int main() {
    GLFWwindow* window = nullptr;
    try {
        InitWindow(&window);
        HWND hwnd = glfwGetWin32Window(window);
        InitDirectX(hwnd);

        // Bucle Principal
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            Render();
        }
    } catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        return 1;
    }

    // Limpieza (Se requiere un Fence para sincronización adecuada, pero se omite por simplicidad)
    if (window) glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}


*/