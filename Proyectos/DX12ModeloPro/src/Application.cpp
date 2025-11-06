#include "Application.h"
#include <iostream>
#include <fstream>
#include <sstream>

void Application::ThrowIfFailed(HRESULT hr, const std::string& msg) {
	if (FAILED(hr)) {
		throw std::runtime_error(msg);
	}
}

void Application::ThrowIfFailed(HRESULT hr) {
	ThrowIfFailed(hr, ":(");
}

void Application::setBlendState(D3D12_BLEND_DESC& blend_desc) {
	blend_desc = {};

	blend_desc.AlphaToCoverageEnable = FALSE;
	blend_desc.IndependentBlendEnable = FALSE;

	D3D12_RENDER_TARGET_BLEND_DESC default_render_target_blend_desc = {};
	default_render_target_blend_desc.BlendEnable = FALSE;
	default_render_target_blend_desc.LogicOpEnable = FALSE;
	default_render_target_blend_desc.SrcBlend = D3D12_BLEND_ONE;
	default_render_target_blend_desc.DestBlend = D3D12_BLEND_ZERO;
	default_render_target_blend_desc.BlendOp = D3D12_BLEND_OP_ADD;
	default_render_target_blend_desc.SrcBlendAlpha = D3D12_BLEND_ONE;
	default_render_target_blend_desc.DestBlendAlpha = D3D12_BLEND_ZERO;
	default_render_target_blend_desc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	default_render_target_blend_desc.LogicOp = D3D12_LOGIC_OP_NOOP;
	default_render_target_blend_desc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; i++) {
		blend_desc.RenderTarget[i] = default_render_target_blend_desc;
	}
}

void Application::setRasterizerState(D3D12_RASTERIZER_DESC& rasterizer_desc) {
	rasterizer_desc = {};

	rasterizer_desc.FillMode = D3D12_FILL_MODE_SOLID;
	rasterizer_desc.CullMode = D3D12_CULL_MODE_BACK;
	rasterizer_desc.FrontCounterClockwise = FALSE;
	rasterizer_desc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	rasterizer_desc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	rasterizer_desc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	rasterizer_desc.DepthClipEnable = TRUE;
	rasterizer_desc.MultisampleEnable = FALSE;
	rasterizer_desc.AntialiasedLineEnable = FALSE;
	rasterizer_desc.ForcedSampleCount = 0;
	rasterizer_desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
}

void Application::setDepthStencilState(D3D12_DEPTH_STENCIL_DESC& depth_stencil_desc) {
	depth_stencil_desc = {};

	depth_stencil_desc.DepthEnable = FALSE;
	depth_stencil_desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D12_COMPARISON_FUNC_GREATER;

	depth_stencil_desc.StencilEnable = FALSE;
	depth_stencil_desc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
	depth_stencil_desc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;

	depth_stencil_desc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	depth_stencil_desc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
}

std::string Application::readFile(const std::string& filename)
{
	std::ifstream myFile(filename);

	if (!myFile.is_open()) {
		return "";
	}

	std::stringstream buffer;
	buffer << myFile.rdbuf();

	return buffer.str();
}

HWND Application::GetWindowNativeHandler() const
{
	return glfwGetWin32Window(window);
}

void Application::setupGeometry()
{
	std::vector<float> geometry{
		// X    Y    Z     W
		-1.0f,  1.0, 0.0f, 1.0f,  //vertice 1
		-1.0f, -1.0, 0.0f, 1.0f,  //vertice 2
		 1.0f, -1.0, 0.0f, 1.0f,  //vertice 3

		1.0f, 0.0f, 0.0f, 1.0f,   //rojo
		0.0f, 1.0f, 0.0f, 1.0f,   //verde
		 0.0f, 0.0f, 1.0f, 1.0f   //azul
	};
}

void Application::keyCallback(int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	//teclas para mover	
}

void Application::setupShaders()
{
	//compile shaders
	ID3DBlob* vertex_shader = nullptr;
	ID3DBlob* pixel_shader = nullptr;
	ThrowIfFailed(D3DCompileFromFile(L"Shaders/shader.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", 0, 0, &vertex_shader, nullptr), "Error compiling shader 1"); 
	ThrowIfFailed(D3DCompileFromFile(L"Shaders/shader.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", 0, 0, &pixel_shader, nullptr), "Error compiling shader 2"); 

	// Pipeline state
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc = {};
	pso_desc.pRootSignature = rootSignature.Get();
	pso_desc.VS.pShaderBytecode = vertex_shader->GetBufferPointer();
	pso_desc.VS.BytecodeLength = vertex_shader->GetBufferSize();
	pso_desc.PS.pShaderBytecode = pixel_shader->GetBufferPointer();
	pso_desc.PS.BytecodeLength = pixel_shader->GetBufferSize();
	setBlendState(pso_desc.BlendState);
	pso_desc.SampleMask = UINT_MAX;
	setRasterizerState(pso_desc.RasterizerState);
	setDepthStencilState(pso_desc.DepthStencilState);
	pso_desc.InputLayout.pInputElementDescs = nullptr;
	pso_desc.InputLayout.NumElements = 0;
	pso_desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	pso_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pso_desc.NumRenderTargets = 1;
	pso_desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	pso_desc.SampleDesc.Count = 1;
	pso_desc.SampleDesc.Quality = 0;

	pipelineState = nullptr;
	ThrowIfFailed(device->CreateGraphicsPipelineState(&pso_desc, IID_PPV_ARGS(&pipelineState)), "Error creating pipeline)");

	vertex_shader->Release();
	vertex_shader = nullptr;
	pixel_shader->Release();
	pixel_shader = nullptr;
}

void Application::SetupConstantBuffer() {
	D3D12_HEAP_PROPERTIES heapProps = {};
	heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProps.CreationNodeMask = 1;
	heapProps.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;

	resourceDesc.Width = sizeof(SceneConstants);
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constantBuffer)
	);
	ThrowIfFailed(constantBuffer->Map(0, nullptr, &mappedMemory), "valio en el mapeo de constant buffer");
	constantBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mappedMemory));
}

void Application::setupDevice()
{
	// Crear el Device (Buscando el adaptador de hardware)
	Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
	for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex) {
		DXGI_ADAPTER_DESC1 desc;
		adapter->GetDesc1(&desc);
		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
		if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(&device)))) {
			break;
		}
	}
}

void Application::setupCommandQueue()
{
	//command queue decides which order the command lists should execute. In our case, we only have one command list.	
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	ThrowIfFailed(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue)), "Failed Command Queue");
}

void Application::setupSwapChain()
{
	//create swap chain
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = 2;
	swapChainDesc.BufferDesc.Width = WINDOW_WIDTH;
	swapChainDesc.BufferDesc.Height = WINDOW_HEIGHT;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.OutputWindow = GetWindowNativeHandler();
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.Windowed = TRUE;
	
	IDXGISwapChain* tempSwapChain = nullptr;
	ThrowIfFailed(factory->CreateSwapChain(commandQueue.Get(), &swapChainDesc, &tempSwapChain), "Failed to create swapchain");

	//cast the swap chain to IDXGISwapChain4 to leverage the latest features
	ThrowIfFailed(tempSwapChain->QueryInterface(IID_PPV_ARGS(&swapChain)), "Error casting to swap chain");
	tempSwapChain->Release();
	tempSwapChain = nullptr;
}

void Application::setupDescriptorHeap()
{
	//memory descriptor heap to store render target views(RTV). Descriptor describes how to interperate resource memory.
	rtvHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = 2;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	ThrowIfFailed(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap)), "Error creating heap");
}

void Application::setupRenderTargetView()
{
	//Crear Render Target Views(RTVs)	
	rtvIncrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	{
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());
		for (UINT i = 0; i < 2; i++) {
			ThrowIfFailed(swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i])), "Failed to create RTV");

			device->CreateRenderTargetView(renderTargets[i].Get(), nullptr, rtvHandle);
			rtvHandle.ptr += rtvIncrementSize;
		}
	}
}

void Application::setupSignature()
{
	//Root signature is like have many object buffers and textures we want to use when drawing.
	//For our rotating triangle, we only need a single constant that is going to be our angle
	D3D12_ROOT_PARAMETER rootParameters[1] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].Constants.Num32BitValues = 0;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	/*rootParameters[0].Constants.ShaderRegister = 0;
	rootParameters[0].Constants.RegisterSpace = 0;*/

	rootSignature = nullptr;
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.NumParameters = _countof(rootParameters);
	rootSignatureDesc.pParameters = rootParameters;
	rootSignatureDesc.NumStaticSamplers = 0;
	rootSignatureDesc.pStaticSamplers = nullptr;
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob), "Error creating signature 1");
		ThrowIfFailed(device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature)), "Error creating signature 2");

	if (signatureBlob) {
		signatureBlob->Release();
		signatureBlob = nullptr;
	}
	if (errorBlob) {
		errorBlob->Release();
		errorBlob = nullptr;
	}
}

void Application::setupCommandAllocator()
{
	//Crear Command Allocator y Command List
	ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator)),
		"Error creando Command Allocator");
	ThrowIfFailed(commandAllocator->Reset(), "Error reseting command Allocator");
}

void Application::setupCommandList()
{
	//command list is used to store a list of commands that we want to execute on the GPU
	ThrowIfFailed(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList)), 
		"Failed to create Command list");
	ThrowIfFailed(commandList->Close(), "Failed to close Commandlist");
}

void Application::update()
{
	++triangle_angle;

	sceneConstants.eye = DirectX::XMVectorSet(0.0f,0.0f,-3.0f,1.0f);
	sceneConstants.center = DirectX::XMVectorSet(0.0f,0.0f,0.0f,1.0f);
	sceneConstants.up = DirectX::XMVectorSet(0.0f,1.0f,0.0f,1.0f);

	sceneConstants.view = DirectX::XMMatrixLookAtLH(sceneConstants.eye, sceneConstants.center, sceneConstants.up);

	float aspect = static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT);
	/*sceneConstants.projection = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XMConvertToRadians(60.0f), 
		aspect, 
		0.1f, 
		1000.0f);*/
	sceneConstants.projection = DirectX::XMMatrixIdentity();
	DirectX::XMVECTOR rotationAxis = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);
	//sceneConstants.model = DirectX::XMMatrixTranspose(DirectX::XMMatrixScaling(.5f, .5f, .5f));
	sceneConstants.model = DirectX::XMMatrixRotationAxis(rotationAxis, DirectX::XMConvertToRadians(triangle_angle));

}

void Application::draw()
{
	// Record commands to draw a triangle
	UINT h = commandAllocator->Reset();
	h = commandList->Reset(commandAllocator.Get(), nullptr);

	UINT back_buffer_index = swapChain->GetCurrentBackBufferIndex();

	{
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = renderTargets[back_buffer_index].Get();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		commandList->ResourceBarrier(1, &barrier);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += back_buffer_index * rtvIncrementSize;

	// Clear the render target
	float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	// Set viewport and scissor
	D3D12_VIEWPORT viewport = { 0.0f, 0.0f, static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT), 0.0f, 1.0f };
	D3D12_RECT scissor_rect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &scissor_rect);
	commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->SetGraphicsRootSignature(rootSignature.Get());
	commandList->SetPipelineState(pipelineState.Get());


	memcpy(mappedMemory, &sceneConstants, sizeof(SceneConstants));
	commandList->SetGraphicsRootConstantBufferView(0, constantBuffer->GetGPUVirtualAddress());
	
	// Draw the triangle
	commandList->DrawInstanced(3, 1, 0, 0);
	
	{

		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = renderTargets[back_buffer_index].Get();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		commandList->ResourceBarrier(1, &barrier);
	}

	h = commandList->Close();

	ID3D12CommandList* commandLists[] = { commandList.Get()};
	commandQueue->ExecuteCommandLists(1, commandLists);

	h = swapChain->Present(1, 0);

}

void Application::setup()
{
	setupGeometry();
	//Inicializa DirectX 12
	// Crear el DXGI Factory		
	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&factory)), "Error creating Factory");
	setupDevice();
	setupCommandQueue();
	setupCommandAllocator();
	setupCommandList();
	setupSwapChain();
	setupDescriptorHeap();
	setupRenderTargetView();
	setupSignature();
	setupShaders();
	SetupConstantBuffer();
}