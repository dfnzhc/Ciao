#version 460

out vec4 FragColor;

void main()
{
    // gl_FragDepth = gl_FragCoord.z;
    FragColor = vec4(gl_FragCoord.z, gl_FragCoord.z * gl_FragCoord.z, 0.0, 1.0);
    //FragColor = vec4(0.0, gl_FragCoord.z * gl_FragCoord.z, 0.0, 1.0);
}