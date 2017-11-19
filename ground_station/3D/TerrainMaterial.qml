import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0

Material {
        id: root

        effect: TerrainEffect { id: effect }

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
                                          maximumAnisotropy: 16.0
                                        TextureImage {
                                            id: diffuseTextureImage
                                            source: "qrc:3D/textures/diffusemap_photo_1024.jpg"
                                        }
                                    }
                                },
                        Parameter {
                            name: "heightTexture"
                            value: Texture2D {
                                id: heightTexture
                                minificationFilter: Texture.LinearMipMapLinear
                                magnificationFilter: Texture.Linear
                                wrapMode {
                                    x: WrapMode.ClampToEdge
                                    y: WrapMode.ClampToEdge
                                }
                                generateMipMaps: true
                                maximumAnisotropy: 16.0
                                TextureImage {
                                    id: heightTextureImage
                                    source: "qrc:3D/textures/heightmap_1024.jpg"
                                }
                            }
                        }
                    ]
    }
