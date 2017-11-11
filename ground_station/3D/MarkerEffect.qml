import Qt3D.Core 2.0
import Qt3D.Render 2.0

Effect {
    id: root

    techniques: [
        Technique {
            graphicsApiFilter {
                api: GraphicsApiFilter.OpenGL
                profile: GraphicsApiFilter.CoreProfile
                majorVersion: 3
                minorVersion: 2
            }

            filterKeys: [ FilterKey { name: "renderingStyle"; value: "forward" } ]

            renderPasses: [
                RenderPass {
                    shaderProgram: standardShader

                    renderStates: [
                                    BlendEquationArguments {
                                    sourceAlpha: BlendEquationArguments.One
                                    destinationAlpha: BlendEquationArguments.OneMinusSourceAlpha
                                    sourceRgb : BlendEquationArguments.SourceAlpha
                                    destinationRgb : BlendEquationArguments.OneMinusSourceAlpha
                                    }
                                  ]

                    ShaderProgram {
                        id: standardShader
                        vertexShaderCode:   loadSource("qrc:/shaders/marker.vert")
                        fragmentShaderCode: loadSource("qrc:/shaders/DShader.frag")
                    }
                }
            ]
        }
    ]
}