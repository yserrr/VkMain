#version 450

//| Descriptor Set | Binding Slot  |
//| -------------- | ------------- | ------------------------------------------------ 
//| `set = 0`      | `binding = 0` | Camera / Global Uniform (view, proj, time)
//| `set = 0`      | `binding = 1` | Lighting 정보 (optional)                           
//| `set = 1`      | `binding = 0` | Material 정보 (texture, sampler )
//| `set = 2`      | `binding = 1` | Material-specific UBO (roughness, etc.)
//| `set = 2`      | `binding = 0` | light (model matrix)

struct GPULight {
    vec4 position;       // 16 bytes
    vec4 direction;      // vec3 + padding (예: direction.xyz, 0.0f) - 16 bytes
    vec4 color;          // 16 bytes
    vec2 coneAngles;     // innerCone, outerCone packed into vec2 - 8 bytes
    vec2 padding;        // 패딩용 (8 bytes) -> 전체 16 bytes 맞춤
};
layout(location = 0) in  vec2 fragTexCoord;
layout(location = 0) out vec4 outColor;

layout(set = 1 , binding  = 0) uniform sampler2D bindlessTexture[];
layout(set = 1 , binding  = 1) uniform sampler2D myTexture;
layout(std140, set = 2 , binding  = 0 ) uniform LightBuffer {
    GPULight lights[16];
    int lightCount;
};

void main() {
    vec4 texColor = texture(myTexture, fragTexCoord);
    vec3 fragPos  = vec3(fragTexCoord, 0.0); // 임시 위치
    vec3 lightDir;
    float intensity;
    vec3 normal      = vec3(0.0, 0.0, 1.0);     // 임시 노멀

    GPULight light = lights[0];
    vec3 lightPos = vec3(light.position);
    lightDir = normalize(lightPos - fragPos);
    float distance = length(lightPos - fragPos);
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
    intensity = max(dot(normal, lightDir), 0.0) * attenuation;
    vec3 resultColor = light.color.rgb * light.color.a * intensity;
    if(resultColor ==vec3(0.0f, 0.0f,0.09) ){
      resultColor= vec3(0.5f);
    }
    outColor = vec4(texColor.rgb * resultColor, texColor.a);

    //for (int i = 0; i < lightCount; ++i) {
    //    GPULight light = lights[i];
    //    vec3  lightDir;
    //    float intensity = 0.0;
    //     if (light.position.w == 0.0) {
    //         lightDir =  normalize(vec3(light.position));
    //         intensity = max(dot(normal, -lightDir), 0.0);
    //     }
    //    if (light.position.w == 1.0) {
    //        lightDir = normalize(lightPos - fragPos);
    //        float distance = length(lightPos - fragPos);
    //        float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
    //        intensity = max(dot(normal, lightDir), 0.0) * attenuation;
    //    }
    //    resultColor += light.color.rgb * light.color.a * intensity;
    //}
    //vec4 texColor = texture(myTexture, fragTexCoord); // 💡 텍스처 샘플링
    //outColor = vec4(resultColor * texColor.rgb, texColor.a); // 💡 조명 * 텍스처 색
}


/*
#version 450
layout(location = 0) in  vec2 fragTexCoord; //2d texture
layout(location = 0) out vec4 outColor;


void main() {

    for (int i = 0; i < lightCount; ++i) {
        GPULight light = lights[i];
        vec3 lightDir;
        float intensity = 0.0;
        if (light.position.w == 0.0) {
            // Directional Light
            lightDir =  normalize(vec3(light.position)); // 방향
            intensity = max(dot(normal, -lightDir), 0.0);
        }
        else if (light.position.w == 1.0) {
            // Point Light
            vec3 lightPos = vec3(light.position);
            lightDir = normalize(lightPos - fragPos);
            float distance = length(lightPos - fragPos);
            float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
            intensity = max(dot(normal, lightDir), 0.0) * attenuation;
        }
        else if (light.position.w == 2.0) {
            // Spot Light
            vec3 lightPos = vec3(light.position);
            lightDir = normalize(lightPos - fragPos);
            float theta = dot(lightDir, normalize(-vec3(light.direction)));
            float cutoff = cos(radians(light.innerCone));
            float outerCutoff = cos(radians(light.outerCone));
            float epsilon = cutoff - outerCutoff;
            float spotFactor = clamp((theta - outerCutoff) / epsilon, 0.0, 1.0);
            float distance = length(lightPos - fragPos);
            float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
            intensity = max(dot(normal, lightDir), 0.0) * attenuation * spotFactor;
        }
        resultColor += light.color.rgb * intensity;
    }
    outColor = vec4(resultColor, 1.0);
}
*/