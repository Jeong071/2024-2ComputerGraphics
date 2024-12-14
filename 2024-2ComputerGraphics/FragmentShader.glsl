#version 330 core
out vec4 FragColor;

in vec3 FragPos;  
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 lightPos;      // 조명 위치
uniform vec3 lightColor;    // 조명 색상
uniform vec3 objectColor;   // 객체 색상
uniform vec3 viewPos;       // 카메라 위치
uniform bool lightOn;       
uniform float ambient;  //엠비언트 세기
uniform sampler2D texture1;
uniform bool useTexture;  // 텍스처 사용 여부를 결정하는 유니폼 변수 추가

void main() {
    vec4 texColor = texture(texture1, TexCoord);
    vec3 effectiveColor = useTexture ? texColor.rgb : objectColor;  // 텍스처 사용 여부에 따라 색상 결정

    // Ambient Lighting
    vec3 ambientLight = ambient * lightColor * effectiveColor;

    // Diffuse Lighting
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * effectiveColor;

    // Specular Lighting
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0),32);
    vec3 specular = spec * lightColor;

    // 조명 결과 계산
    vec3 result = ambientLight + (lightOn ? (diffuse + specular) : vec3(0.0));
    FragColor = vec4(result, useTexture ? texColor.a : 1.0);  // 텍스처의 알파 값을 적용하거나 기본값 1 사용
}