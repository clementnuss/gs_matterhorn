import Qt3D.Core 2.10
import Qt3D.Render 2.10

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

                    ShaderProgram {
                        id: standardShader
                        vertexShaderCode:   loadSource("qrc:/shaders/line.vert")
                        fragmentShaderCode: loadSource("qrc:/shaders/line.frag")
                    }

                    renderStates: [
                                    LineWidth { value: 5.0 }
                                ]
                }
            ]
        }
    ]
}