#version 450 core

layout(binding = 0) uniform sampler2D colorTexture;

layout(location = 0) in vec2 inTexcoord;
layout(location = 0) out vec4 outColor;

void main() {
  outColor = texture(colorTexture, inTexcoord);
}
