import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0

Entity {
    id: root

    property real x: 0.0
    property real y: 0.0
    property real z: 0.0
    property real scale: 1.0
    property real theta: 0.0
    property real phi: 0.0
    property color tint: Qt.rgba( 0.0, 0.0, 0.0, 1.0 )
    property real intensity: 0.3
    property Layer layer: null

    Transform {
        id: transform
        translation: Qt.vector3d(root.x, root.y, root.z);
        rotation: fromEulerAngles(root.theta, root.phi)
        scale: root.scale
    }

    TerrainMaterial {
        id: material
    }

    PlaneMesh {
        id: mesh
        width: 25.0
        height: 25.0
        meshResolution: Qt.size( 100, 100 )
    }

    components: [ transform, mesh, material, layer ]

}
