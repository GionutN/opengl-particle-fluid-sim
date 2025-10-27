#version 330 core

in vec3 pos;
in vec4 finalColor;

out vec4 fragCol;

void main()
{
	float dist2 = dot(pos.xy, pos.xy);  // the distance of the current pixel from the quad's middle
	float threshold = 0.25;

	if (dist2 < threshold) {
		fragCol = finalColor;
	} else {
		discard;
	}
}
