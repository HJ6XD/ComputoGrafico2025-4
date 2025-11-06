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
#include "DirectXMath.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h> // Para Microsoft::WRL::ComPtr
#include <d3dcompiler.h>

#include <fstream>
#include <sstream>
#include <map>
#include <string>

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

Model load_model_from_obj(const std::string& path) {
    std::ifstream file(path);  // Open the file

    if (!file.is_open()) {
        return {};
    }

    Model model;
    std::vector<DirectX::XMFLOAT3> temp_positions;
    std::vector<DirectX::XMFLOAT3> temp_normals;
    std::map<std::pair<unsigned int, unsigned int>, unsigned int> index_map;
    std::vector<unsigned int> temp_face_indices;

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);

        std::string prefix;
        ss >> prefix;

        if (prefix == "v") { //vertex position
            DirectX::XMFLOAT3 position;
            ss >> position.x >> position.y >> position.z;
            temp_positions.push_back(position);
        }
        else if (prefix == "vn") { //vertex normal
            DirectX::XMFLOAT3 normal;
            ss >> normal.x >> normal.y >> normal.z;
            temp_normals.push_back(normal);
        }
        else if (prefix == "f") {
            temp_face_indices.clear();

            std::string vertexindex_slashslash_normalindex;
            while (ss >> vertexindex_slashslash_normalindex) {
                size_t slash_pos = vertexindex_slashslash_normalindex.find("//");

                std::string vertex_index_str = vertexindex_slashslash_normalindex.substr(0, slash_pos);
                std::string normal_index_str = vertexindex_slashslash_normalindex.substr(slash_pos + 2);
                unsigned int vertex_index = std::stoi(vertex_index_str) - 1;
                unsigned int normal_index = std::stoi(normal_index_str) - 1;

                // Check if the vertex and normal index pair already exists in the map
                auto it = index_map.find({ vertex_index, normal_index });

                if (it != index_map.end()) {
                    temp_face_indices.push_back(it->second);
                }
                else {
                    // If it doesn't exist, create a new vertex and add it to the model
                    Vertex vertex;
                    vertex.position = temp_positions[vertex_index];
                    vertex.normal = temp_normals[normal_index];
                    unsigned int new_index = static_cast<unsigned int>(model.vertices.size());
                    model.vertices.push_back(vertex);
                    // Store the new index in the map
                    index_map[{ vertex_index, normal_index }] = new_index;
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
    file.close();  // Close the file
    return model;
}

typedef struct _SceneConstants {
	DirectX::XMMATRIX model; //4x4 flotantes = 64 bytes
	DirectX::XMMATRIX view; //4x4 flotantes = 64 bytes
	DirectX::XMMATRIX projection; //4x4 flotantes = 64 bytes

	//Parametros para la matriz de Vista (Look At)
	DirectX::XMVECTOR eye; //4 flotante = 16 bytes
	DirectX::XMVECTOR center; //4 flotante = 16 bytes
	DirectX::XMVECTOR up; //4 flotante = 16 bytes

	UINT triangle_angle = 0; //4 bytes

	float padding[3];
} SceneConstants;

class Application
{
private:
    //Cosas de application 
	void ThrowIfFailed(HRESULT hr, const std::string& msg);
	void ThrowIfFailed(HRESULT hr);
	void setupGeometry();
	void setupShaders();
	void SetupConstantBuffer();
	void setupDevice();
	void setupCommandQueue();
	void setupCommandAllocator();
	void setupCommandList();
	void setupSwapChain();
	void setupDescriptorHeap();
	void setupRenderTargetView();
	void setupSignature();
	void swapBuffers();
	void setBlendState(D3D12_BLEND_DESC& blend_desc);
	void setRasterizerState(D3D12_RASTERIZER_DESC& rasterizer_desc);
	void setDepthStencilState(D3D12_DEPTH_STENCIL_DESC& depth_stencil_desc);


	std::string readFile(const std::string& filename);

	// --- Configuración y Constantes ---
	static const UINT BUFFER_COUNT = 2; // Double buffering

	// --- Variables Globales de DX12 ---
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
	Microsoft::WRL::ComPtr<ID3D12Resource > depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource > constantBuffer;

	UINT frameIndex{ 0 };
	UINT rtvIncrementSize;
	void* mappedMemory;
	SceneConstants sceneConstants;

	unsigned int triangle_angle;

public:
	const int WINDOW_WIDTH = 1024;
	const int WINDOW_HEIGHT = 768;
	GLFWwindow* window;
	HWND GetWindowNativeHandler() const;
	void setup();
	void update();
	void draw();
	void keyCallback(int key, int scancode, int action, int mods);
};