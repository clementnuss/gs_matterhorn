import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0

import "Helpers.js" as Helpers

Entity{

    property string texturePath
    property vector3d position : Qt.vector3d(0,0,0)
    property vector3d offset : Qt.vector3d(0,2,0)

    Transform {
        id: transform

        matrix : {
            var m = Qt.matrix4x4()

            // Rotate it so that when facing camera top of texture goes on top in 3D world
            m.rotate(180, Qt.vector3d(0,1,0))
            // Make it face camera since default is like laying flat on ground
            m.rotate(-90, Qt.vector3d(1,0,0))
            m = Helpers.billboardMV(position.plus(offset), camera.viewMatrix).times(m)
            return m
        }
    }

    PlaneMesh {
        id: mesh

        width: 2.0
        height: 2.0
        meshResolution: Qt.size( 2, 2 )
    }

    Material {
            id: material

            effect: MarkerEffect { id: effect }

            parameters: [
                            Parameter {
                                        name: "diffuseTexture"
                                        value: Texture2D {
                                            id: diffuseTexture
                                            minificationFilter: Texture.LinearMipMapLinear
                                            magnificationFilter: Texture.Linear
                                            wrapMode {
                                                x: WrapMode.ClampToEdge
                                                y: WrapMode.ClampToEdge
                                            }
                                            generateMipMaps: true
                                            maximumAnisotropy: 4.0
                                            TextureImage {
                                                id: diffuseTextureImage
                                                source: texturePath
                                            }
                                        }
                                    }
                        ]
        }

    components: [transform, mesh, material, renderSettings.activeFrameGraph.markerLayer]

}