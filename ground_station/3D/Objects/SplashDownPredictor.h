
#ifndef GS_MATTERHORN_SPLASHDOWNPREDICTION_H
#define GS_MATTERHORN_SPLASHDOWNPREDICTION_H


#include <QtGui/QVector3D>
#include <QtGui/QVector2D>
#include <QtCore/QVector>
#include <Qt3DCore/QNode>
#include <3D/Line/Line.h>

class SplashDownPredictor {
public:
    SplashDownPredictor(std::string &path, Qt3DCore::QNode *parent);

    void updatePos(const QVector3D &pos);

    QVector2D getTouchdownCoordinates();

private:
    void loadPredictions();
    void recomputePrediction();

    QVector2D windVectorForAltitude(float alt);

    QVector2D dataToWindVector(const float windSpeed, const float windAngle);

    QVector<QPair<int, QVector2D>> windTable_;
    QVector3D predictorPos_;
    QVector<QVector3D> trajectoryEstimations_;
    float descentSpeed_;
    Line *trajectoryLine_;
    std::string predictionPath_;

    int searchIndex_;
};


#endif //GS_MATTERHORN_SPLASHDOWNPREDICTION_H
