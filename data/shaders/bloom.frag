
varying vec3 pos;

void main()
{
    float intensity = min(1.0, cos(length(pos*2.0)/gl_TexCoord[0].x));
    float gradient = intensity * smoothstep(0.0, 2.0, intensity);

    gradient *= smoothstep(1.0,0.33333+fract(sin(dot(pos.xy ,vec2(11.3713,67.3219))) * 2351.3718)*0.66667, 1.0-intensity);

    gl_FragColor = gl_Color * gradient;
}
