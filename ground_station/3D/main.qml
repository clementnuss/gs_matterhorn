import QtQuick 2.0
import QtQuick.Scene3D 2.0

Item {
        Scene3D {
            id: scene3d

            property alias exported_sceneRoot : sceneRoot

            multisample : true
            anchors.fill: parent
            focus: true
            aspects: ["input", "logic"]
            cameraAspectRatioMode: Scene3D.AutomaticAspectRatio

            SceneRoot { id : sceneRoot}
        }

        Rectangle{
            anchors.top: parent.top
            anchors.topMargin: 10
            anchors.left: parent.left
            anchors.leftMargin: 20

            Text{
                text: "x: %1 y: %2 z: %3".arg(scene3d.exported_sceneRoot.cameraPosition.x)
                .arg(scene3d.exported_sceneRoot.cameraPosition.y)
                .arg(scene3d.exported_sceneRoot.cameraPosition.z)
                font.pointSize : 12
                color: "white"
            }
        }

}
