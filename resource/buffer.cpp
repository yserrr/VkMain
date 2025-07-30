#include<buffer.hpp> 


~Buffer::Buffer(){
    vkDeviceWaitIdle(device);
    if(stagingBuffer!= VK_NULL_HANDLE) vkDestroyBuffer(device, stagingBuffer, nullptr);
    if(buffer       != VK_NULL_HANDLE)vkDestroyBuffer(device, buffer,        nullptr);
    allocator.free(stagingAllocation, bufferSize);
    allocator.free(allocation,        bufferSize);
}