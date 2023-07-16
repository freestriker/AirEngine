#version 450
#extension GL_GOOGLE_include_directive: enable
#extension GL_EXT_nonuniform_qualifier: require

layout (input_attachment_index = 0, set = 0, binding = 0) uniform subpassInput inputColor;
layout (input_attachment_index = 1, set = 0, binding = 1) uniform subpassInput inputDepth;
layout (set = 0, binding = 2) uniform sampler2D sampler2d;
layout (set = 0, binding = 3) uniform MatrixData
{
    mat2 mvp;
} matrixData[2];
layout (set = 0, binding = 5) uniform sampler2DArray sampler2dArray;
layout (set = 0, binding = 6) uniform samplerCube cubes[];

layout(location = 0) in vec3 outPosition;
layout(location = 0) out vec4 SwapchainAttachment;

layout(push_constant) uniform AttachmentSizeInfo
{
    vec2 size;
}attachmentSizeInfo;

void main() 
{
    vec2 aPosition = gl_FragCoord.xy / attachmentSizeInfo.size;
    aPosition *= matrixData[0].mvp;
    aPosition *= matrixData[1].mvp;

    vec3 color = vec3(1, 1, 1);
    color *= subpassLoad(inputColor).rgb;
    color *= subpassLoad(inputDepth).rgb;
    color *= texture(sampler2d, aPosition).rgb;
    color *= texture(sampler2dArray, vec3(aPosition, 1)).rgb;
    color *= texture(cubes[256], vec3(aPosition, 1)).rgb;
    color *= texture(cubes[nonuniformEXT(int(aPosition.x))], vec3(aPosition, 1)).rgb;
    
    SwapchainAttachment = vec4(color, 1);
}
