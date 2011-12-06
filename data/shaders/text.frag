uniform sampler2D tex;
uniform float shadow_strength;
uniform float texel_size;

void main(void)
{
    float colour_alpha = texture2D(tex,gl_TexCoord[0].xy).w;
    float shadow_alpha = texture2D(tex,gl_TexCoord[0].xy - vec2(texel_size)).w * shadow_strength;

    float combined_alpha = 1.0 - (1.0-shadow_alpha)*(1.0-colour_alpha);

    if(combined_alpha > 0.0) colour_alpha /= combined_alpha;

    gl_FragColor = gl_Color * vec4(vec3(colour_alpha), combined_alpha);
}
