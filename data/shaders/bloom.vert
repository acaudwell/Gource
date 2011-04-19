
varying vec3 pos;

void main()
{
    pos = gl_Vertex.xyz - gl_MultiTexCoord0.yzw;
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_FrontColor = gl_Color;
    gl_Position = ftransform();
}
