#version 330 

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;

uniform mat4 modelview;
uniform mat4 modelview_inv;
uniform mat4 projection;
uniform vec4 light_pos;

varying float light;

void main()
{
	const vec4 position_world = vec4(in_position, 1.0f);
	const vec4 position_cam = modelview * position_world;
    gl_Position = projection * position_cam;
	
	const vec3 normal_cam = vec3(modelview_inv * vec4(in_normal, 1.0f));
	const vec3 light_dir = light_pos.xyz - position_cam.xyz;

	light = dot(normal_cam, normalize(light_dir));
}
	