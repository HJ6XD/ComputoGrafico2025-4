#include "Application.h"
#include <fstream>
#include <sstream>

using namespace DirectX;

void Application::ThrowIfFailed(HRESULT hr, const std::string& msg) {
    if (FAILED(hr)) throw std::runtime_error(msg);
}
void Application::ThrowIfFailed(HRESULT hr) { ThrowIfFailed(hr, "HRESULT failed"); }

std::string Application::readFile(const std::string& filename) {
    std::ifstream f(filename);
    if (!f.is_open()) return "";
    std::stringstream ss; ss << f.rdbuf(); return ss.str();
}
HWND Application::GetWindowNativeHandler() const { return glfwGetWin32Window(window); }

void Application::keyCallback(int key, int, int action, int) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void Application::setBlendState(D3D12_BLEND_DESC& d) {
    d = {};
    d.AlphaToCoverageEnable = FALSE;
    d.IndependentBlendEnable = FALSE;
    D3D12_RENDER_TARGET_BLEND_DESC rt = {};
    rt.BlendEnable = FALSE;
    rt.LogicOpEnable = FALSE;
    rt.SrcBlend = D3D12_BLEND_ONE;
    rt.DestBlend = D3D12_BLEND_ZERO;
    rt.BlendOp = D3D12_BLEND_OP_ADD;
    rt.SrcBlendAlpha = D3D12_BLEND_ONE;
    rt.DestBlendAlpha = D3D12_BLEND_ZERO;
    rt.BlendOpAlpha = D3D12_BLEND_OP_ADD;
    rt.LogicOp = D3D12_LOGIC_OP_NOOP;
    rt.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i) d.RenderTarget[i] = rt;
}
void Application::setRasterizerState(D3D12_RASTERIZER_DESC& d) {
    d = {};
    d.FillMode = D3D12_FILL_MODE_SOLID;
    d.CullMode = D3D12_CULL_MODE_BACK;
    d.FrontCounterClockwise = FALSE;
    d.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
    d.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
    d.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    d.DepthClipEnable = TRUE;
    d.MultisampleEnable = FALSE;
    d.AntialiasedLineEnable = FALSE;
    d.ForcedSampleCount = 0;
    d.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
}
void Application::setDepthStencilState(D3D12_DEPTH_STENCIL_DESC& d) {
    d = {};
    d.DepthEnable = TRUE;
    d.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    d.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
    d.StencilEnable = FALSE;
    d.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
    d.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
    d.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
    d.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
    d.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
    d.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    d.BackFace = d.FrontFace;
}

Model Application::load_model_from_obj(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return {};

    Model model;
    std::vector<XMFLOAT3> temp_positions;
    std::vector<XMFLOAT3> temp_normals;
    std::map<std::pair<unsigned int, unsigned int>, unsigned int> index_map;
    std::vector<unsigned int> temp_face_indices;

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string prefix; ss >> prefix;
        if (prefix == "v") {
            XMFLOAT3 p; ss >> p.x >> p.y >> p.z; temp_positions.push_back(p);
        }
        else if (prefix == "vn") {
            XMFLOAT3 n; ss >> n.x >> n.y >> n.z; temp_normals.push_back(n);
        }
        else if (prefix == "f") {
            temp_face_indices.clear();
            std::string token;
            while (ss >> token) {
                size_t slash_pos = token.find("//");
                unsigned int vi = std::stoul(token.substr(0, slash_pos)) - 1;
                unsigned int ni = std::stoul(token.substr(slash_pos + 2)) - 1;
                auto key = std::make_pair(vi, ni);
                auto it = index_map.find(key);
                if (it != index_map.end()) {
                    temp_face_indices.push_back(it->second);
                }
                else {
                    Vertex v; v.position = temp_positions[vi]; v.normal = temp_normals[ni];
                    unsigned int new_index = (unsigned int)model.vertices.size();
                    model.vertices.push_back(v);
                    index_map[key] = new_index;
                    temp_face_indices.push_back(new_index);
                }
            }
            for (int i = 0; i < (int)temp_face_indices.size() - 2; i++) {
                model.indicies.push_back(temp_face_indices[0]);
                model.indicies.push_back(temp_face_indices[i + 1]);
                model.indicies.push_back(temp_face_indices[i + 2]);
            }
        }
    }
    file.close();
    return model;
}

void Application::setupDevice() {
    ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&factory)), "Error creating Factory");

    Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
    for (UINT i = 0; factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i) {
        DXGI_ADAPTER_DESC1 desc;
        adapter->GetDesc1(&desc);
        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
        if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device)))) break;
    }
    if (!device) {
        // WARP fallback
        Microsoft::WRL::ComPtr<IDXGIAdapter> warp;
        ThrowIfFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&warp)));
        ThrowIfFailed(D3D12CreateDevice(warp.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device)));
    }
}

void Application::setupCommandQueue() {
    D3D12_COMMAND_QUEUE_DESC q = {};
    q.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    q.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    ThrowIfFailed(device->CreateCommandQueue(&q, IID_PPV_ARGS(&commandQueue)), "Failed Command Queue");
}

void Application::setupSwapChain() {
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = BUFFER_COUNT;
    sd.BufferDesc.Width = WINDOW_WIDTH;
    sd.BufferDesc.Height = WINDOW_HEIGHT;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    sd.OutputWindow = GetWindowNativeHandler();
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;

    Microsoft::WRL::ComPtr<IDXGISwapChain> temp;
    ThrowIfFailed(factory->CreateSwapChain(commandQueue.Get(), &sd, &temp), "Failed to create swapchain");
    ThrowIfFailed(temp.As(&swapChain), "Error casting to IDXGISwapChain4");
}

void Application::setupDescriptorHeaps() {
    // RTV heap
    D3D12_DESCRIPTOR_HEAP_DESC rtvDesc = {};
    rtvDesc.NumDescriptors = BUFFER_COUNT;
    rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    ThrowIfFailed(device->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(&rtvHeap)), "Error creating RTV heap");
    rtvIncrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    // DSV heap
    D3D12_DESCRIPTOR_HEAP_DESC dsvDesc = {};
    dsvDesc.NumDescriptors = 1;
    dsvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    ThrowIfFailed(device->CreateDescriptorHeap(&dsvDesc, IID_PPV_ARGS(&dsvHeap)), "Error creating DSV heap");
    dsvIncrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void Application::setupRenderTargetView() {
    D3D12_CPU_DESCRIPTOR_HANDLE h = rtvHeap->GetCPUDescriptorHandleForHeapStart();
    for (UINT i = 0; i < BUFFER_COUNT; ++i) {
        ThrowIfFailed(swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i])), "Failed GetBuffer");
        device->CreateRenderTargetView(renderTargets[i].Get(), nullptr, h);
        h.ptr += rtvIncrementSize;
    }
}

void Application::setupDepthBuffer() {

    D3D12_HEAP_PROPERTIES hp = {}; hp.Type = D3D12_HEAP_TYPE_DEFAULT;
    D3D12_RESOURCE_DESC rd = {};
    rd.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    rd.Alignment = 0;
    rd.Width = WINDOW_WIDTH;
    rd.Height = WINDOW_HEIGHT;
    rd.DepthOrArraySize = 1;
    rd.MipLevels = 1;
    rd.Format = DXGI_FORMAT_D32_FLOAT;
    rd.SampleDesc.Count = 1;
    rd.SampleDesc.Quality = 0;
    rd.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    rd.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE cv = {};
    cv.Format = DXGI_FORMAT_D32_FLOAT;
    cv.DepthStencil.Depth = 1.0f;
    cv.DepthStencil.Stencil = 0;

    ThrowIfFailed(device->CreateCommittedResource(
        &hp, D3D12_HEAP_FLAG_NONE, &rd,
        D3D12_RESOURCE_STATE_DEPTH_WRITE, &cv,
        IID_PPV_ARGS(&depthStencilBuffer)), "Failed to create Depth Buffer");

    // DSV, dsvHeap
    D3D12_CPU_DESCRIPTOR_HANDLE dsv = dsvHeap->GetCPUDescriptorHandleForHeapStart();
    device->CreateDepthStencilView(depthStencilBuffer.Get(), nullptr, dsv);
}

void Application::setupCommandAllocator() {
    ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator)),
        "Error creando Command Allocator");
    ThrowIfFailed(commandAllocator->Reset(), "Error reseting command Allocator");
}
void Application::setupCommandList() {
    ThrowIfFailed(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(),
        nullptr, IID_PPV_ARGS(&commandList)),
        "Failed to create Command list");
    ThrowIfFailed(commandList->Close(), "Failed to close CommandList");
}

void Application::setupSignature() {
    // CBV b0
    D3D12_ROOT_PARAMETER rp = {};
    rp.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rp.Descriptor.ShaderRegister = 0;
    rp.Descriptor.RegisterSpace = 0;
    rp.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

    D3D12_ROOT_SIGNATURE_DESC rsd = {};
    rsd.NumParameters = 1;
    rsd.pParameters = &rp;
    rsd.NumStaticSamplers = 0;
    rsd.pStaticSamplers = nullptr;
    rsd.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    Microsoft::WRL::ComPtr<ID3DBlob> sig, err;
    ThrowIfFailed(D3D12SerializeRootSignature(&rsd, D3D_ROOT_SIGNATURE_VERSION_1, &sig, &err), "Serialize RS");
    ThrowIfFailed(device->CreateRootSignature(0, sig->GetBufferPointer(), sig->GetBufferSize(),
        IID_PPV_ARGS(&rootSignature)), "Create RS");
}

void Application::setupShaders() {
    Microsoft::WRL::ComPtr<ID3DBlob> vs, ps;
    ThrowIfFailed(D3DCompileFromFile(L"Shaders/shaderConejo.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", 0, 0, &vs, nullptr),
        "Compile VS");
    ThrowIfFailed(D3DCompileFromFile(L"Shaders/shaderConejo.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", 0, 0, &ps, nullptr),
        "Compile PS");

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pso = {};
    pso.pRootSignature = rootSignature.Get();
    pso.VS = { vs->GetBufferPointer(), vs->GetBufferSize() };
    pso.PS = { ps->GetBufferPointer(), ps->GetBufferSize() };
    setBlendState(pso.BlendState);
    pso.SampleMask = UINT_MAX;
    setRasterizerState(pso.RasterizerState);
    setDepthStencilState(pso.DepthStencilState);

    D3D12_INPUT_ELEMENT_DESC input[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };
    pso.InputLayout = { input, _countof(input) };
    pso.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
    pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pso.NumRenderTargets = 1;
    pso.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    pso.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    pso.SampleDesc.Count = 1;
    ThrowIfFailed(device->CreateGraphicsPipelineState(&pso, IID_PPV_ARGS(&pipelineState)), "Create PSO");
}

void Application::setupConstantBuffer() {
    // UPLOAD buffer CPU write / GPU read
    D3D12_HEAP_PROPERTIES hp = {}; hp.Type = D3D12_HEAP_TYPE_UPLOAD;
    D3D12_RESOURCE_DESC rd = {};
    rd.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    rd.Alignment = 0;
    rd.Width = (sizeof(SceneConstants) + 255) & ~255u;
    rd.Height = 1;
    rd.DepthOrArraySize = 1;
    rd.MipLevels = 1;
    rd.Format = DXGI_FORMAT_UNKNOWN;
    rd.SampleDesc.Count = 1;
    rd.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    ThrowIfFailed(device->CreateCommittedResource(&hp, D3D12_HEAP_FLAG_NONE, &rd,
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
        IID_PPV_ARGS(&constantBuffer)),
        "Create CB");
    ThrowIfFailed(constantBuffer->Map(0, nullptr, &mappedMemory), "Map CB");
}

void Application::setupGeometry() {
    modelData = load_model_from_obj("rabbit.obj");

    size_t vbSize = sizeof(Vertex) * modelData.vertices.size();
    size_t ibSize = sizeof(unsigned int) * modelData.indicies.size();

    D3D12_HEAP_PROPERTIES hpDefault = {}; hpDefault.Type = D3D12_HEAP_TYPE_DEFAULT;
    D3D12_HEAP_PROPERTIES hpUpload = {}; hpUpload.Type = D3D12_HEAP_TYPE_UPLOAD;
    D3D12_RESOURCE_DESC bd = {};
    bd.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    bd.Alignment = 0;
    bd.Height = 1; bd.DepthOrArraySize = 1; bd.MipLevels = 1;
    bd.Format = DXGI_FORMAT_UNKNOWN; bd.SampleDesc.Count = 1;
    bd.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    // VB
    bd.Width = vbSize;
    ThrowIfFailed(device->CreateCommittedResource(&hpDefault, D3D12_HEAP_FLAG_NONE, &bd,
        D3D12_RESOURCE_STATE_COMMON, nullptr,
        IID_PPV_ARGS(&vbDefault)), "Create VB default");
    ThrowIfFailed(device->CreateCommittedResource(&hpUpload, D3D12_HEAP_FLAG_NONE, &bd,
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
        IID_PPV_ARGS(&vbUpload)), "Create VB upload");

    // IB
    bd.Width = ibSize;
    ThrowIfFailed(device->CreateCommittedResource(&hpDefault, D3D12_HEAP_FLAG_NONE, &bd,
        D3D12_RESOURCE_STATE_COMMON, nullptr,
        IID_PPV_ARGS(&ibDefault)), "Create IB default");
    ThrowIfFailed(device->CreateCommittedResource(&hpUpload, D3D12_HEAP_FLAG_NONE, &bd,
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
        IID_PPV_ARGS(&ibUpload)), "Create IB upload");

    void* dst = nullptr;
    ThrowIfFailed(vbUpload->Map(0, nullptr, &dst), "Map VB upload");
    memcpy(dst, modelData.vertices.data(), vbSize);
    vbUpload->Unmap(0, nullptr);

    ThrowIfFailed(ibUpload->Map(0, nullptr, &dst), "Map IB upload");
    memcpy(dst, modelData.indicies.data(), ibSize);
    ibUpload->Unmap(0, nullptr);

    vbv.BufferLocation = vbDefault->GetGPUVirtualAddress();
    vbv.StrideInBytes = sizeof(Vertex);
    vbv.SizeInBytes = (UINT)vbSize;

    ibv.BufferLocation = ibDefault->GetGPUVirtualAddress();
    ibv.SizeInBytes = (UINT)ibSize;
    ibv.Format = DXGI_FORMAT_R32_UINT;
}

void Application::uploadStaticBuffers() {
    ThrowIfFailed(commandAllocator->Reset());
    ThrowIfFailed(commandList->Reset(commandAllocator.Get(), nullptr));

    D3D12_RESOURCE_BARRIER b[2] = {};
    b[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    b[0].Transition.pResource = vbDefault.Get();
    b[0].Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
    b[0].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
    b[0].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    b[1] = b[0];
    b[1].Transition.pResource = ibDefault.Get();

    commandList->ResourceBarrier(2, b);
    commandList->CopyResource(vbDefault.Get(), vbUpload.Get());
    commandList->CopyResource(ibDefault.Get(), ibUpload.Get());

    b[0].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    b[0].Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
    b[1].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    b[1].Transition.StateAfter = D3D12_RESOURCE_STATE_INDEX_BUFFER;
    commandList->ResourceBarrier(2, b);

    ThrowIfFailed(commandList->Close());
    ID3D12CommandList* lists[] = { commandList.Get() };
    commandQueue->ExecuteCommandLists(1, lists);

    // Fence
    if (!fence) {
        ThrowIfFailed(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)), "Create Fence");
        fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    }
    waitGPU();
}

void Application::waitGPU() {
    const UINT64 fv = ++fenceValue;
    ThrowIfFailed(commandQueue->Signal(fence.Get(), fv), "Signal fence");
    if (fence->GetCompletedValue() < fv) {
        ThrowIfFailed(fence->SetEventOnCompletion(fv, fenceEvent), "Set fence event");
        WaitForSingleObject(fenceEvent, INFINITE);
    }
}

void Application::setup() {
    setupDevice();
    setupCommandQueue();
    setupCommandAllocator();
    setupCommandList();
    setupSwapChain();
    setupDescriptorHeaps();
    setupRenderTargetView();
    setupDepthBuffer();
    setupSignature();
    setupShaders();
    setupConstantBuffer();
    setupGeometry();
    uploadStaticBuffers();
}

void Application::update() {
    sceneConstants.triangleAngle++;
    sceneConstants.eye = XMVectorSet(0.0f, 0.0f, -3.0f, 1.0f); // Posición de la cámara
    sceneConstants.center = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);  // Punto al que mira
    sceneConstants.up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);    // Vector 'Up'

    sceneConstants.view = XMMatrixLookAtLH(sceneConstants.eye, sceneConstants.center, sceneConstants.up);

    float aspect = static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT);
	sceneConstants.projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), aspect, 0.1f, 1000.0f); // Proyección

	// Modelo: rotación alrededor del eje Y sin angulo variable
	XMVECTOR axis = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f); // Eje Y
	float angleRad = XMConvertToRadians((float)(sceneConstants.triangleAngle % 360)); // Ángulo en radianes
    sceneConstants.model = XMMatrixRotationAxis(axis,angleRad) * XMConvertToRadians(sceneConstants.triangleAngle);

    memcpy(mappedMemory, &sceneConstants, sizeof(SceneConstants));// Sizeof es la cantidad de bits a copiar
}

void Application::draw() {
    ThrowIfFailed(commandAllocator->Reset());
    ThrowIfFailed(commandList->Reset(commandAllocator.Get(), nullptr));

    UINT backIdx = swapChain->GetCurrentBackBufferIndex();

    {
        D3D12_RESOURCE_BARRIER br = {};
        br.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        br.Transition.pResource = renderTargets[backIdx].Get();
        br.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        br.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
        br.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        commandList->ResourceBarrier(1, &br);
    }

    // RTV / DSV
    D3D12_CPU_DESCRIPTOR_HANDLE rtv = rtvHeap->GetCPUDescriptorHandleForHeapStart();
    rtv.ptr += backIdx * rtvIncrementSize;
    D3D12_CPU_DESCRIPTOR_HANDLE dsv = dsvHeap->GetCPUDescriptorHandleForHeapStart();

    // Clear
    float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
    commandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
    commandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    // Viewport / Scissor
    D3D12_VIEWPORT vp = { 0.0f, 0.0f, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT, 0.0f, 1.0f };
    D3D12_RECT sc = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
    commandList->RSSetViewports(1, &vp);
    commandList->RSSetScissorRects(1, &sc);

    commandList->OMSetRenderTargets(1, &rtv, FALSE, &dsv);
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandList->SetGraphicsRootSignature(rootSignature.Get());
    commandList->SetPipelineState(pipelineState.Get());

    commandList->IASetVertexBuffers(0, 1, &vbv);
    commandList->IASetIndexBuffer(&ibv);
    commandList->SetGraphicsRootConstantBufferView(0, constantBuffer->GetGPUVirtualAddress());

    commandList->DrawIndexedInstanced((UINT)modelData.indicies.size(), 1, 0, 0, 0);

    {
        D3D12_RESOURCE_BARRIER br = {};
        br.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        br.Transition.pResource = renderTargets[backIdx].Get();
        br.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        br.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
        br.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        commandList->ResourceBarrier(1, &br);
    }

    ThrowIfFailed(commandList->Close());
    ID3D12CommandList* lists[] = { commandList.Get() };
    commandQueue->ExecuteCommandLists(1, lists);

    ThrowIfFailed(swapChain->Present(1, 0));
    waitGPU();
}
