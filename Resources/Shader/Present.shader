{
    "renderPass": "AirEngine::Runtime::FrontEnd::PresentRenderPass",
    "subShaders": 
    [
        {
            "subPass": "PresentSubpass",

            "spvShaderPaths": 
            [
                "../../Resources/Shader/Spv/Present.vert.spv",
                "../../Resources/Shader/Spv/Present.frag.spv"
            ],

            "cullMode": "NONE",
            "frountFace": "CLOCKWISE",

            "depthTestEnable": false,
            "depthWriteEnable": false,
            "depthCompareOp": "LESS",

            "colorAttachmentBlendStates":
            [
                {
                    "colorAttachment": "SwapchainAttachment",

                    "blendEnable": false,
                    "srcColorBlendFactor": "ZERO",
                    "colorBlendOp": "ADD",
                    "dstColorBlendFactor": "ZERO",
                    "srcAlphaBlendFactor": "ZERO",
                    "alphaBlendOp": "ADD",
                    "dstAlphaBlendFactor": "ZERO",
        
                    "colorWriteMasks": ["R", "G", "B", "A"] 
                }
            ]
        }
    ]
}