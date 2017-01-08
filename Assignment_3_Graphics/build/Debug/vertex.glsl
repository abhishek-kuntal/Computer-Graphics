/*attribute vec4 position;
attribute vec4 normal;

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

uniform mat4 normalMatrix;
varying vec3 varyingNormal;

void main() {
    varyingNormal = normalize((normalMatrix * normal).xyz);
    gl_Position = projectionMatrix * modelViewMatrix * position;
}
*/
attribute vec4 position;

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 normalMatrix;

varying vec3 varyingNormal;
varying vec3 varyingPosition;

attribute vec2 texCoord;
varying vec2 varyingTexCoord;

attribute vec4 normal;
attribute vec4 binormal;
attribute vec4 tangent;

varying mat3 varyingTBNMatrix;

void main() {
    varyingNormal = normalize((normalMatrix * normal).xyz);
    vec4 p = modelViewMatrix * position;
    varyingPosition = p.xyz;
    varyingTexCoord = texCoord;
    varyingTBNMatrix = mat3(normalize((normalMatrix * tangent).xyz), normalize((normalMatrix *binormal).xyz), normalize((normalMatrix * normal).xyz));
    gl_Position = projectionMatrix * modelViewMatrix * position;
}
