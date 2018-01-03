
#ifndef GS_MATTERHORN_CAMERACONTROLLER_H
#define GS_MATTERHORN_CAMERACONTROLLER_H


#include <Qt3DExtras/QAbstractCameraController>
#include <Utilities/TimeUtils.h>
#include <ProgramConstants.h>
#include <iostream>
#include <memory>
#include <Qt3DInput/QKeyboardHandler>
#include <Qt3DInput/QKeyboardDevice>
#include <Qt3DCore/QTransform>
#include "3DVisualisationConstants.h"


static float easeInOutCubic(float t) {
    return t < .5 ? 4 * t * t * t : (t - 1) * (2 * t - 2) * (2 * t - 2) + 1; // From https://gist.github.com/gre/1650294
}

static float normalizedTime(const std::chrono::system_clock::time_point &initialTime) {
    return msecsBetween(initialTime, std::chrono::system_clock::now()) / CameraConstants::ANIMATION_DURATION;
}

static float
floatInterpolate(const std::chrono::system_clock::time_point &initialTime, const float &v1, const float &v2) {
    float t = easeInOutCubic(normalizedTime(initialTime));
    return (t * v2) + ((1.0f - t) * v1);
}

static QVector3D
qVector3DInterpolate(const std::chrono::system_clock::time_point &initialTime, const QVector3D &v1,
                     const QVector3D &v2) {
    float t = easeInOutCubic(normalizedTime(initialTime));
    return (t * v2) + ((1.0f - t) * v1);
}

class Interpolator {
public:
    Interpolator() : animationStartTime_{std::chrono::system_clock::now()} {}

    virtual void updateTarget() = 0;

    bool animationIsFinished() const {
        return msecsBetween(animationStartTime_, std::chrono::system_clock::now()) >
               CameraConstants::ANIMATION_DURATION;
    }

protected:
    std::chrono::system_clock::time_point animationStartTime_;
};

class FloatInterpolator : public Interpolator {
public:
    FloatInterpolator(const float v1, const float v2, float *target) :
            v1_{v1},
            v2_{v2},
            target_{target} {}

    void updateTarget() override {
        *target_ = floatInterpolate(animationStartTime_, v1_, v2_);
    }

private:
    float v1_;
    float v2_;
    float *target_;
};


class QVector3DInterpolator : public Interpolator {
public:
    QVector3DInterpolator(const QVector3D &v1, const QVector3D &v2, QVector3D *target)
            :
            v1_{v1},
            v2_{v2},
            target_{target} {}

    void updateTarget() override {
        *target_ = qVector3DInterpolate(animationStartTime_, v1_, v2_);
    }

private:
    QVector3D v1_;
    QVector3D v2_;
    QVector3D *target_;
};


class CameraController : public Qt3DExtras::QAbstractCameraController {

Q_OBJECT

public:
    explicit CameraController(Qt3DCore::QNode *parent = nullptr);

    ~CameraController() override;

    void registerObservable(Qt3DCore::QTransform *);

    void unregisterObservable(Qt3DCore::QTransform *);

    void switchObservable();

public slots:

    void handleKeyPress(Qt3DInput::QKeyEvent *event);

private:
    void moveCamera(const Qt3DExtras::QAbstractCameraController::InputState &state, float dt) override;

    void placeCamera();

    void updateAnimators();

    Qt3DInput::QKeyboardHandler *keyboardHandler_;
    bool arrowPressed_;
    float desiredPan_;
    float desiredTilt_;
    float azimuthalAngle_;
    float polarAngle_;
    float viewingDistance_;
    QVector3D viewCenter_;
    QVector3D viewCenterOffset_;
    QVector3D desiredViewCenter_;
    QVector3D desiredPos_;
    std::vector<std::shared_ptr<Interpolator>> animators_;
    std::list<Qt3DCore::QTransform *> observables_;
    std::list<Qt3DCore::QTransform *>::const_iterator observableIt_;
};


#endif //GS_MATTERHORN_CAMERACONTROLLER_H
