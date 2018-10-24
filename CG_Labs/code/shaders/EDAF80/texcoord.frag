#version 410

in VS_OUT {
	vec2 texcoord;
} fs_in;

out vec4 frag_color;

uniform sampler2D diffuse_texture;

void main()
{
	frag_color = texture(diffuse_texture, fs_in.texcoord);
}
