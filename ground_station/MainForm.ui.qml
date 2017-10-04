import QtQuick 2.6

Rectangle {

    width: 800
    height: 600
    property alias group_speed: group_speed
    property alias zone1: zone1
    property alias mouseArea: mouseArea

    MouseArea {
        id: mouseArea
        width: 800
        height: 600
        anchors.fill: parent

        Item {
            id: zone1
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.right: parent.right
            anchors.rightMargin: 10
            anchors.top: parent.top
            anchors.topMargin: 8
            anchors.bottom: parent.verticalCenter
            anchors.bottomMargin: 187

            Item {
                id: ground_zone
                x: 616
                y: 0
                width: 166
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 0
                anchors.top: parent.top
                anchors.topMargin: 0
                anchors.right: parent.right
                anchors.rightMargin: 0

                Rectangle {
                    id: border_ground
                    color: "#ffffff"
                    anchors.fill: parent
                    border.width: 1
                    z: -3

                    Text {
                        id: ground_time
                        x: 62
                        y: 17
                        text: qsTr("00:00:00")
                        font.pointSize: 12
                        anchors.verticalCenterOffset: -30
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                    }

                    Text {
                        id: ground_temperature
                        x: 62
                        y: 43
                        text: qsTr("0.0")
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                        font.pixelSize: 14
                    }

                    Text {
                        id: ground_pressure
                        x: 62
                        y: 70
                        text: qsTr("0.0")
                        anchors.verticalCenterOffset: 25
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                        font.pixelSize: 14
                    }
                }

                Text {
                    id: text7
                    x: 103
                    y: 45
                    text: qsTr("°C")
                    anchors.right: parent.right
                    anchors.rightMargin: 50
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: 14
                }

                Text {
                    id: text8
                    x: 103
                    y: 71
                    text: qsTr("kPa")
                    anchors.right: parent.right
                    anchors.rightMargin: 44
                    anchors.verticalCenterOffset: 25
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: 14
                }
            }

            Item {
                id: link_zone
                x: 460
                y: 0
                width: 150
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 0
                anchors.right: ground_zone.left
                anchors.rightMargin: 6
                anchors.top: parent.top
                anchors.topMargin: 0

                Rectangle {
                    id: border_link
                    color: "#ffffff"
                    anchors.fill: parent
                    border.width: 1
                    z: -14
                }

                Item {
                    id: group_radio_link
                    x: 22
                    y: 15
                    width: 106
                    height: 30
                    anchors.verticalCenterOffset: -20
                    anchors.verticalCenter: parent.verticalCenter

                    Text {
                        id: link_radio_status
                        text: qsTr("Radio link")
                        anchors.leftMargin: 10
                        anchors.bottom: parent.bottom
                        anchors.left: parent.left
                        anchors.top: parent.top
                        fontSizeMode: Text.VerticalFit
                        renderType: Text.NativeRendering
                        horizontalAlignment: Text.AlignLeft
                        padding: 0
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 12
                    }

                    Rectangle {
                        id: border_radio
                        x: 0
                        y: 0
                        width: 106
                        height: 30
                        color: "#ffffff"
                        anchors.horizontalCenterOffset: 0
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: parent.top
                        anchors.topMargin: 0
                        border.width: 1
                        z: -12
                    }

                    Text {
                        id: text5
                        x: 68
                        text: qsTr("____")
                        verticalAlignment: Text.AlignVCenter
                        anchors.right: parent.right
                        anchors.rightMargin: 10
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 0
                        anchors.top: parent.top
                        anchors.topMargin: 0
                        font.pixelSize: 12
                    }
                }

                Item {
                    id: group_video_link
                    x: 22
                    y: 55
                    width: 106
                    height: 30
                    anchors.verticalCenterOffset: 20
                    anchors.verticalCenter: parent.verticalCenter

                    Text {
                        id: link_video_status
                        text: qsTr("Video link")
                        anchors.leftMargin: 10
                        anchors.bottom: parent.bottom
                        anchors.left: parent.left
                        anchors.top: parent.top
                        horizontalAlignment: Text.AlignLeft
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 12
                    }

                    Rectangle {
                        id: border_video
                        color: "#ffffff"
                        anchors.fill: parent
                        border.width: 1
                        z: -11
                    }

                    Text {
                        id: text6
                        x: 67
                        text: qsTr("____")
                        anchors.right: parent.right
                        anchors.rightMargin: 10
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 0
                        anchors.top: parent.top
                        anchors.topMargin: 0
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 12
                    }
                }
            }

            Item {
                id: telemetry_zone
                x: 0
                y: 0
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 0
                anchors.right: link_zone.left
                anchors.rightMargin: 6
                anchors.top: parent.top
                anchors.topMargin: 0
                anchors.left: parent.left
                anchors.leftMargin: 0

                Rectangle {
                    id: border_telemetry
                    color: "#ffffff"
                    radius: 0
                    anchors.fill: parent
                    border.width: 1
                    z: -16
                }

                Item {
                    id: group_altitude
                    height: parent.height / 4.0
                    anchors.top: parent.top
                    anchors.topMargin: 10
                    anchors.right: parent.horizontalCenter
                    anchors.rightMargin: 6
                    anchors.left: parent.left
                    anchors.leftMargin: 10

                    Rectangle {
                        id: border_altitude
                        anchors.fill: parent
                        border.width: 1
                        z: -6
                    }

                    Text {
                        id: units_altitude
                        x: -10
                        width: 40
                        text: qsTr("meters")
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 0
                        anchors.top: parent.top
                        anchors.topMargin: 0
                        verticalAlignment: Text.AlignVCenter
                        anchors.right: parent.right
                        anchors.rightMargin: 10
                        font.pixelSize: 12
                    }

                    Text {
                        id: value_altitude
                        x: 83
                        width: 43
                        text: qsTr("0.0")
                        verticalAlignment: Text.AlignVCenter
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 0
                        anchors.top: parent.top
                        anchors.topMargin: 0
                        horizontalAlignment: Text.AlignRight
                        anchors.right: parent.right
                        anchors.rightMargin: 70
                        font.pixelSize: 12
                    }

                    Text {
                        id: altitude
                        y: 6
                        height: 25
                        text: qsTr("ALT.")
                        verticalAlignment: Text.AlignVCenter
                        font.bold: true
                        padding: 10
                        bottomPadding: 10
                        style: Text.Normal
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        font.pixelSize: 15
                    }
                }

                Item {
                    id: group_speed
                    height: parent.height / 4.0
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.horizontalCenter
                    anchors.rightMargin: 6
                    anchors.left: parent.left
                    anchors.leftMargin: 10

                    Rectangle {
                        id: border_speed
                        color: "#ffffff"
                        anchors.fill: parent
                        border.width: 1
                        z: -8
                    }

                    Text {
                        id: speed
                        text: qsTr("SPEED")
                        font.bold: true
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 0
                        anchors.top: parent.top
                        anchors.topMargin: 0
                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        verticalAlignment: Text.AlignVCenter
                        padding: 10
                        font.pixelSize: 15
                    }

                    Text {
                        id: value_speed
                        x: 100
                        text: qsTr("0.0")
                        horizontalAlignment: Text.AlignRight
                        verticalAlignment: Text.AlignVCenter
                        anchors.top: parent.top
                        anchors.topMargin: 0
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 0
                        anchors.right: parent.right
                        anchors.rightMargin: 70
                        font.pixelSize: 12
                    }

                    Text {
                        id: units_speed
                        x: 162
                        width: 40
                        text: qsTr("m/s")
                        anchors.right: parent.right
                        anchors.rightMargin: 10
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 0
                        anchors.top: parent.top
                        anchors.topMargin: 0
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 12
                    }
                }

                Item {
                    id: group_acceleration
                    height: parent.height / 4.0
                    anchors.right: parent.horizontalCenter
                    anchors.rightMargin: 6
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 11
                    anchors.left: parent.left
                    anchors.leftMargin: 10

                    Rectangle {
                        id: border_acceleration
                        color: "#ffffff"
                        anchors.fill: parent
                        border.width: 1
                        z: -17
                    }

                    Text {
                        id: acceleration
                        text: qsTr("ACCEL.")
                        font.bold: true
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 0
                        anchors.top: parent.top
                        anchors.topMargin: 0
                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        padding: 10
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 15
                    }

                    Text {
                        id: value_acceleration
                        x: 120
                        text: qsTr("0.0")
                        verticalAlignment: Text.AlignVCenter
                        anchors.right: parent.right
                        anchors.rightMargin: 70
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 0
                        anchors.top: parent.top
                        anchors.topMargin: 0
                        font.pixelSize: 12
                    }

                    Text {
                        id: units_acceleration
                        x: 162
                        width: 40
                        text: qsTr("m/s²")
                        verticalAlignment: Text.AlignVCenter
                        anchors.right: parent.right
                        anchors.rightMargin: 10
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 0
                        anchors.top: parent.top
                        anchors.topMargin: 0
                        font.pixelSize: 12
                    }
                }

                Item {
                    id: group_temperature
                    height: parent.height / 4.0
                    anchors.bottomMargin: 17
                    anchors.left: parent.horizontalCenter
                    anchors.leftMargin: 6
                    anchors.right: parent.right
                    anchors.rightMargin: 10
                    anchors.top: parent.top
                    anchors.topMargin: 10

                    Rectangle {
                        id: border_temperature
                        color: "#ffffff"
                        anchors.fill: parent
                        border.width: 1
                        z: -9
                    }

                    Text {
                        id: value_temperature
                        x: 107
                        y: 0
                        text: qsTr("0.0")
                        verticalAlignment: Text.AlignVCenter
                        anchors.right: parent.right
                        anchors.rightMargin: 80
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 0
                        anchors.top: parent.top
                        anchors.topMargin: 0
                        font.pixelSize: 12
                    }

                    Text {
                        id: units_temperature
                        x: 160
                        y: 0
                        width: 40
                        text: qsTr("°C")
                        verticalAlignment: Text.AlignVCenter
                        anchors.right: parent.right
                        anchors.rightMargin: 10
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 0
                        anchors.top: parent.top
                        anchors.topMargin: 0
                        font.pixelSize: 12
                    }

                    Text {
                        id: temperature
                        text: qsTr("TEMP.")
                        font.bold: true
                        anchors.bottom: parent.bottom
                        anchors.left: parent.left
                        anchors.top: parent.top
                        padding: 10
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 15
                    }
                }

                Item {
                    id: group_pressure
                    height: parent.height / 4.0
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.bottomMargin: -12
                    anchors.left: parent.horizontalCenter
                    anchors.leftMargin: 6
                    anchors.right: parent.right
                    anchors.rightMargin: 10
                    anchors.topMargin: -12

                    Rectangle {
                        id: border_pressure
                        color: "#ffffff"
                        anchors.fill: parent
                        border.width: 1
                        z: -10
                    }

                    Text {
                        id: text1
                        x: 107
                        text: qsTr("0.0")
                        verticalAlignment: Text.AlignVCenter
                        anchors.right: parent.right
                        anchors.rightMargin: 80
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 0
                        anchors.top: parent.top
                        anchors.topMargin: 0
                        font.pixelSize: 12
                    }

                    Text {
                        id: pressure
                        text: qsTr("PRESSURE")
                        font.bold: true
                        anchors.bottom: parent.bottom
                        anchors.left: parent.left
                        anchors.top: parent.top
                        verticalAlignment: Text.AlignVCenter
                        padding: 10
                        font.pixelSize: 15
                    }

                    Text {
                        id: text2
                        width: 40
                        text: qsTr("kPa")
                        verticalAlignment: Text.AlignVCenter
                        anchors.right: parent.right
                        anchors.rightMargin: 10
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 0
                        anchors.top: parent.top
                        anchors.topMargin: 0
                        font.pixelSize: 12
                    }
                }

                Item {
                    id: group_ypr
                    height: parent.height / 4.0
                    anchors.left: parent.horizontalCenter
                    anchors.leftMargin: 6
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 10
                    anchors.right: parent.right
                    anchors.rightMargin: 10
                    anchors.topMargin: 18

                    Rectangle {
                        id: border_ypr
                        color: "#ffffff"
                        anchors.fill: parent
                        border.width: 1
                        z: -15
                    }

                    Text {
                        id: ypr
                        width: 39
                        text: qsTr("YPR")
                        font.bold: true
                        anchors.bottom: parent.bottom
                        anchors.left: parent.left
                        anchors.top: parent.top
                        padding: 10
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 15
                    }

                    Text {
                        id: text3
                        x: 68
                        text: qsTr("0.0 / 0.0 / 0.0")
                        anchors.right: parent.right
                        anchors.rightMargin: 80
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 0
                        anchors.top: parent.top
                        anchors.topMargin: 0
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 12
                    }

                    Text {
                        id: text4
                        x: 162
                        width: 40
                        text: qsTr("°")
                        verticalAlignment: Text.AlignVCenter
                        anchors.right: parent.right
                        anchors.rightMargin: 10
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 0
                        anchors.top: parent.top
                        anchors.topMargin: 0
                        font.pixelSize: 12
                    }
                }
            }
        }

        Item {
            id: zone2
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.right: parent.right
            anchors.rightMargin: 8
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 10
            anchors.top: parent.verticalCenter
            anchors.topMargin: -175

            Item {
                id: visualisation_zone
                x: 0
                y: 0
                anchors.right: event_zone.left
                anchors.rightMargin: 10
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 0
                anchors.top: parent.top
                anchors.topMargin: 0
                anchors.left: parent.left
                anchors.leftMargin: 0

                Rectangle {
                    id: visualisation_tab
                    y: 0
                    height: 25
                    color: "#ffffff"
                    anchors.right: parent.right
                    anchors.rightMargin: 0
                    anchors.left: parent.left
                    anchors.leftMargin: 0
                    border.width: 1
                    z: -5
                }

                Rectangle {
                    id: visualisation_area
                    color: "#ffffff"
                    anchors.top: visualisation_tab.bottom
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    anchors.topMargin: 0
                    border.width: 1
                }

                Text {
                    id: visualisation_label
                    height: 25
                    text: qsTr("________ Visualisation")
                    verticalAlignment: Text.AlignVCenter
                    anchors.top: parent.top
                    anchors.topMargin: 0
                    anchors.left: visualisation_tab.left
                    anchors.leftMargin: 10
                    font.pixelSize: 12
                }
            }

            Item {
                id: event_zone
                x: 498
                y: 0
                width: 282
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 0
                anchors.top: parent.top
                anchors.topMargin: 0
                anchors.right: parent.right
                anchors.rightMargin: 0

                Rectangle {
                    id: event_tab
                    height: 25
                    color: "#ffffff"
                    anchors.top: parent.top
                    anchors.topMargin: 0
                    anchors.right: parent.right
                    anchors.rightMargin: 0
                    anchors.left: parent.left
                    anchors.leftMargin: 0
                    border.width: 1
                    z: -4
                }

                TextEdit {
                    id: event_log
                    width: 282
                    text: qsTr("Text Edit")
                    anchors.top: event_tab.bottom
                    anchors.topMargin: 0
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 0
                    anchors.right: parent.right
                    anchors.rightMargin: 0
                    anchors.left: parent.left
                    anchors.leftMargin: 0
                    padding: 20
                    horizontalAlignment: Text.AlignLeft
                    font.pixelSize: 8
                    font.family: "Verdana"
                }

                Rectangle {
                    id: log_zone
                    color: "#ffffff"
                    anchors.left: parent.left
                    anchors.leftMargin: 0
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 0
                    anchors.right: parent.right
                    anchors.rightMargin: 0
                    anchors.top: event_tab.bottom
                    anchors.topMargin: 0
                    z: -5
                    border.width: 1
                }

                Text {
                    id: event_label
                    y: 5
                    height: 25
                    text: qsTr("Event log")
                    anchors.left: event_tab.left
                    anchors.leftMargin: 10
                    verticalAlignment: Text.AlignVCenter
                    anchors.verticalCenter: event_tab.verticalCenter
                    font.pixelSize: 12
                }
            }
        }
    }
}
