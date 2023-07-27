#version 450
#extension GL_GOOGLE_include_directive: enable
#extension GL_EXT_nonuniform_qualifier: require

layout (set = 0, binding = 0) uniform sampler2D sampler2d;
layout (set = 0, binding = 1) uniform MatrixData
{
    mat2 mvp;
} matrixData[2];
layout (set = 0, binding = 3) uniform sampler2DArray sampler2dArray;
layout (set = 0, binding = 4) uniform samplerCube cubes[];

layout(push_constant) uniform TransformConstant { layout(offset = 16) vec4 uiTransform; } pcTransform;

layout(location = 0) in vec3 outPosition;
layout(location = 0) out vec4 ColorAttachment;

void main() 
{
    vec2 aPosition = gl_FragCoord.xy;
    aPosition *= matrixData[0].mvp;
    aPosition *= matrixData[1].mvp;

    vec3 color = vec3(1, 1, 1);
    color *= texture(sampler2d, aPosition).rgb;
    color *= texture(sampler2dArray, vec3(aPosition, 1)).rgb;
    color *= texture(cubes[256], vec3(aPosition, 1)).rgb;
    color *= texture(cubes[nonuniformEXT(int(aPosition.x))], vec3(aPosition, 1)).rgb;
    
    ColorAttachment = vec4(color, 1);
}
