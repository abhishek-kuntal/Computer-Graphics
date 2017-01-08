uniform sampler2D screenFramebuffer;
varying vec2 texCoordVar;
void main() {
  /*  float conversionFactor = 255 / (16 - 1);
   Gray = (Red * 0.2126 + Green * 0.7152 + Blue * 0.0722)
   
    vec4 texColor = texture2D(screenFramebuffer, texCoordVar);
    //float brightness = (texColor.x*0.2126 + texColor.y*0.7152 +texColor.z*0.0722);
     float brightness = (texColor.x+texColor.y+texColor.z)/3.0;
    gl_FragColor = vec4(brightness, brightness, brightness, 1.0);
   */
    gl_FragColor = vec4(1.0-texture2D(screenFramebuffer, texCoordVar).xyz, 1.0);
}

