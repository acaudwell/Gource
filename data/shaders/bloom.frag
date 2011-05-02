
varying vec3 pos;

void main()
{
    float r = fract(sin(dot(pos.xy ,vec2(11.3713,67.3219))) * 2351.3718);

    float offset = (0.5 - r) * gl_TexCoord[0].x * 0.045;

    float intensity = min(1.0, cos((length(pos*2.0)+offset)/gl_TexCoord[0].x));
    float gradient  = intensity * smoothstep(0.0, 2.0, intensity);

    gradient *= smoothstep(1.0,0.67+r*0.33, 1.0-intensity);

    gl_FragColor = gl_Color * gradient;
}
