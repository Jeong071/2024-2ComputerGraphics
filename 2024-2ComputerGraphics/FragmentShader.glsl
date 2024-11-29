#version 330 core
out vec4 FragColor;

in vec3 FragPos;  
in vec3 Normal;

uniform vec3 lightPos;      // ���� ��ġ
uniform vec3 lightColor;    // ���� ����
uniform vec3 objectColor;   // ��ü ����
uniform vec3 viewPos;       // ī�޶� ��ġ

void main() {
    // Ambient Lighting
    vec3 ambientLight = 0.3 * lightColor;

    // Diffuse Lighting
    vec3 lightDir = normalize(lightPos - FragPos); // ���� ����
    vec3 norm = normalize(Normal);                // ����ȭ�� ���� ����
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular Lighting
    vec3 viewDir = normalize(viewPos - FragPos);  // ī�޶� ����
    vec3 reflectDir = reflect(-lightDir, norm);   // �ݻ� ����
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = spec * lightColor;

    // ���� ��� ���
    vec3 result = ambientLight + diffuse + specular;
    result *= objectColor;

    FragColor = vec4(result, 1.0);
}
