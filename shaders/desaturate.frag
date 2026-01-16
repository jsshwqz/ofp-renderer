#version 450

layout(binding = 0) uniform sampler2D inputTexture;

layout(location = 0) in vec2 inTexCoord;
layout(location = 0) out vec4 outColor;

void main() {
    vec4 color = texture(inputTexture, inTexCoord);
    float luminance = dot(color.rgb, vec3(0.299, 0.587, 0.114));
    outColor = mix(color, vec4(vec3(luminance), color.a), 0.5);
}
