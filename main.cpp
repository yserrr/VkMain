#include "engine.hpp"

int main(){
    const char* frag = "/home/ljh/CLionProjects/VkMain/shader/fragment.spv";
    const char* vert = "/home/ljh/CLionProjects/VkMain/shader/vertex.spv";
    Engine engine;
    engine.initialize(); 
    engine.mainLoop();
}