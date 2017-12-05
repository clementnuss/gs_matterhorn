
#ifndef GS_MATTERHORN_SPLASHDOWNPREDICTION_H
#define GS_MATTERHORN_SPLASHDOWNPREDICTION_H


#include <QtGui/QVector3D>
#include <QtCore/QVector>
#include <Qt3DCore/QNode>
#include <3D/Line/Line.h>

class SplashDownPredictor {
public:
    SplashDownPredictor(Qt3DCore::QNode *parent);

    void updatePos(const QVector3D &pos);

    QVector2D getTouchdownCoordinates();

private:
    void recomputePrediction();

    QVector2D windVectorForAltitude(float alt);

    QVector<QPair<int, QVector2D>> windTable_;
    QVector3D predictorPos_;
    QVector<QVector3D> trajectoryEstimations_;
    float descentSpeed_;
    Line *trajectoryLine_;

    int searchIndex_;
};


#endif //GS_MATTERHORN_SPLASHDOWNPREDICTION_H
