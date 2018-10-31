#version 330

uniform float player_radius;

in VS_OUT {
	float radius;
} fs_in;


out vec4 frag_color;

void main()
{
	if (fs_in.radius >= player_radius) {
		frag_color = vec4(1,0,0,1);
	} else {
		frag_color = vec4(0,1,0,1);
	}
}
