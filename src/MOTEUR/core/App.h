//#pragma comment(lib, "d3d12.lib")
//#pragma comment(lib, "dxgi.lib")
//#pragma comment(lib, "D3DCompiler.lib")
//#include "Camera.h"
//#include "Config.h"
//
//using namespace DirectX;
//using namespace Microsoft::WRL;
//
//class App
//{
//public:
//    struct Vertex {
//        XMFLOAT3 position;
//        XMFLOAT4 color;
//    };
//    struct ConstantBuffer {
//        XMFLOAT4X4 world;
//        XMFLOAT4X4 view;
//        XMFLOAT4X4 projection;
//    };
//    enum class ShapeType {
//        Pyramid,
//        Cube,
//        Sphere,
//        Triangle
//    };
//
//protected:
//    float frameTime = 0.0f;  // Temps écoulé entre les frames
//    float fps = 0.0f;        // Nombre d'images par seconde
//    std::chrono::steady_clock::time_point lastFrameTime;  // Temps de la dernière frame
//    float timeSinceLastTitleUpdate = 0.0f; // Temps depuis la dernière mise à jour du titre
//    std::chrono::steady_clock::time_point lastTitleUpdateTime;
//
//    Camera m_camera;
//    bool usePostProcessing = false;
//
//
//    ShapeType currentShape = ShapeType::Pyramid;
//
//    int windowWidth = 1280;
//    int windowHeight = 720;
//    std::wstring configPath = L"../config.ini";
//    std::wstring iconPath;
//    std::wstring shaderPath;
//    std::wstring postProcessPath;
//
//    HWND hwnd;
//   
//    //Geometry
//    ComPtr<ID3D12Resource> vertexBuffer;
//    ComPtr<ID3D12Resource> indexBuffer;
//    ComPtr<ID3D12DescriptorHeap> dsvHeap;
//    static constexpr DXGI_FORMAT DepthBufferFormat = DXGI_FORMAT_D32_FLOAT;
//
//
//    int m_indexCount = 18;
//
//public:
//    App() {
//        iconPath = GetConfigValue(L"Paths", L"Icon", L"icon.ico", configPath);
//        shaderPath = GetConfigValue(L"Paths", L"Shader", L"shaders.hlsl", configPath);
//        postProcessPath = GetConfigValue(L"Paths", L"PostProcessing", L"postprocess.hlsl", configPath);
//    }
//    ~App();
//
//    void InitWindow(HINSTANCE hInstance, int nCmdShow);
//    void Run();
//
//    void SetShape(ShapeType shape);
//    void LoadGeometry(ShapeType shape);
//    void LoadPyramidGeometry();
//    void LoadCubeGeometry();
//    void LoadSphereGeometry();
//    void LoadTriangleGeometry();
//    void LoadMeshGeometry(Vertex* vertices, UINT vertexCount, UINT16* indices, UINT indexCount,
//        ComPtr<ID3D12Resource>& outVertexBuffer, D3D12_VERTEX_BUFFER_VIEW& outVertexBufferView,
//        ComPtr<ID3D12Resource>& outIndexBuffer, D3D12_INDEX_BUFFER_VIEW& outIndexBufferView);
//
//    void UpdateFPS();
//    void UpdateWindowTitle();
//
//    friend class Renderer;
//};
