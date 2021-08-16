#version 330 core
out vec4 FragColor;  

in vec3 FragPos;
in vec3 Normal;
//вычислять блик проще для мозга в world space, однако быстрее в view space. Для этого нужно преобразовать все векторы с view matrix(нормали тоже)
//собсна делаем это
in vec3 LightPosition;

uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 viewPos;

void main()
{
    float specularStrength = 0.5;
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;


    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPosition - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 viewDir = normalize(- FragPos);
    //reflect ожидает получить вектор направленный ОТ источника к фрагменту, у нас же это наоборот. Поэтому обратный lightDir
    vec3 reflectDir = reflect(-lightDir, norm);

    //степень - уровень блеска блика
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;
    vec3 result = (ambient + diffuse + specular) * objectColor;

    FragColor = vec4(result, 1.0);
}