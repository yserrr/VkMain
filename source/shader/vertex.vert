
#version 450
//| `set = 0`      | `binding = 0` | Camera / Global Uniform
//| `set = 0`      | `binding = 1` | Lighting
//| `set = 1`      | `binding = 0` | texture bindless
//| `set = 1`      | `binding = 1` | material options
//| `set = 2`      | `binding = 0` | light
//| `set = 3+`     | -             | Compute 전용이나 special-case 처리 등                   |
layout(location = 0) in  vec3 inPosition;
layout(location = 1) in  vec3 inNormal;
layout(location = 2) in  vec2 inUV;
layout(location = 3) in  vec3 inTangent;
layout(location = 4) in  vec3 inBitangent;
layout(location = 5) in  vec4 inColor;
layout(location = 6) in ivec4 inBoneIndices;
layout(location = 7) in  vec4 inBoneWeights;

layout(location =0) out vec2 fragTexCoord;
//set 0 binding 0 -> uniform buffer 
layout(set = 0, binding = 0 ) uniform cameraUBD{
    mat4 view; 
    mat4 proj;
} camera;

layout(push_constant) uniform PushConstants {
    mat4 model;
    vec4 color;
    uint bindlessIndex;
} pc;

void main() {
    // 월드 → 뷰 → 투영 변환을 거쳐 gl_Position에 할당
    gl_Position = camera.proj* camera.view*vec4(inPosition, 1.0);
    fragTexCoord = inUV;
}
