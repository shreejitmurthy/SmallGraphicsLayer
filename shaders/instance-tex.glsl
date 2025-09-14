@ctype mat4 SmallGraphicsLayer::Math::Mat4

@vs vs

layout(binding = 0) uniform params {
    mat4 proj;
};

in vec2 aPos;
// in vec2 aTexCoord;
in vec2 aUV;
in vec2 aOffset;
in vec2 aUVOffset;
in vec2 aWorldScale;
in vec2 aUVScale;

out vec2 vUV;

void main() {
    vec2 world_xy = aPos * aWorldScale + aOffset;
    gl_Position = proj * vec4(world_xy, 0.0, 1.0);
    // vUV = aTexCoord + aUVOffset;
    vUV = aUVOffset + (aUV * aUVScale);
}
@end

@fs fs
out vec4 FragColor;

in vec2 vUV;

layout(binding = 0) uniform texture2D tex;
layout(binding = 0) uniform sampler smp;
#define ourTexture sampler2D(tex, smp)

void main() {
    FragColor = texture(ourTexture, vUV);
}
@end

@program main vs fs
