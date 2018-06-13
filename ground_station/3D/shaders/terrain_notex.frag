#version 120

varying vec3 color;

void main()
{
    vec3 clay = vec3(0.81, 0.77, 0.69);
    gl_FragColor = vec4(color.x * clay.x, color.y * clay.y, color.z * clay.z, 1.0);
}
