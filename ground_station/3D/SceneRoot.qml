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

    Transform {
            id: sphereTransform
            property real userAngle: 0.0
            property matrix4x4 m
            property vector3d position

            onUserAngleChanged :{
                                  var newm = Qt.matrix4x4();
                                  newm.rotate(userAngle, Qt.vector3d(0, 1, 0))
                                  newm.translate(Qt.vector3d(20, 10, 0))
                                  m = newm
                                  position = Qt.vector3d(newm.m14,newm.m24,newm.m34)
                                }
            matrix: m
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

    TrackingText{
        id: trackingText
        cameraMatrix : camera.viewMatrix
        parentPosition :sphereTransform.position
        useBold: true
        text: "ROCKET"
        pointSize : 0.5
    }


}
