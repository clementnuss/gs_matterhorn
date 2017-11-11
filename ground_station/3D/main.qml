import QtQuick 2.0
import QtQuick.Scene3D 2.0

Item {
        Scene3D {
            id: scene3d
            multisample : false
            anchors.fill: parent
            anchors.margins: 10
            focus: true
            aspects: ["input", "logic"]
            cameraAspectRatioMode: Scene3D.AutomaticAspectRatio

            SceneRoot {}
        }

        Rectangle{
            Text{
                text: "Test"
            }
        }

}
