#version 330 core

out vec4 FragColor;  

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
//вычислять блик проще для мозга в world space, однако можно и в view space. Для этого нужно преобразовать все векторы с view matrix(нормали тоже)


//ambient - свет всегда чуть чуть присутсвует - это цвет объекта в полном отсуствии источников освещения
//diffuse - цвет объекта должен становится ярче при близости к иточнику и темнее при дальности
//вычисления diffuse и ambient обычно проиходят над одним значением - цветом объекта
//specular - блики от источника на объекте, для симуляции разнообразия бликовых поверхностей используется мапа
//emission - источник света на самом объекте, тоже мапа
struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

//освещение подобное солнцу
struct DirLight
{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

//просто светящийся объект с затуханием
struct PointLight
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    //затухание света
    float constant;
    float linear;
    float quadratic;
    
};

//свет-проектор
struct SpotLight
{
    //направление освещения-конуса
    vec3 direction;
    //угол после которого конус не касается объектов
    float cutOff;
    float outerCutOff;

    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    //затухание света
    float constant;
    float linear;
    float quadratic;
    
};

#define NR_POINT_LIGHTS 4

uniform Material material;
uniform SpotLight spotLight;
uniform DirLight directionalLight;
uniform PointLight pointLight[NR_POINT_LIGHTS];

uniform vec3 viewPos;

uniform float u_time;

//func prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewPos);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewPos, vec3 fragPos);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 viewPos, vec3 fragPos);

//TODO::большой потенциал оптимизации, много лишних действий и остаточных переменных от эксперементов всяких
void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = vec3(0.0);
    result += CalcDirLight(directionalLight, norm, viewDir);
    for (int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLight[i], norm, viewDir, FragPos);
    result += CalcSpotLight(spotLight, norm, viewDir, FragPos);
    //"светящиеся" части на объекте
    //vec3 emissionMap = texture(material.emission, TexCoord + vec2(0.0, u_time * 0.5)).rgb;
    //vec3 emissionMask = step(vec3(1.0f), vec3(1.0f) - specularMap);
    //vec3 emission =  emissionMap * emissionMask;
    result = clamp(result, vec3(0.0), vec3(1.0));
    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{

    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0f);
    //reflect ожидает получить вектор направленный ОТ источника к фрагменту, у нас же это наоборот. Поэтому обратный lightDir
    vec3 reflectDir = reflect(-lightDir, normal); 
    //степень - уровень блеска блика
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 specularMap = texture(material.specular, TexCoord).rgb;
    vec3 diffuseMap = texture(material.diffuse, TexCoord).rgb;
    vec3 ambientMap = texture(material.diffuse, TexCoord).rgb;

    vec3 diffuse =  light.diffuse * diff * diffuseMap;
    vec3 ambient = light.ambient * ambientMap;
    vec3 specular = light.specular * spec * specularMap;

    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir, vec3 fragPos)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 reflectDir = reflect(-lightDir, normal); 
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 specularMap = texture(material.specular, TexCoord).rgb;
    vec3 diffuseMap = texture(material.diffuse, TexCoord).rgb;
    vec3 ambientMap = texture(material.diffuse, TexCoord).rgb;
    
    float distance = length(light.position - fragPos);
    float attenuation = 1.0/(light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 diffuse =  light.diffuse * diff * diffuseMap * attenuation;
    vec3 ambient = light.ambient * ambientMap * attenuation;
    vec3 specular = light.specular * spec * specularMap * attenuation;

    return (ambient + diffuse + specular); 
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 viewDir, vec3 fragPos)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 reflectDir = reflect(-lightDir, normal); 
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 specularMap = texture(material.specular, TexCoord).rgb;
    vec3 diffuseMap = texture(material.diffuse, TexCoord).rgb;
    vec3 ambientMap = texture(material.diffuse, TexCoord).rgb;
    
    float distance = length(light.position - fragPos);
    float attenuation = 1.0/(light.constant + light.linear * distance + light.quadratic * (distance * distance));

    float theta = dot(lightDir, normalize(-light.direction));
    //мягкая интерполяция конуса-спотлайта, свет размывается на краях
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff)/ epsilon, 0.0, 1.0);

    vec3 diffuse =  light.diffuse * diff * diffuseMap * attenuation * intensity;
    vec3 ambient = light.ambient * ambientMap * attenuation * intensity;
    vec3 specular = light.specular * spec * specularMap * attenuation * intensity;

    return (ambient + diffuse + specular); 
}