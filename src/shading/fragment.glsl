#version 330

in vec3 normal_cam;
in vec3 position_cam;
in vec3 light_dir;

uniform float phong_power;

out vec4 outputColor;


void main()
{
	float light = dot(normal_cam, light_dir);
	light = clamp(light, 0, 1);

	vec3 refl = reflect(-light_dir, normal_cam);
	float phong = dot(-normalize(position_cam), normalize(refl));
	phong = clamp(phong, 0, 1);
	phong = light > 0.00001 ? phong : 0.0;
	phong = pow(phong, phong_power);

	light = light + phong;

	outputColor = vec4(light, 0, 0, 1.0f);
}
