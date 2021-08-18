#version 330 core

out vec4 FragColor;  

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
//вычислять блик проще для мозга в world space, однако быстрее в view space. Для этого нужно преобразовать все векторы с view matrix(нормали тоже)

struct Material
{
    //цвет каждой составляющей 
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;

uniform vec3 viewPos;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    vec3 ambient = material.ambient * light.ambient;


    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse =  light.diffuse * (material.diffuse*diff);

    vec3 viewDir = normalize(viewPos - FragPos);
    //reflect ожидает получить вектор направленный ОТ источника к фрагменту, у нас же это наоборот. Поэтому обратный lightDir
    vec3 reflectDir = reflect(-lightDir, norm);

    //степень - уровень блеска блика
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = material.specular * spec * light.specular;

    vec3 result = (ambient + diffuse + specular);

    FragColor = vec4(result, 1.0) * mix(texture(texture1,TexCoord), texture(texture2, TexCoord), 0.6);
}