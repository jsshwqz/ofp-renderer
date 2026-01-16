#version 450

layout(binding = 0) uniform sampler2D inputTexture;

layout(location = 0) in vec2 inTexCoord;
layout(location = 0) out vec4 outColor;

void main() {
    vec4 color = texture(inputTexture, inTexCoord);
    vec3 brightness = max(color.rgb, vec3(0.0));
    float glare = length(brightness) * 0.5;
    outColor = color + vec4(vec3(glare), 0.0);
}
