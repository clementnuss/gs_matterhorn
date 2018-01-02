
#include "CameraController.h"
#include <Qt3DRender/QCamera>
#include <Qt3DExtras/Qt3DExtras>

CameraController::CameraController(Qt3DCore::QNode *parent)
        : Qt3DExtras::QAbstractCameraController(parent),
          arrowPressed_{false}, desiredPan_{0}, desiredTilt_{90}, desiredViewCenter_{0, 0, 0}, animators_{} {
}


CameraController::~CameraController() {
}


inline float clampInputs(float input1, float input2) {
    float axisValue = input1 + input2;
    return (axisValue < -1) ? -1 : (axisValue > 1) ? 1 : axisValue;
}

inline float zoomDistance(QVector3D firstPoint, QVector3D secondPoint) {
    return (secondPoint - firstPoint).lengthSquared();
}

inline float toDegrees(double x) {
    return static_cast<float>((180.0 / M_PI) * x);
}

void CameraController::moveCamera(const Qt3DExtras::QAbstractCameraController::InputState &state, float dt) {

    Qt3DRender::QCamera *cam = camera();


    if (cam == nullptr)
        return;

    // Mouse input
    if (state.rightMouseButtonActive) {
        if (state.leftMouseButtonActive) {
            if (zoomDistance(camera()->position(), cam->viewCenter()) >
                CameraConstants::ZOOMIN_LIMIT * CameraConstants::ZOOMIN_LIMIT) {
                // Dolly up to limit
                cam->translate(QVector3D(0, 0, (state.ryAxisValue * linearSpeed()) * dt),
                               cam->DontTranslateViewCenter);
            } else {
                cam->translate(QVector3D(0, 0, -0.5), cam->DontTranslateViewCenter);
            }
        } else {
            // Translate
            cam->translate(QVector3D(clampInputs(state.rxAxisValue, state.txAxisValue) * linearSpeed(),
                                           clampInputs(state.ryAxisValue, state.tyAxisValue) * linearSpeed(),
                                     0) * dt);
        }
        return;
    } else if (state.leftMouseButtonActive) {
        // Orbit
        cam->panAboutViewCenter((state.rxAxisValue * lookSpeed()) * dt, CameraConstants::UP_VECTOR);
        cam->tiltAboutViewCenter((state.ryAxisValue * lookSpeed()) * dt);

        QVector3D x = cam->viewVector().normalized();
        float tilt = toDegrees(std::acos(QVector3D::dotProduct(x, CameraConstants::UP_VECTOR)));
        desiredTilt_ = tilt;
        desiredPan_ = cam->transform()->rotationY();
    }

    // Keyboard Input
    if (state.altKeyActive) {
        // Orbit
        cam->panAboutViewCenter((state.txAxisValue * lookSpeed()) * dt, CameraConstants::UP_VECTOR);
        cam->tiltAboutViewCenter((state.tyAxisValue * lookSpeed()) * dt);
    } else if (state.shiftKeyActive) {
        if (zoomDistance(camera()->position(), cam->viewCenter()) >
            CameraConstants::ZOOMIN_LIMIT * CameraConstants::ZOOMIN_LIMIT) {
            // Dolly
            cam->translate(QVector3D(0, 0, state.tyAxisValue * linearSpeed() * dt),
                           cam->DontTranslateViewCenter);
        } else {
            cam->translate(QVector3D(0, 0, -0.5), cam->DontTranslateViewCenter);
        }
    } else {
        // Translate
        /*theCamera->translate(QVector3D(
                clampInputs(state.leftMouseButtonActive ? state.rxAxisValue : 0, state.txAxisValue) * linearSpeed(),
                clampInputs(state.leftMouseButtonActive ? state.ryAxisValue : 0, state.tyAxisValue) * linearSpeed(),
                state.tzAxisValue * linearSpeed()) * dt);*/

        if (state.txAxisValue == 0.0 && state.tyAxisValue == 0.0 && arrowPressed_) {
            arrowPressed_ = false;
        }

        if (!arrowPressed_) {

            QVector3D x = cam->viewVector().normalized();
            float tilt = toDegrees(std::acos(QVector3D::dotProduct(x, CameraConstants::UP_VECTOR)));

            std::cout << tilt << std::endl;

            if (state.txAxisValue != 0.0 || state.tyAxisValue != 0.0) {
                arrowPressed_ = true;
            }

            if (state.txAxisValue > 0.0) {
                animators_.push_back(std::make_shared<FloatInterpolator>(
                        cam->transform()->rotationY(),
                        cam->transform()->rotationY() + CameraConstants::PAN_STEP,
                        &desiredPan_)
                );

            } else if (state.txAxisValue < 0.0) {
                animators_.push_back(std::make_shared<FloatInterpolator>(
                        cam->transform()->rotationY(),
                        cam->transform()->rotationY() - CameraConstants::PAN_STEP,
                        &desiredPan_)
                );
            } else if (state.tyAxisValue > 0.0) {
                animators_.push_back(std::make_shared<FloatInterpolator>(
                        tilt,
                        tilt + CameraConstants::TILT_STEP,
                        &desiredTilt_)
                );
            } else if (state.tyAxisValue < 0.0) {
                animators_.push_back(std::make_shared<FloatInterpolator>(
                        tilt,
                        tilt - CameraConstants::TILT_STEP,
                        &desiredTilt_)
                );
            }
        }
    }

    updateAnimators();
}


void CameraController::updateAnimators() {

    bool updateOccured = !animators_.empty();

    for (auto animator : animators_) {
        animator->updateTarget();
    }

    // Remove animators which have finished
    animators_.erase(std::remove_if(
            animators_.begin(),
            animators_.end(),
            [](std::shared_ptr<Interpolator> i) { return i->animationIsFinished(); }), animators_.end());

    if (updateOccured)
        moveToDesiredAttitude();
}

void CameraController::moveToDesiredAttitude() {
    float currentPan = camera()->transform()->rotationY();
    float currentTilt = toDegrees(
            std::acos(QVector3D::dotProduct(camera()->viewVector().normalized(), CameraConstants::UP_VECTOR)));

    if (desiredPan_ != currentPan) {
        camera()->panAboutViewCenter(currentPan - desiredPan_, CameraConstants::UP_VECTOR);
    }
    if (desiredTilt_ != currentTilt) {
        camera()->tiltAboutViewCenter(desiredTilt_ - currentTilt);
    }
    if (desiredViewCenter_ != camera()->viewCenter()) {

    }

    std::cout << camera()->transform()->rotationY() << std::endl;
}
