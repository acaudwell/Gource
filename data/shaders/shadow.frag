uniform sampler2D tex;

void main(void)
{
    vec4 colour = texture2D(tex,gl_TexCoord[0].st);

    gl_FragColor = vec4(0.0, 0.0, 0.0, gl_Color.w * colour.w * 0.5);
}
