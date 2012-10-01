#version 330


uniform sampler3D sTexture;
uniform float uLayer;


#ifdef _VERTEX_
out vec2 vsTexCoord;
#define oTexCoord vsTexCoord

void main() {
	oTexCoord = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2);
	gl_Position.xy = oTexCoord*2.0-1.0;
}
#endif



#ifdef _FRAGMENT_
in vec2 vsTexCoord;
#define iTexCoord vsTexCoord

layout(location=0) out vec4 oColour;

void main() {
	oColour = texture(sTexture, vec3(vsTexCoord, uLayer));
}
#endif

