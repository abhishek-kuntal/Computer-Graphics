varying vec3 varyingPosition;
varying vec3 varyingNormal;

struct Light {
    vec3 lightPosition;
    vec3 lightColor;
    vec3 specularLightColor;
};
uniform Light lights[2];

varying vec2 varyingTexCoord;
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D normalTexture;
varying mat3 varyingTBNMatrix;
uniform vec3 useSpecular;
uniform vec3 uColor;

float attenuate(float dist, float a, float b) {
    return 1.0 / (1.0 + a*dist + b*dist*dist);
}

void main() {
    if(useSpecular[1] == 0.0)
    {
        vec3 diffuseColor = vec3(0.0, 0.0, 0.0);
        vec3 specularColor = vec3(0.0, 0.0, 0.0);
        vec3 textureNormal = normalize((texture2D(normalTexture, varyingTexCoord).xyz * 2.0) -1.0);
        textureNormal = normalize(varyingTBNMatrix * textureNormal);
        
        for(int i=0; i< 2; i++) {
            vec3 lightDirection = -normalize(varyingPosition-lights[i].lightPosition);
            float diffuse = max(0.0, dot(textureNormal, lightDirection));
            float attenuation = attenuate(distance(varyingPosition, lights[i].lightPosition) / 500.0, 2.5, 5.0);
            diffuseColor += (lights[i].lightColor * diffuse) * attenuation;
            vec3 v = normalize(-varyingPosition);
            vec3 h = normalize(v + lightDirection);
            float specular = pow(max(0.0, dot(h, textureNormal)), 64.0);
            specularColor += lights[i].specularLightColor * specular * attenuation;
        }
        vec3 intensity = (texture2D(diffuseTexture, varyingTexCoord).xyz * diffuseColor) + (texture2D(specularTexture, varyingTexCoord).xyz * specularColor);
        if(useSpecular[0] == 1.0)
        {
            intensity = diffuseColor + specularColor;
        }
        gl_FragColor = vec4(intensity.xyz, 1.0);
    }
    else
    {
        vec3 diffuseColor = vec3(0.0, 0.0, 0.0);
        vec3 specularColor = vec3(0.0, 0.0, 0.0);
        for(int i=0; i< 2; i++) {
            vec3 lightDirection = -normalize(varyingPosition-lights[i].lightPosition);
            float diffuse = max(0.0, dot(varyingNormal, lightDirection));
            float attenuation = attenuate(distance(varyingPosition, lights[i].lightPosition) / 500.0, 2.0, 5.0);
            diffuseColor += (lights[i].lightColor * diffuse) * attenuation;
            vec3 v = normalize(-varyingPosition);
            vec3 h = normalize(v + lightDirection);
            float specular = pow(max(0.0, dot(h, varyingNormal)), 64.0);
            specularColor += lights[i].specularLightColor * specular * attenuation;
        }
        vec3 intensity = (texture2D(diffuseTexture, varyingTexCoord).xyz * diffuseColor) + (texture2D(specularTexture, varyingTexCoord).x * specularColor);
        if(useSpecular[0] == 1.0)
        {
            intensity =  diffuseColor;
        }
        if(useSpecular[0] == 2.0)
        {
            intensity = diffuseColor + texture2D(specularTexture, varyingTexCoord).x * specularColor;
        }
        if(useSpecular[0] == 3.0)
        {
            intensity = (texture2D(diffuseTexture, varyingTexCoord).xyz * diffuseColor);
        }
        gl_FragColor = vec4(intensity.xyz, 1.0);
        if(useSpecular[0] == 4.0)
        {
            gl_FragColor = texture2D(diffuseTexture, varyingTexCoord);
        }
    }
    
}
