#version 450
#extension GL_GOOGLE_include_directive: enable

layout (set = 0, binding = 0) uniform sampler sourceAttachmentSampler;
layout (set = 0, binding = 1) uniform texture2D sourceAttachment;
layout(push_constant) uniform ConstantInfo 
{ 
    vec4 swapchainAttachmentSize;
} constantInfo;

layout(location = 0) in vec3 vertexPosition;
layout(location = 0) out vec4 outParameter;

void main() 
{
    gl_Position = vec4(vertexPosition.xy, 0, 1.0);
    outParameter = vec4(0, 1, 2, 3);
}
