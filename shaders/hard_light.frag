#version 450

layout(binding = 0) uniform sampler2D inputTexture;

layout(location = 0) in vec2 inTexCoord;
layout(location = 0) out vec4 outColor;

void main() {
    vec4 color = texture(inputTexture, inTexCoord);
    vec3 hardLight = color.rgb * 2.0;
    hardLight = mix(hardLight, 1.0 - 2.0 * (1.0 - color.rgb), step(0.5, color.rgb));
    outColor = vec4(hardLight, color.a);
}
