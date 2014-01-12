#version 330 

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;

uniform mat4 modelview;
uniform mat4 modelview_inv;
uniform mat4 projection;
uniform vec4 light_pos;

out vec3 normal_cam;
out vec3 position_cam;
out vec3 light_dir;
//out float light;

void main()
{
	vec4 position_world = vec4(in_position, 1.0f);
	vec4 position_cam4 = modelview * position_world;
    gl_Position = projection * position_cam4;

	position_cam = vec3(position_cam4);
	 
	normal_cam = vec3(modelview_inv * vec4(in_normal, 1.0f));
	light_dir = vec3(light_pos) - position_cam;
	light_dir = normalize(light_dir);
}
	