{
    "renderPass": "AirEngine::Runtime::FrontEnd::PresentRenderPass",
    "subShaders": 
    [
        {
            "subPass": "RtTestSubpass",

            "spvShaderPaths": 
            [
                "../../Resources/Shader/Spv/RtTest.rchit.spv",
                "../../Resources/Shader/Spv/RtTest.rgen.spv",
                "../../Resources/Shader/Spv/RtTest.rmiss.spv"
            ],

            "cullMode": "NONE",
            "frountFace": "CLOCKWISE",

            "depthTestEnable": false,
            "depthWriteEnable": false,
            "depthCompareOp": "LESS",

            "colorAttachmentBlendStates":
            [
            ]
        }
    ]
}