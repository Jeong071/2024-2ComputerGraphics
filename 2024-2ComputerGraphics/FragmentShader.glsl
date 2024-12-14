#version 330 core
out vec4 FragColor;

in vec3 FragPos;  
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 lightPos;      // ���� ��ġ
uniform vec3 lightColor;    // ���� ����
uniform vec3 objectColor;   // ��ü ����
uniform vec3 viewPos;       // ī�޶� ��ġ
uniform bool lightOn;       
uniform float ambient;  //�����Ʈ ����
uniform sampler2D texture1;
uniform bool useTexture;  // �ؽ�ó ��� ���θ� �����ϴ� ������ ���� �߰�

void main() {
    vec4 texColor = texture(texture1, TexCoord);
    vec3 effectiveColor = useTexture ? texColor.rgb : objectColor;  // �ؽ�ó ��� ���ο� ���� ���� ����

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

    // ���� ��� ���
    vec3 result = ambientLight + (lightOn ? (diffuse + specular) : vec3(0.0));
    FragColor = vec4(result, useTexture ? texColor.a : 1.0);  // �ؽ�ó�� ���� ���� �����ϰų� �⺻�� 1 ���
}