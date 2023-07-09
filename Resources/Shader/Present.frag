#version 450
#extension GL_GOOGLE_include_directive: enable

layout(set = 0, binding = 0) uniform sampler2D colorAttachment;
layout(push_constant) uniform AttachmentSizeInfo
{
    vec2 size;
}attachmentSizeInfo;

layout(location = 0) out vec4 SwapchainAttachment;

void main() 
{
    vec2 aPosition = gl_FragCoord.xy / attachmentSizeInfo.size;
    SwapchainAttachment = vec4(texture(colorAttachment, aPosition).rgb, 1);
}
