#version 410

// These are applied by SGL using AttributeBuilder::ApplyDefaultUniforms(),
// The values ie resolution and time must be calculated yourself depending on your windowing backend.
// The examples show a way of doing this.
uniform vec2 iResolution;
uniform float iTime;

// SGL's AttributeBuilder outputs one Vector2 "TexCoord".
in vec2 TexCoord;

// Vector4 for the colour must be outputted, naming doesn't matter. This is the final fragment colour per pixel.
out vec4 FragColor;


// Credit: https://www.shadertoy.com/view/tlVGDt
float gTime = 0.0;
const float REPEAT = 5.0; // (unused but kept for completeness)

// Rotation matrix
mat2 rot(float a) {
	float c = cos(a), s = sin(a);
	return mat2(c, s, -s, c);
}

float sdBox(vec3 p, vec3 b) {
	vec3 q = abs(p) - b;
	return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

float box(vec3 pos, float scale) {
	pos *= scale;
	float base = sdBox(pos, vec3(0.4, 0.4, 0.1)) / 1.5;
	pos.xy *= 5.0;
	pos.y  -= 3.5;
	pos.xy *= rot(0.75);
	float result = -base;
	return result;
}

float box_set(vec3 pos, float timeVal) {
	vec3 pos_origin = pos;

	// 1
	pos = pos_origin;
	pos.y += sin(gTime * 0.4) * 2.5;
	pos.xy *= rot(0.8);
	float box1 = box(pos, 2.0 - abs(sin(gTime * 0.4)) * 1.5);

	// 2
	pos = pos_origin;
	pos.y -= sin(gTime * 0.4) * 2.5;
	pos.xy *= rot(0.8);
	float box2 = box(pos, 2.0 - abs(sin(gTime * 0.4)) * 1.5);

	// 3
	pos = pos_origin;
	pos.x += sin(gTime * 0.4) * 2.5;
	pos.xy *= rot(0.8);
	float box3 = box(pos, 2.0 - abs(sin(gTime * 0.4)) * 1.5);

	// 4
	pos = pos_origin;
	pos.x -= sin(gTime * 0.4) * 2.5;
	pos.xy *= rot(0.8);
	float box4 = box(pos, 2.0 - abs(sin(gTime * 0.4)) * 1.5);

	// 5
	pos = pos_origin;
	pos.xy *= rot(0.8);
	float box5 = box(pos, 0.5) * 6.0;

	// 6
	pos = pos_origin;
	float box6 = box(pos, 0.5) * 6.0;

	float result = max(
	max(
	max(
	max(
	max(box1, box2),
	box3
	),
	box4
	),
	box5
	),
	box6
	);
	return result;
}

float mapScene(vec3 pos, float timeVal) {
	vec3 pos_origin = pos;
	float box_set1 = box_set(pos_origin, timeVal);
	return box_set1;
}

// ------------------------------------------------------------------
// Main (converted from mainImage)
// ------------------------------------------------------------------
void main() {
	vec2 fragCoord = gl_FragCoord.xy;

	// Normalized pixel coords (keeping aspect)
	vec2 p = (fragCoord * 2.0 - iResolution.xy) /
	min(iResolution.x, iResolution.y);

	// Ray origin and direction
	vec3 ro  = vec3(0.0, -0.2, iTime * 4.0);
	vec3 ray = normalize(vec3(p, 1.5));

	ray.xy = ray.xy * rot(sin(iTime * 0.03) * 5.0);
	ray.yz = ray.yz * rot(sin(iTime * 0.05) * 0.2);

	float t   = 0.1;
	vec3  col = vec3(0.0);
	float ac  = 0.0;

	// Raymarch loop
	for (int i = 0; i < 99; i++) {
		vec3 pos = ro + ray * t;
		pos = mod(pos - 2.0, 4.0) - 2.0;

		gTime = iTime - float(i) * 0.01;

		float d = mapScene(pos, iTime);

		d  = max(abs(d), 0.01);
		ac += exp(-d * 23.0);

		t += d * 0.55;
	}

	col = vec3(ac * 0.02);
	col += vec3(0.0,
	0.2 * abs(sin(iTime)),
	0.5 + sin(iTime) * 0.2);

	float alpha = 1.0 - t * (0.02 + 0.02 * sin(iTime));

	FragColor = vec4(col, alpha);
}
