uniform sampler2D tex;
uniform float shadow_strength;

void main(void)
{
    vec4 colour = texture2D(tex,gl_TexCoord[0].st);

    gl_FragColor = vec4(0.0, 0.0, 0.0, gl_Color.w * colour.w * shadow_strength);
}
