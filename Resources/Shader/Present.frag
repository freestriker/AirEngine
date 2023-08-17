#version 450
#extension GL_GOOGLE_include_directive: enable
#extension GL_EXT_nonuniform_qualifier: require

layout (set = 0, binding = 0) uniform S0B0
{
    mat2 mvp;
}sampler2d;
layout (set = 0, binding = 1) uniform MatrixData
{
    mat2 mvp;
} matrixData[2];
layout (set = 0, binding = 3) uniform S0B3
{
    mat2 mvp;
}sampler2dArray;
layout (set = 0, binding = 4) uniform S0B4
{
    mat2 mvp;
}cubes[];

layout(push_constant) uniform TransformConstant { layout(offset = 16) vec4 uiTransform; } pcTransform;

layout(location = 0) in vec3 outPosition;
layout(location = 0) out vec4 ColorAttachment;

void main() 
{
    // vec2 aPosition = gl_FragCoord.xy;
    // aPosition *= matrixData[0].mvp;
    // aPosition *= matrixData[1].mvp;

    vec3 color = vec3(1, 1, 1);
    // color *= texture(sampler2d, aPosition).rgb;
    // color *= texture(sampler2dArray, vec3(aPosition, 1)).rgb;
    color *= cubes[256].mvp[0][0];
    color *= cubes[nonuniformEXT(int(color.x))].mvp[0][0];
    
    ColorAttachment = vec4(1, 1, 1, 1);
}
