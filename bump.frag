#version 330

uniform vec3 light_position;
uniform vec3 camera_position;
uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;
uniform float shininess;
uniform mat4 normal_model_to_world;
uniform mat4 vertex_to_world;
uniform mat4 world_to_clip;
uniform sampler2D opacity_texture;
vec3 n;
vec3 temp;
uniform sampler2D diffuse_texture;

in VS_OUT {
	vec3 vertex;
	vec3 normal;
	vec2 texcoord;
	vec3 tangent;
	vec3 binormal;
} fs_in;

out vec4 frag_color;


void main()
{
	n = texture(opacity_texture, fs_in.texcoord).rgb;
	n = normalize(n * 2 - 1);
	mat3 TBN = mat3(fs_in.tangent,fs_in.binormal,fs_in.normal);
	n = normalize(TBN*n);
	vec3 normal = vec3(normal_model_to_world * vec4(n,0));
	frag_color = texture(diffuse_texture, fs_in.texcoord);;
}
