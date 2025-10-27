#version 330 core

layout (location = 0) in vec3 a_Pos;
layout (location = 1) in vec3 a_Normal;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

uniform float u_SpeedSq;

out vec3 pos;
out vec4 finalColor;

void main()
{
	gl_Position = u_Projection * u_View * u_Model * vec4(a_Pos, 1.0);

	// pass the vertex position in local space for circle rendering
	pos = a_Pos;

	vec4 color[4];
	float thresholds[4];
	color[0] = vec4(0.141, 0.373, 1.0, 1.0);
	color[1] = vec4(0.122, 0.941, 0.655, 1.0);
	color[2] = vec4(1.0, 1.0, 0.0, 1.0);
	color[3] = vec4(1.0, 0.0, 0.0, 1.0);
	thresholds[0] = 0.0;
	thresholds[1] = 9.0;
	thresholds[2] = 36.0;
	thresholds[3] = 81.0;
	finalColor = color[0];
	if (u_SpeedSq > thresholds[3]) {
		finalColor = color[3];
	}
	else if (u_SpeedSq > thresholds[2]) {
		finalColor = mix(color[2], color[3], (u_SpeedSq - thresholds[2]) / (thresholds[3] - thresholds[2]));
	}
	else if (u_SpeedSq > thresholds[1]) {
		finalColor = mix(color[1], color[2], (u_SpeedSq - thresholds[1]) / (thresholds[2] - thresholds[1]));
	}
	else if (u_SpeedSq > thresholds[0]) {
		finalColor = mix(color[0], color[1], (u_SpeedSq - thresholds[0]) / (thresholds[1] - thresholds[0]));
	}
}
