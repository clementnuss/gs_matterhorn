
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
#include <3D/Interfaces/IObservable.h>
#include "3DVisualisationConstants.h"


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
    FloatInterpolator(const float objectiveValue, float *target) :
            objectiveValue_{objectiveValue},
            target_{target} {}

    void updateTarget() override {
        *target_ += CameraConstants::INTERPOLATION_STRENGTH * (objectiveValue_ - (*target_));
    }

private:
    float objectiveValue_;
    float *target_;
};


class QVector3DInterpolator : public Interpolator {
public:
    QVector3DInterpolator(const QVector3D &objectiveValue, QVector3D *target) :
            objectiveValue_{objectiveValue},
            target_{target} {}

    void updateTarget() override {
        *target_ += CameraConstants::INTERPOLATION_STRENGTH * (objectiveValue_ - (*target_));
    }

private:
    QVector3D objectiveValue_;
    QVector3D *target_;
};


class CameraController : public Qt3DExtras::QAbstractCameraController {

Q_OBJECT

public:
    explicit CameraController(Qt3DCore::QNode *parent = nullptr);

    ~CameraController() override;

    void registerObservable(IObservable *);

    void unregisterObservable(IObservable *);

    void setCameraViewCenter(const QVector3D &);

    void switchObservable();

public slots:

    void handleKeyPress(Qt3DInput::QKeyEvent *event);

private:
    void moveCamera(const Qt3DExtras::QAbstractCameraController::InputState &state, float dt) override;

    void placeCamera();

    void updateAnimators();

    Qt3DInput::QKeyboardHandler *keyboardHandler_;
    bool arrowPressed_;
    float azimuthalAngle_;
    float polarAngle_;
    float viewingDistance_;
    QVector3D viewCenter_;
    QVector3D viewCenterOffset_;
    std::vector<std::shared_ptr<Interpolator>> animators_;
    std::list<IObservable *> observables_;
    std::list<IObservable *>::const_iterator observableIt_;
};


#endif //GS_MATTERHORN_CAMERACONTROLLER_H
