#version 450 core
out vec4 FragColor;
in vec3 v_WorldPos; // 월드 공간에서의 픽셀 위치
in vec3 v_WorldNormal; // 월드 공간에서의 정규화된 노멀
// PBR 머티리얼 파라미터 (유니폼으로 받음)
uniform vec3 u_Albedo; // 표면의 기본 색상 (비금속의 경우 굴절된 빛의 색상)
uniform float u_Metallic; // 금속성 (0.0 = 비금속, 1.0 = 금속)
uniform float u_Roughness; // 거칠기 (0.0 = 거울처럼 부드러움, 1.0 = 매우 거침)
// 광원 정보 (유니폼으로 받음)
uniform vec3 u_LightPos; // 광원의 월드 공간 위치
uniform vec3 u_LightColor; // 광원의 색상 (강도 포함)
uniform vec3 u_CamPos; // 카메라의 월드 공간 위치

// 상수 (자주 사용되는 값)
const float PI = 3.14159265359;

// --- GGX D(NDF) - 정규 분포 함수 ---
// 마이크로패싯 노멀의 분포를 나타냄
float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a      = roughness * roughness;
    float a2     = a * a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float nom    = a2;
    float denom  = (NdotH2 * (a2 - 1.0) + 1.0);
    denom        = PI * denom * denom;
    return nom / denom;
}

// --- Geometry Smith(G) - 기하 함수 ---
// 마이크로패싯의 자체 그림자 및 가림 효과를 모델링
float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0; // PBR Guide: roughness is squared for better approximation

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

// --- Fresnel Schlick (F) - 프레넬 방정식 ---
// 표면의 각도에 따라 반사되는 빛의 양을 계산
vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main() {
    // 1. 필요한 벡터 계산
    vec3 N = normalize(v_WorldNormal); // 정규화된 노멀
    vec3 V = normalize(u_CamPos - v_WorldPos); // 시선 벡터 (뷰어에서 표면으로)
    vec3 L = normalize(u_LightPos - v_WorldPos); // 광원 벡터 (표면에서 광원으로)
    vec3 H = normalize(V + L); // 하프 벡터 (노멀과 빛/시선 벡터 사이의 중간)

    // 각 벡터와 노멀의 내적
    float NdotL = max(dot(N, L), 0.0); // 노멀과 광원 벡터의 내적 (0.0보다 작으면 뒤집혔으므로 클램프)
    float NdotV = max(dot(N, V), 0.0); // 노멀과 시선 벡터의 내적
    float HdotV = max(dot(H, V), 0.0); // 하프 벡터와 시선 벡터의 내적

    // 광원까지의 거리와 감쇠
    float dist        = length(u_LightPos - v_WorldPos);
    float attenuation = 1.0 / (dist * dist);
    vec3 radiance     = u_LightColor * attenuation; // 광원의 실제 방사량

    // 2. F0 계산 (반사율)
    // 비금속의 기본 F0는 일반적으로 0.04
    vec3 F0 = vec3(0.04);
    // 금속성(u_Metallic)에 따라 F0를 알베도(u_Albedo)로 선형 보간
    F0 = mix(F0, u_Albedo, u_Metallic);

    // 3. BRDF 계산
    // 3.1. D (정규 분포 함수)
    float D = DistributionGGX(N, H, u_Roughness);

    // 3.2. G (기하 함수)
    float G = GeometrySmith(N, V, L, u_Roughness);

    // 3.3. F (프레넬 방정식)
    vec3 F = FresnelSchlick(HdotV, F0);

    // 4. 반사(Specular) BRDF 계산
    vec3 numerator   = D * G * F;
    float denominator = 4.0 * NdotV * NdotL + 0.001; // 작은 값 더해서 0으로 나누는 것 방지
    vec3 specular    = numerator / denominator;

    // 5. 확산(Diffuse) BRDF 계산
    // 비금속일수록 확산 성분이 강함
    vec3 kS = F; // 반사된 빛의 양
    vec3 kD = vec3(1.0) - kS; // 확산된 빛의 양 = (1.0 - 반사된 빛의 양)
    // 금속은 굴절된 빛이 없으므로 확산 성분은 0.0이 됨
    kD *= (1.0 - u_Metallic);

    // 6. 최종 렌더링 방정식 (Lo) 계산
    vec3 Lo = (kD * u_Albedo / PI + specular) * radiance * NdotL;

    // 7. 결과 색상 출력 (여기에 주변광/IBL 추가)
    // 현재는 직접 조명(Direct Light)만 포함
    vec3 ambient = vec3(0.03) * u_Albedo * (1.0 - u_Metallic); // 매우 단순한 주변광
    vec3 finalColor = ambient + Lo;

    // HDR 대비 톤 매핑 (간단한 감마 보정)
    finalColor = finalColor / (finalColor + vec3(1.0)); // Filmic Tone Mapping의 단순화
    finalColor = pow(finalColor, vec3(1.0/2.2)); // 감마 보정
    FragColor = vec4(finalColor, 1.0);
}