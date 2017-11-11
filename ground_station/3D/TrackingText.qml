import Qt3D.Core 2.9
import Qt3D.Render 2.9
import Qt3D.Input 2.0
import Qt3D.Extras 2.9

import QtQuick 2.9 as QQ2
import "Helpers.js" as Helpers

Entity{

    id: root

    property matrix4x4 cameraMatrix
    property vector3d parentPosition : Qt.vector3d(0,0,0)
    property vector3d offset : Qt.vector3d(1,0,0)
    property real pointSize : 0.5
    property bool useBold : false
    property string text : ""


    Transform {
        id: textTransform
        matrix: {
            var m = Qt.matrix4x4()
            m.translate(offset)
            m = Helpers.billboardMV(parentPosition, cameraMatrix).times(m)
            return m
        }
    }

    QQ2.TextMetrics {
        id: textMetrics
        font.pointSize : root.pointSize
        font.bold : root.useBold
        text: root.text
    }

    Text2DEntity {
        id: text
        text: textMetrics.text
        font.pointSize: root.pointSize
        font.bold : root.useBold
        width: textMetrics.width
        height: textMetrics.height

        components: [textTransform, renderSettings.activeFrameGraph.markerLayer]
    }
}