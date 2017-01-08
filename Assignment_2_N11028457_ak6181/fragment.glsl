varying vec4 varyingNormal;
uniform vec3 uColor;

void main() {

    float diffuse = max(0.0, dot(varyingNormal, vec4(-0.5773, 0.5773, 0.5773, 0.0)));
    float diffuse1 = max(0.0, dot(varyingNormal, vec4(0.5773, -0.5773, 0.5773, 0.0)));
    vec3 intensity = uColor * (diffuse + diffuse1) ;
    gl_FragColor = vec4(intensity.xyz, 1.0);

}
