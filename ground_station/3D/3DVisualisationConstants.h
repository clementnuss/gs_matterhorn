
#ifndef GS_MATTERHORN_3DVISUALISATIONCONSTANTS_H
#define GS_MATTERHORN_3DVISUALISATIONCONSTANTS_H

#include <QVector3D>

namespace OpenGLConstants {
    static constexpr int VERSION_MAJOR = 2;
    static constexpr int VERSION_MINOR = 1;
    static constexpr int ANISOTROPY_MAX = 8;
    static constexpr int MAX_LINE_RESOLUTION = 500;
    static constexpr int MAX_LINE_DATAPOINTS = 100000;
    static constexpr int RULER_SPACING = 500;
    static constexpr int RULER_MAX_HEIGHT = 5000;
    static const QVector3D ABOVE = QVector3D(0, 1, 0);
    static const QVector3D ABOVE_CENTER_LABEL = QVector3D(-4, 2.5, 0);
    static const QVector3D RIGHT_1 = QVector3D(1, 0, 0);
    static const QVector3D RIGHT_2 = QVector3D(2, 0, 0);
    static const QVector3D RIGHT_4 = QVector3D(4, 0, 0);
    static const QVector3D ABOVE_RIGHT = QVector3D(2, 1, 0);
    static const QVector3D RIGHT_LABEL = QVector3D(3, -0.5f, 0);
    static const QVector3D RIGHT_TICK = QVector3D(0.5, 0, 0);
}

namespace TextureConstants {
    static const QString CARET_LEFT = QStringLiteral("qrc:/3D/textures/angle-left.png");
    static const QString CARET_DOWN = QStringLiteral("qrc:/3D/textures/caret_down.png");
    static const QString DOUBLE_DOWN_ARROW = QStringLiteral("qrc:/3D/textures/double_down_arrow.png");
}


namespace CameraConstants {
    static const QVector3D UP_VECTOR = QVector3D(0, 1, 0);
    static constexpr float INTERPOLATION_STRENGTH = 0.1f;
    static constexpr float INTERPOLATION_THRESHOLD = 0.0001f;
    static constexpr float VIEWING_DISTANCE_MIN = 400.0f;
    static constexpr float VIEWING_DISTANCE_MAX = 10000.0f;
    static constexpr float VIEWING_DISTANCE_DEFAULT = 5000.0f;
    static constexpr float AZIMUTH_STEP = 0.785398f; // 45 degrees
    static constexpr float POLAR_STEP = 0.261799f; // 15 degrees
    static constexpr float POLAR_MIN = 0.261799f;
    static constexpr float POLAR_MAX = 2.87979f;
    static constexpr float ANIMATION_DURATION = 400.0f;
}


#endif //GS_MATTERHORN_3DVISUALISATIONCONSTANTS_H
