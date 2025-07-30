#include "Engine.hpp"

int main(){
    const char* frag = "/home/ljh/vk/build/shader/fragment.spv"; 
    const char* vert = "/home/ljh/vk/build/shader/vertex.spv"; 
    Engine engine(vert,frag);
    engine.initialize(); 
    engine.mainLoop();
}