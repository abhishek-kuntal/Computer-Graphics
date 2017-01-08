uniform sampler2D screenFramebuffer;
varying vec2 texCoordVar;
void main() {
  //  gl_FragColor = texture2D( screenFramebuffer, texCoordVar);
    //gl_FragColor = vec4(1.0-texture2D(screenFramebuffer, texCoordVar).xyz, 1.0);
    vec4 texColor = texture2D(screenFramebuffer, texCoordVar);
    //float brightness = (texColor.x*0.2126 + texColor.y*0.7152 +texColor.z*0.0722);
    float brightness = (texColor.x+texColor.y+texColor.z)/3.0;
    gl_FragColor = vec4(brightness, brightness, brightness, 1.0);
}

/*

uniform sampler2D screenFramebuffer;
varying vec2 texCoordVar;
void main() {
    const float gamma = 2.2;
    vec3 hdrColor = texture2D( screenFramebuffer, texCoordVar).rgb;
    vec3 mapped = hdrColor / (hdrColor + vec3(1.0));
    mapped = pow(mapped, vec3(gamma));
    gl_FragColor = vec4(mapped, 1.0);
}*/
