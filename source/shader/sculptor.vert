#version 450

layout(location = 0) in vec3 inPos;
layout(std430, binding = 0) buffer VertexDelta {
    vec3 vertex[];
    vec3 delta[];
};

void main() {
    uint idx = gl_VertexIndex;       // 현재 vertex index
    vec3 pos = inPos + delta[idx];   // SSBO에서 변형값 읽어 적용
    gl_Position = projection * view * vec4(pos, 1.0);
}
