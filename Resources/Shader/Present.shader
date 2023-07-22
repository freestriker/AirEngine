{
    "renderPass": "AirEngine::Runtime::Graphic::Instance::DummyRenderPass",
    "subShaders": 
    [
        {
            "subPass": "DrawSubpass",

            "spvShaderPaths": 
            [
                "../../Resources/Shader/Spv/Present.vert.spv",
                "../../Resources/Shader/Spv/Present.frag.spv"
            ],

            "cullModes": ["BACK"],
            "frountFace": "CLOCKWISE",

            "depthTestEnable": true,
            "depthWriteEnable": true,
            "depthCompareOp": "LESS",

            "colorAttachmentBlendStates":
            [
                {
                    "colorAttachment": "ColorAttachment",

                    "blendEnable": false,
                    "srcColorBlendFactor": "ZERO",
                    "colorBlendOp": "ADD",
                    "dstColorBlendFactor": "ZERO",
                    "srcAlphaBlendFactor": "ZERO",
                    "alphaBlendOp": "ADD",
                    "dstAlphaBlendFactor": "ZERO",
        
                    "colorWriteMasks": ["R", "G", "B"] 
                }
            ]
        }
    ]
}