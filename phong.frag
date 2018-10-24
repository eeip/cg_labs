#version 330

uniform vec3 light_position;
uniform vec3 camera_position;
uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;
uniform float shininess;

in VS_OUT {
	vec3 vertex;
	vec3 normal;
} fs_in;

out vec4 frag_color;


void main()
{
	vec3 L = normalize(light_position - fs_in.vertex);
	vec3 V = normalize(camera_position - fs_in.vertex);
	frag_color = vec4(ambient + diffuse * max(0.0, dot(fs_in.normal, L)) + specular * pow(max(0.0, dot(reflect(-L,fs_in.normal),V)),shininess),1.0);
}
