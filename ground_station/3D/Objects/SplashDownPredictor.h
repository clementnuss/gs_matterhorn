
#ifndef GS_MATTERHORN_SPLASHDOWNPREDICTION_H
#define GS_MATTERHORN_SPLASHDOWNPREDICTION_H


#include <QtGui/QVector3D>
#include <QtGui/QVector2D>
#include <QtCore/QVector>
#include <Qt3DCore/QNode>
#include <3D/Line/Line.h>
#include <3D/Objects/PredictionStrategies/IPredictionStrategy.h>
#include <3D/Ground/Ground.h>
#include "DataStructures/WindData.h"

class SplashDownPredictor {
public:
    SplashDownPredictor(std::string &path, Qt3DCore::QNode *parent);

    void highlightTouchdown(Ground *const ground) const;
    void updatePos(const QVector3D &pos);
    void updateSpeed(const QVector3D &speed);
    void recomputePrediction();

    const WindData *windData();

private:
    static constexpr float LEAKY_FACTOR = 0.95f;
    std::unique_ptr<IPredictionStrategy> predictionStrategy_;
    FlightStatus status_;
    QVector<QVector3D> trajectory_;
    Line *trajectoryLine_;
    WindData windData_;
};


#endif //GS_MATTERHORN_SPLASHDOWNPREDICTION_H
