
#ifndef GS_MATTERHORN_CAMERACONTROLLER_H
#define GS_MATTERHORN_CAMERACONTROLLER_H


#include <Qt3DExtras/QAbstractCameraController>
#include <Utilities/TimeUtils.h>
#include <ProgramConstants.h>
#include <iostream>
#include <memory>
#include <Qt3DInput/QKeyboardHandler>
#include <Qt3DInput/QKeyboardDevice>
#include "3DVisualisationConstants.h"

static float
linearInterpolation(const std::chrono::system_clock::time_point &initialTime, const float &v1, const float &v2) {
    float t = msecsBetween(initialTime, std::chrono::system_clock::now()) / CameraConstants::ANIMATION_DURATION;
    return (t * v2) + ((1.0f - t) * v1);
}

static float
easeInOutCubic(const std::chrono::system_clock::time_point &initialTime, const float &v1, const float &v2) {
    float t = msecsBetween(initialTime, std::chrono::system_clock::now()) / CameraConstants::ANIMATION_DURATION;
    t = t < .5 ? 4 * t * t * t : (t - 1) * (2 * t - 2) * (2 * t - 2) + 1; // From https://gist.github.com/gre/1650294
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
    FloatInterpolator(const float v1, const float v2, float *target) : Interpolator(),
                                                                       v1_{v1},
                                                                       v2_{v2},
                                                                       target_{target} {}

    void updateTarget() override {
        *target_ = easeInOutCubic(animationStartTime_, v1_, v2_);
    }

private:
    float v1_;
    float v2_;
    float *target_;
};


class QVector3DInterpolator : public Interpolator {
public:
    QVector3DInterpolator(const QVector3D &v1, const QVector3D &v2, QVector3D *target) : Interpolator(),
                                                                                         v1_{v1},
                                                                                         v2_{v2},
                                                                                         target_{target} {}

    void updateTarget() override {
        //*target_ = linearInterpolation(animationStartTime_, v1_, v2_);
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

public slots:

    void handleKeyPress(Qt3DInput::QKeyEvent *event);

private:
    void moveCamera(const Qt3DExtras::QAbstractCameraController::InputState &state, float dt) override;

    void updateAnimators();

    void moveToDesiredAttitude();

    Qt3DInput::QKeyboardHandler *keyboardHandler_;
    bool arrowPressed_;
    float desiredPan_;
    float desiredTilt_;
    QVector3D desiredViewCenter_;
    std::vector<std::shared_ptr<Interpolator>> animators_;
};


#endif //GS_MATTERHORN_CAMERACONTROLLER_H
