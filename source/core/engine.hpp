
/*
의존성 분리 

//버퍼 클래스 만들어서 유지보수하기
텍스쳐  
버텍스 
라이트
버퍼 및 디스크립터 -> 추상클래스 분리하고 관리하기 
그리고 이를 바탕으로 메모리 할당 하도록 설정하기 
virtual class
buffer
sampler  
desriptor set
texture 
shader pass
최우선 :: 일단camera interaction쪽은 거의 완성해둬야할듯
그래야 렌더링 실험 과정을 이해할 수 있고
다른 texture들을 bind할 수 있음 필수임


project: 
engine for modeling: 
modeling multi view program:: 
       -- simple rendering program 


texture ->materail model for PBR 
        ::displace ment 
        ::nomal Map
        ::light BAKE 
rendering module ::  
renderpass  
pipeline 
descriptorPool  
setting 
rigging structure 
zbrush -- z buffer 
       -- displacement for subdivision 
       -- vertex modleing 
rendering -- anold cpu base rendering
          -- mamoset realtime Engine 
FXSimulation :: houdini 
                node base programming method 
end to end:: rendering post processing DLSS 
             AI postProcessing 
🔹 Material 구조 도입 전 임시 정리
    DescriptorSetBinding 표준화
    → 각 shader resource를 어떻게 binding하고 있는지 명확하게 구조화
    예:
        binding 0: camera UBO
        binding 1: object transform UBO
        binding 2: texture sampler
        binding 3: light UBO
GLSL Uniform 구조 정리
layout(set = 0, binding = x) 기준으로 shader 간 일관성 유지
Scene 단위 관리 구조
현재는 object마다 descriptorset을 bind하고 있을 가능성 있음 → scene 단위에서 관리되도록 확장 고려
Material 클래스 설계
텍스처 + 라이트맵 + 셰이더 파라미터 관리
셰이더에 필요한 유니폼 값 바인딩
다양한 재질 속성 관리 (반사, 투명도 등)
Object 클래스 생성
Mesh + Material 묶음
월드 변환 행렬 포함
렌더링 시 파이프라인에 바인딩 및 드로우 호출 담당
렌더링 파이프라인 구축
카메라, 조명, 오브젝트 순서로 렌더링
뎁스 테스트, 블렌딩 설정 관리
프레임 단위 렌더링 루프 구현
카메라 클래스 기본 구조
주요 역할
뷰 행렬(view matrix) 계산 (카메라 위치, 방향, 업벡터)
투영 행렬(projection matrix) 계산 (원근 또는 직교 투영)
카메라 위치, 방향, 이동 및 회전 기능 제공
interaction :
keyboard 
mouse
프론트로 가메라 move 가능하도록 해야함 

get
set 
Call
*/
#ifndef ENGINE_HPP
#define ENGINE_HPP 

#include  <common.hpp>
#include  <ImporterEx.hpp>
#include  <instance.hpp>
#include  <logicalDevice.hpp>
#include  <physicalDevice.hpp>
#include  <surface.hpp>
#include  <window.hpp>
#include  <mesh.hpp>
#include  <sceneRenderer.hpp>
#include  <ui_renderer.hpp>
#include  <event_manager.hpp>
#include  <assetManager.hpp> 
#include  <interface.hpp>
class engine{
public: 
 engine()= default;
~engine();
void initialize();
void initUI();
void mainLoop();   
void run(const char* vert, const char* frag);

private:
//init context 
void initVulkan();
void initFrame ();
void initRender();
void initAsset ();
void  createSwapchain();
void  createImageManager();
void  createRenderPass();
void  createFrameBuffer();
void  createCommandBuffer();
void  createSignals();
void  createDescriptors();
//draw context
void  setUp();
void  draw ();
VkCommandBuffer rec(uint32_t imageIndex); 
void  summitQueue(VkCommandBuffer command, uint32_t imageIndex);

#ifdef NDEBUG 
const bool enableValidationLayers =false; 
#else 
const bool enableValidationLayers = true;
#endif 
//handler
GLFWwindow*      window_h;
VkInstance       instance_h;
VkSurfaceKHR     surface_h;
VkPhysicalDevice physical_device_h;  
VkDevice         device_h;
VkSwapchainKHR   swapchain_h;
VkFormat         imageformat_h;
VkRenderPass     renderpass_h; 
VkCommandPool    command_pool_h;
VkQueue          present_q; 
VkQueue          graphics_q;
VkPipelineLayout pipeline_layout_h;
//queue index : 
uint32_t         graphics_family; 
uint32_t         present_family; 
double           lastFrameTime;
//signals for synchonize and context
uint32_t           currentFrame = 0; 
uint32_t           MAX_FRAMES_IN_FLIGHT = 2;
uint32_t           frameCount;
uint64_t           timeout; //similar with float
uint32_t           imageIndex;   
//renderer
// 가장 낮은 의존성 (가장 먼저 생성, 가장 늦게 파괴되어야 하는 것)
std::unique_ptr<Window>               window;
std::unique_ptr<Instance>             instance;
std::unique_ptr<Surface>              surface;
std::unique_ptr<PhysicalDevice>       physicalDevice;
std::unique_ptr<LogicalDevice>        device; 
std::unique_ptr<Swapchain>            swapchain; 
std::unique_ptr<ImageViewManager>     imageManager;
std::unique_ptr<RenderPass>           renderPass;
std::unique_ptr<FramebufferManager>   frameBufferManager;
std::unique_ptr<CommandPool>          commandPool;
std::unique_ptr<CommandBufferManager> commandBufferManager; 
std::unique_ptr<DescriptorManager>    descriptorManager;

// Render / Asset / UI / Interaction (device가 생성된 후 의존하는 객체)
std::unique_ptr<MemoryAllocator>      allocator;
std::unique_ptr<AssetManager>         asset;
std::unique_ptr<EventManager>          interaction;
std::unique_ptr<UIRenderer>           uiRenderer;
std::unique_ptr<SceneRenderer>        sceneRenderer;

// 동기화 객체 (device 이후 생성)
std::unique_ptr<SemaphoreManager>     imageAvailableSemaphores;
std::unique_ptr<SemaphoreManager>     renderFinishedSemaphores;
std::unique_ptr<FenceManager>         inFlightFences;

// 기타
std::vector  <VkRenderPassBeginInfo>  renderPassInfos;
std::array   <VkClearValue,2>         clearValues;
std::array<VkDescriptorSet,3>         sets;

VkExtent2D extent= {1200,800};
const char* title =  "vulkan";
const char* vert; 
const char* frag;

}; 

#endif //engine_hpp