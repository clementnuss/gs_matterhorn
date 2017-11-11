import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0

Entity{

    id : root

    property vector3d position : Qt.vector3d(0,0,0)

    CuboidMesh{
        id : groundStationMesh
    }

    Transform{
        id: groundStationTransform
        matrix : {
            var m = Qt.matrix4x4()
            m.translate(position)
            return m
        }
    }

    PhongMaterial{
        id: groundStationMaterial

    }

    components : [groundStationMesh, groundStationTransform, groundStationMaterial]

    TrackingText{
            id: trackingText
            cameraMatrix : camera.viewMatrix
            useBold: true
            text: "GROUND STATION"
            pointSize : 0.5
    }
}