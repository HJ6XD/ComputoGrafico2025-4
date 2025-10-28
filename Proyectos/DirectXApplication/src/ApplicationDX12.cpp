#include "ApplicationDX12.h"
#include <iostream>
#include <fstream>
#include <sstream>

//Ya Actual
void ApplicationDX12::ThrowIfFailed(HRESULT hr, const std::string& msg) {
	if (FAILED(hr)) {
		throw std::runtime_error(msg);
	}
}

//Ya Actual
void ApplicationDX12::ThrowIfFailed(HRESULT hr) {
	ThrowIfFailed(hr, ":(");
}

//Ya Actual
std::string ApplicationDX12::readFile(const std::string& filename)
{
	std::ifstream myFile(filename);

	if (!myFile.is_open()) {
		return "";
	}

	std::stringstream buffer;
	buffer << myFile.rdbuf();

	return buffer.str();
}

//Ya Actual
HWND ApplicationDX12::GetWindowNativeHandler() const
{
	return glfwGetWin32Window(window);
}

void ApplicationDX12::setupGeometry()
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

void ApplicationDX12::keyCallback(int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	//teclas para mover	
}

void ApplicationDX12::setupShaders()
{
	ID3DBlob* vertex_shader = nullptr;
	ID3DBlob* pixel_shader = nullptr;
	ThrowIfFailed(D3DCompileFromFile(L"shader.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", 0, 0, &vertex_shader, nullptr));
	ThrowIfFailed(D3DCompileFromFile(L"shader.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", 0, 0, &pixel_shader, nullptr));

	//Pipeline State
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc = {};
	pso_desc.pRootSignature = root_signature;
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

	g_pipelineState = nullptr;
	ThrowIfFailed(device->CreateGraphicsPipelineState(&pso_desc, IID_PPV_ARGS(&g_pipelineState)));

	vertex_shader->Release();
	vertex_shader = nullptr;
	pixel_shader->Release();
	pixel_shader = nullptr;

}

//Ya Actual
void ApplicationDX12::setupDevice()
{
	// Crear el Device (Buscando el adaptador de hardware)
	Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
	Microsoft::WRL::ComPtr<ID3D12Device> device;

	for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex) {
		DXGI_ADAPTER_DESC1 desc;
		adapter->GetDesc1(&desc);
		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
		if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(&device)))) {
			break;
		}
	}
	//std::cout << "Failed to create d3d11 device and context." << std::endl;
}

//Ya actual
void ApplicationDX12::setupCommandQueue()
{
	// Crear Command Queue
	ID3D12CommandQueue* commandQueue = nullptr;
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	ThrowIfFailed(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue)), "Error creating Command Queue");
}

//Ya Actual
void ApplicationDX12::setupSwapChain()
{
	DXGI_SWAP_CHAIN_DESC swap_chain_desc = {};
	swap_chain_desc.BufferCount = 2;
	swap_chain_desc.BufferDesc.Width = WINDOW_WIDTH;
	swap_chain_desc.BufferDesc.Height = WINDOW_HEIGHT;
	swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swap_chain_desc.OutputWindow = GetWindowNativeHandler();
	swap_chain_desc.SampleDesc.Count = 1;
	swap_chain_desc.Windowed = TRUE;

	IDXGISwapChain* temp_swap_chain = nullptr;
	ThrowIfFailed(factory->CreateSwapChain(commandQueue.Get(), &swap_chain_desc, &temp_swap_chain), "buuuuu");

	IDXGISwapChain3 *swap_chain = {};
	ThrowIfFailed(temp_swap_chain->QueryInterface(IID_PPV_ARGS(&swap_chain)), "Error creando Swapchain");
	temp_swap_chain->Release();
	temp_swap_chain = nullptr;
}

//YaActual
void ApplicationDX12::SetUpSignature()
{
	D3D12_ROOT_PARAMETER root_parameters[1] = {};
	root_parameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	root_parameters[0].Constants.Num32BitValues = 1;
	root_parameters[0].Constants.ShaderRegister = 0;
	root_parameters[0].Constants.RegisterSpace = 0;
	root_parameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	root_signature = nullptr;
	D3D12_ROOT_SIGNATURE_DESC root_signature_desc = {};
	root_signature_desc.NumParameters = _countof(root_parameters);
	root_signature_desc.pParameters = root_parameters;
	root_signature_desc.NumStaticSamplers = 0;
	root_signature_desc.pStaticSamplers = nullptr;
	root_signature_desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ID3DBlob* signature_blob = nullptr;
	ID3DBlob* error_blob = nullptr;
	ThrowIfFailed(D3D12SerializeRootSignature(&root_signature_desc, D3D_ROOT_SIGNATURE_VERSION_1, &signature_blob, &error_blob));
	ThrowIfFailed(device->CreateRootSignature(0, signature_blob->GetBufferPointer(), signature_blob->GetBufferSize(), IID_PPV_ARGS(&root_signature)));

	if (signature_blob) {
		signature_blob->Release();
		signature_blob = nullptr;
	}
	if (error_blob) {
		error_blob->Release();
		error_blob = nullptr;
	}
}

//Ya actual
void ApplicationDX12::setupDescriptorHeap()
{
	//Crear Descriptor Heap para RTVs
	rtv_heap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC rtv_heap_desc = {};
	rtv_heap_desc.NumDescriptors = 2;
	rtv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	ThrowIfFailed(device->CreateDescriptorHeap(&rtv_heap_desc, IID_PPV_ARGS(&rtv_heap)));
	
	ID3D12Resource* render_targets[2];

	rtv_increment_size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	{
		D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle(rtv_heap->GetCPUDescriptorHandleForHeapStart());
		for (UINT i = 0; i < 2; i++) {
			ThrowIfFailed(swapChain->GetBuffer(i, IID_PPV_ARGS(&render_targets[i])));

			device->CreateRenderTargetView(render_targets[i], nullptr, rtv_handle);
			rtv_handle.ptr += rtv_increment_size;
		}
	}
}

void ApplicationDX12::setupRenderTargetView()
{
	//Crear Render Target Views(RTVs)
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
	for (UINT i = 0; i < BUFFER_COUNT; i++) {
		ThrowIfFailed(swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i])), "Error al crear Target View");
		device->CreateRenderTargetView(renderTargets[i].Get(), nullptr, rtvHandle);
		// Correcto: Asegurar que el cálculo sea SIZE_T para punteros
		rtvHandle.ptr += (SIZE_T)rtvDescriptorSize;
	}
}

//Ya Actual
void ApplicationDX12::setupCommandAllocator()
{
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> command_allocator = nullptr;
	//Crear Command Allocator y Command List
	ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator)),
		"Error creando Command Allocator");
	ThrowIfFailed(commandAllocator->Reset(), "Error creando Command Allocator");
}

//Ya Actual
void ApplicationDX12::setupCommandList()
{
	ID3D12GraphicsCommandList* command_list = nullptr;
	ThrowIfFailed(device->CreateCommandList(0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		commandAllocator.Get(),
		nullptr, // No necesitamos PSO para solo limpiar
		IID_PPV_ARGS(&commandList)), "Error creando Command Queue");
	
	ThrowIfFailed(command_list->Close(), "Error creando Command Queue");
}

//Ya actual
void ApplicationDX12::setBlendState(D3D12_BLEND_DESC& blend_desc)
{
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

//Ya actual
void ApplicationDX12::setRasterizerState(D3D12_RASTERIZER_DESC& rasterizer_desc)
{
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

//Ya actual
void ApplicationDX12::clearColorBuffer(const float& r, const float& g, const float& b, const float& a)
{
	// Obtener el handle al RTV actual
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();

	// Establecer RTV (Aunque solo limpiemos, es buena práctica)
	rtvHandle.ptr += (SIZE_T)frameIndex * rtvDescriptorSize;
	commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	// 5. ¡El Clear! (ID3D12GraphicsCommandList::ClearRenderTargetView)
	const float clearColor[] = { r, g, b, a };
	commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

}

void ApplicationDX12::setup()
{

	setupGeometry();
	//Inicializa DirectX 12
	// Crear el DXGI Factory	
	//Ya Actual
	factory = nullptr;
	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&factory)), "No jalo el factry");
	
	setupDevice();
	setupCommandQueue();
	setupSwapChain();
	setupDescriptorHeap();
	setupRenderTargetView();
	setupCommandAllocator();
	setupCommandList();
	setupShaders();
}

void ApplicationDX12::update()
{
}

//Ya Actual
void ApplicationDX12::setDepthStencilState(D3D12_DEPTH_STENCIL_DESC& depth_stencil_desc)
{
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

//Ya actual
LRESULT ApplicationDX12::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

void ApplicationDX12::draw()
{
	// Resetear el Command Allocator y la Command List
	ThrowIfFailed(commandAllocator->Reset(), "Error borrando el Command Allocator");
	ThrowIfFailed(commandList->Reset(commandAllocator.Get(), nullptr), "Error borrando el Command List");

	// Transición del Back Buffer: Present -> Render Target
	{
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = renderTargets[frameIndex].Get();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		commandList->ResourceBarrier(1, &barrier);
	}
	//Borrar el buffer

	D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle = rtv_heap->GetCPUDescriptorHandleForHeapStart();
	rtv_handle.ptr += frameIndex * rtv_increment_size;

	// Clear the render target
	float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	commandList->ClearRenderTargetView(rtv_handle, clearColor, 0, nullptr);

	// Set viewport and scissor
	D3D12_VIEWPORT viewport = { 0.0f, 0.0f, static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT), 0.0f, 1.0f };
	D3D12_RECT scissor_rect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &scissor_rect);
		   
	commandList->OMSetRenderTargets(1, &rtv_handle, FALSE, nullptr);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->SetGraphicsRootSignature(root_signature);
	commandList->SetPipelineState(g_pipelineState.Get());

	// Draw the triangle
	//command_list->SetGraphicsRoot32BitConstant(0, triangle_angle, 0);
	commandList->DrawInstanced(3, 1, 0, 0);


	//render !!
	//Agrega aqui tus comandos

	{
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = renderTargets[frameIndex].Get();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		commandList->ResourceBarrier(1, &barrier);
	}

	// Cerrar y Ejecutar Command List
	ThrowIfFailed(commandList->Close(), "Error al cerrar Command List");

	ID3D12CommandList* commandLists[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists(1, commandLists);

	// 8. Presentar el frame y actualizar el índice
	ThrowIfFailed(swapChain->Present(1, 0), "Error al renderizar el cuadro"); // Sincronizado con VSync (1)
}