#version 330 core
out vec4 FragColor;

in vec3 FragPos;  
in vec3 Normal;

uniform vec3 lightPos;      // 조명 위치
uniform vec3 lightColor;    // 조명 색상
uniform vec3 objectColor;   // 객체 색상
uniform vec3 viewPos;       // 카메라 위치

void main() {
    // Ambient Lighting
    vec3 ambientLight = 0.3 * lightColor;

    // Diffuse Lighting
    vec3 lightDir = normalize(lightPos - FragPos); // 조명 방향
    vec3 norm = normalize(Normal);                // 정규화된 법선 벡터
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular Lighting
    vec3 viewDir = normalize(viewPos - FragPos);  // 카메라 방향
    vec3 reflectDir = reflect(-lightDir, norm);   // 반사 벡터
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = spec * lightColor;

    // 조명 결과 계산
    vec3 result = ambientLight + diffuse + specular;
    result *= objectColor;

    FragColor = vec4(result, 1.0);
}
