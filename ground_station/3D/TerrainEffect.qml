import Qt3D.Core 2.0
import Qt3D.Render 2.0

Effect {
    id: root

    property bool wireframe: false

    // These parameters act as default values for the effect. They take
    // priority over any parameters specified in the RenderPasses below
    // (none provided in this example). In turn these parameters can be
    // overwritten by specifying them in a Material that references this
    // effect.
    // The priority order is:
    //
    // Material -> Effect -> Technique -> RenderPass -> GLSL default values
    parameters: [
        Parameter { name: "ka"; value: Qt.vector3d( 0.1, 0.1, 0.1 ) },
        Parameter { name: "kd"; value: Qt.vector3d( 0.7, 0.7, 0.7 ) }
    ]

    techniques: [
        Technique {
            graphicsApiFilter {
                api: GraphicsApiFilter.OpenGL
                profile: GraphicsApiFilter.CoreProfile
                majorVersion: 3
                minorVersion: 2
            }

            filterKeys: [ FilterKey { name: "renderingStyle"; value: "forward" } ]

            parameters: [
                Parameter { name: "light.position"; value: Qt.vector4d( 0.0, 0.0, 0.0, 1.0 ) },
                Parameter { name: "light.intensity"; value: Qt.vector3d( 1.0, 1.0, 1.0 ) },
                Parameter { name: "line.width"; value: 0.8 },
                Parameter { name: "line.color"; value: Qt.vector4d( 1.0, 1.0, 1.0, 1.0 ) }
            ]

            renderPasses: [
                RenderPass {
                    shaderProgram: standardShader

                    ShaderProgram {
                        id: standardShader
                        vertexShaderCode:   loadSource("qrc:/shaders/terrain.vert")
                        fragmentShaderCode: loadSource("qrc:/shaders/DShader.frag")
                    }
                }
            ]
        }
    ]
}
