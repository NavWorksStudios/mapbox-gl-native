
varying vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D ssaoBlur;

struct Light {
    vec3 Position;
    vec3 Color;
    float Linear;
    float Quadratic;
};

uniform Light light;

void main()
{             
    // retrieve data from gbuffer
    vec3 FragPos = texture2D(gPosition, TexCoords).rgb;
    vec3 Normal = texture2D(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture2D(gAlbedo, TexCoords).rgb;
    float AmbientOcclusion = texture2D(ssaoBlur, TexCoords).r;
    
    // then calculate lighting as usual
    vec3 ambient = vec3(0.4 * Diffuse * AmbientOcclusion);
    vec3 viewDir  = normalize(-FragPos); // viewpos is (0.0.0)

    // diffuse
    vec3 lightDir = normalize(light.Position - FragPos);
    vec3 diffuse = dot(Normal, lightDir) * Diffuse * light.Color;

    // specular
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), 8.0);
    vec3 specular = light.Color * spec;

    // attenuation
    float distance = length(light.Position - FragPos);
    float attenuation = 1.0 / (1.0 + light.Linear * distance + light.Quadratic * distance * distance);
    diffuse *= attenuation;
    specular *= attenuation;

    vec3 lighting = ambient + diffuse * 0.4 + specular * 0.1;
    gl_FragColor = vec4(lighting, 1.0);


    //gl_FragColor = texture2D(ssaoBlur, TexCoords);
}