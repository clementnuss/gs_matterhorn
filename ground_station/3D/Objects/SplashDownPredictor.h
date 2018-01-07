
#ifndef GS_MATTERHORN_SPLASHDOWNPREDICTION_H
#define GS_MATTERHORN_SPLASHDOWNPREDICTION_H


#include <QtGui/QVector3D>
#include <QtGui/QVector2D>
#include <QtCore/QVector>
#include <Qt3DCore/QNode>
#include <3D/Line/Line.h>
#include "DataStructures/WindData.h"

class SplashDownPredictor {
public:
    SplashDownPredictor(std::string &path, Qt3DCore::QNode *parent);

    void updatePos(const QVector3D &pos);

    QVector2D getTouchdownCoordinates();

private:
    void recomputePrediction();

    QVector3D predictorPos_;
    QVector<QVector3D> trajectoryEstimations_;
    float descentSpeed_;
    Line *trajectoryLine_;
    WindData windData_;

    int searchIndex_;
};


#endif //GS_MATTERHORN_SPLASHDOWNPREDICTION_H
