#version 330 core
out vec4 FragColor;

in vec2 texCoord;
in vec3 Normal;
in vec3 fragPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

struct PointLight {
    vec3 position;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float constant;
    float linear;
    float quadratic;
};
uniform PointLight pointLight;
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

struct FlashLight {
    vec3 position;
    vec3 direction;
    float innerCutOffAngle; //stored as cosine of the angle (for performance)
    float outerCutOffAngle;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float constant;
    float linear;
    float quadratic;
};
uniform FlashLight flashlight;
vec3 CalcFlashLight(FlashLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

uniform vec3 cameraPos;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(cameraPos - fragPos);
    vec3 result = CalcPointLight(pointLight, norm, fragPos, viewDir) + CalcFlashLight(flashlight, norm, fragPos, viewDir); 
    FragColor = vec4(result,1.0);
}


vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 2.0);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // combine results
    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1,texCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1,texCoord));
    vec3 specular = light.specular * spec * vec3(texture(texture_specular1,texCoord));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 CalcFlashLight(FlashLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float theta = dot(lightDir, normalize(-light.direction));
    if(theta > light.outerCutOffAngle)
    {
        //ambient lighting
        vec3 ambient = light.ambient * vec3(texture(texture_diffuse1,texCoord).rgb);
        //diffuse lighting
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1,texCoord).rgb);
        //specular lighting
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
        vec3 specular = light.specular * spec * vec3(texture(texture_specular1,texCoord).rgb);
        //distance attenuation
        float distance = length(light.position - fragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
        //spotlight cone attenuation
        float epsilon = (light.innerCutOffAngle  - light.outerCutOffAngle);
        float intensity = clamp((theta - light.outerCutOffAngle) / epsilon, 0.0, 1.0);

        ambient *= intensity;
        diffuse = diffuse * attenuation * intensity;
        specular = diffuse * attenuation * intensity;
        return (ambient + diffuse + specular);
    }
    else
    {
        return light.ambient * vec3(texture(texture_diffuse1, texCoord));
    }
}