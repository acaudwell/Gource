
varying vec3 pos;

void main()
{
    float intensity = min(1.0, cos(length(pos*2.0)/gl_TexCoord[0].x));
    float gradient = intensity * smoothstep(0.0, 2.0, intensity);

    gl_FragColor = gl_Color * gradient;
}
