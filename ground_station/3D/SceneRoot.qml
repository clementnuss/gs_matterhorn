import Qt3D.Core 2.9
import Qt3D.Render 2.9
import Qt3D.Input 2.0
import Qt3D.Extras 2.9

import QtQuick 2.9 as QQ2


Entity {
    id: sceneRoot


    Camera {
        id: camera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        nearPlane : 0.1
        farPlane : 1000.0
        position: Qt.vector3d( 0.0, 1.0, 20.0 )
        upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
    }

    FirstPersonCameraController { camera: camera }

    components: [
        RenderSettings {
            id: renderSettings
            activeFrameGraph: TerrainForwardRenderer {
                camera: camera

            }
        },
        InputSettings { }
    ]

    PhongMaterial {
        id: material
    }

    SphereMesh {
        id: sphereMesh
        radius: 1
    }

    CuboidMesh{
        id: cubeMesh
    }

    Transform {
            id: sphereTransform
            property real userAngle: 0.0
            matrix: {
                var m = Qt.matrix4x4();
                m.rotate(userAngle, Qt.vector3d(0, 1, 0))
                m.translate(Qt.vector3d(20, 10, 0));
                return m;
            }
        }

    Transform {
            id: cubeTransform
            matrix: billboard(Qt.vector3d(0.0,0.0,0.0), camera.position, camera.upVector)
        }

    function billboard(position, cameraPos, cameraUp) {
        var look = cameraPos.minus(position).normalized();
        var right = cameraUp.crossProduct(look);
        var up2 = look.crossProduct(right);
        var transform = Qt.matrix4x4();

        transform.m11 = right.x;
        transform.m12 = right.y;
        transform.m13 = right.z;
        transform.m14 = 0;

        transform.m21 = up2.x;
        transform.m22 = up2.y;
        transform.m23 = up2.z;
        transform.m24 = 0;

        transform.m31 = look.x;
        transform.m32 = look.y;
        transform.m33 = look.z;
        transform.m34 = 0;

        transform.m41 = 0;
        transform.m42 = 0;
        transform.m43 = 0;
        transform.m44 = 1;

        // Uncomment this line to translate the position as well
        // (without it, it's just a rotation)
        //transform[3] = vec4(position, 1);

        return transform.transposed()
    }

    Transform {
            id: textTransform
            matrix: {
                var mat = billboard(Qt.vector3d(0.0,0.0,0.0), camera.position, Qt.vector3d(0.0,1.0,0.0))
                //mat.translate(Qt.vector3d(-2,-2,0))
                mat.scale(0.1,0.1,0.1)
                return mat
            }
        }

    QQ2.NumberAnimation {
        target: sphereTransform
        property: "userAngle"
        duration: 10000
        from: 0
        to: 360

        loops: QQ2.Animation.Infinite
        running: true
    }

    Entity {
        id: sphereEntity
        components: [ sphereMesh, material, sphereTransform, renderSettings.activeFrameGraph.waveLayer]
    }

    Entity {
        id: cubeEntity
        components: [cubeMesh, material, cubeTransform, renderSettings.activeFrameGraph.waveLayer]
    }

    Terrain {
            id: terrain
            layer: renderSettings.activeFrameGraph.waveLayer
    }

    Text2DEntity {
        id: text
        text: "Hello World"
        font.pointSize : 3
        width: 40
        height: 10

        components: [textTransform]
    }


}
