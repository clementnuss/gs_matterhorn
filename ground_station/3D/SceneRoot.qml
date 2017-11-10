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
        fieldOfView: 70
        nearPlane : 0.1
        farPlane : 1000.0
        position: Qt.vector3d( 0.0, 10.0, 20.0 )
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
            id: textTransform
            property real userAngle: 0.0
            matrix: {
                var m = Qt.matrix4x4();
                m.scale(Qt.vector3d(0.1, 0.1, 1.0))
                return m;
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

    Terrain {
            id: terrain
            layer: renderSettings.activeFrameGraph.waveLayer
    }

    Text2DEntity {
            id: text
            text: "Hello World"
            width: 400
            height: 30

            components: [textTransform]
    }

}
