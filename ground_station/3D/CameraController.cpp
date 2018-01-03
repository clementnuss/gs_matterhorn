
#include "CameraController.h"
#include <Qt3DRender/QCamera>
#include <Qt3DExtras/Qt3DExtras>

CameraController::CameraController(Qt3DCore::QNode *parent)
        : Qt3DExtras::QAbstractCameraController(parent),
          keyboardHandler_{new Qt3DInput::QKeyboardHandler(this)},
          arrowPressed_{false},
          desiredPan_{0},
          desiredTilt_{90},
          desiredViewCenter_{0, 0, 0},
          desiredPos_{0, 0, 0},
          animators_{},
          observables_{},
          observableIt_{observables_.begin()} {
    connect(keyboardHandler_, &Qt3DInput::QKeyboardHandler::pressed, this, &CameraController::handleKeyPress);
    keyboardHandler_->setSourceDevice(keyboardDevice());
    keyboardHandler_->setFocus(true);
}

CameraController::~CameraController() {
}

void CameraController::registerObservable(Qt3DCore::QTransform *o) {
    observables_.emplace_back(o);
}

void CameraController::unregisterObservable(Qt3DCore::QTransform *o) {
    observables_.remove(o);
}

void CameraController::switchObservable() {

    if (observables_.empty())
        return;

    if (++observableIt_ == observables_.end()) {
        observableIt_ = observables_.begin();
    }

    QVector3D observablePos = (*observableIt_)->translation();
    QVector3D obsToCam = camera()->transform()->translation() - observablePos;
    obsToCam = CameraConstants::ZOOMIN_DEFAULT * obsToCam.normalized();

    animators_.push_back(std::make_shared<QVector3DInterpolator>(
            camera()->transform()->translation(),
            observablePos + obsToCam,
            &desiredPos_,
            [this]() {
                if (desiredViewCenter_ != camera()->viewCenter())
                    camera()->setViewCenter(desiredViewCenter_);
            }
    ));

    animators_.push_back(std::make_shared<QVector3DInterpolator>(
            camera()->viewCenter(),
            observablePos,
            &desiredViewCenter_,
            [this]() {
                if (desiredPos_ != camera()->transform()->translation())
                    camera()->translateWorld(desiredPos_ - camera()->transform()->translation());
            }
    ));

}

void CameraController::handleKeyPress(Qt3DInput::QKeyEvent *event) {

    std::cout << "Qt3D captured: " << event->key() << "\n";

    switch (event->key()) {
        case Qt::Key_Space:
            switchObservable();
            break;
        default:
            break;
    }
}

inline float clampInputs(float input1, float input2) {
    float axisValue = input1 + input2;
    return (axisValue < -1) ? -1 : (axisValue > 1) ? 1 : axisValue;
}

inline float clampTilt(float tilt) {
    return tilt < CameraConstants::TILT_MIN ?
           CameraConstants::TILT_MIN :
           tilt > CameraConstants::TILT_MAX ?
           CameraConstants::TILT_MAX : tilt;
}

inline float zoomDistance(QVector3D firstPoint, QVector3D secondPoint) {
    return (secondPoint - firstPoint).lengthSquared();
}

inline float toDegrees(double x) {
    return static_cast<float>((180.0 / M_PI) * x);
}

void CameraController::moveCamera(const Qt3DExtras::QAbstractCameraController::InputState &state, float dt) {

    Qt3DRender::QCamera *cam = camera();

    if (observableIt_ != observables_.end()) {
        std::cout << (*observableIt_)->translation().x() << " " << (*observableIt_)->translation().x() << " "
                  << (*observableIt_)->translation().z() << " "
                  << std::endl;
    }


    if (cam == nullptr) {
        return;
    }



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
            cam->translate(QVector3D(0, 0, -0.5f), cam->DontTranslateViewCenter);
        }
    } else {

        if (state.txAxisValue == 0.0 && state.tyAxisValue == 0.0 && arrowPressed_) {
            arrowPressed_ = false;
        }

        if (!arrowPressed_) {

            QVector3D x = cam->viewVector().normalized();
            float tilt = toDegrees(std::acos(QVector3D::dotProduct(x, CameraConstants::UP_VECTOR)));

            if (state.txAxisValue != 0.0 || state.tyAxisValue != 0.0) {
                arrowPressed_ = true;
            }

            if (state.txAxisValue > 0.0) {
                animators_.push_back(std::make_shared<FloatInterpolator>(
                        cam->transform()->rotationY(),
                        cam->transform()->rotationY() - CameraConstants::PAN_STEP,
                        &desiredPan_,
                        [this]() {
                            float currentPan = camera()->transform()->rotationY();
                            if (desiredPan_ != currentPan)
                                camera()->panAboutViewCenter(currentPan - desiredPan_, CameraConstants::UP_VECTOR);
                        })
                );

            } else if (state.txAxisValue < 0.0) {
                animators_.push_back(std::make_shared<FloatInterpolator>(
                        cam->transform()->rotationY(),
                        cam->transform()->rotationY() + CameraConstants::PAN_STEP,
                        &desiredPan_,
                        [this]() {
                            float currentPan = camera()->transform()->rotationY();
                            if (desiredPan_ != currentPan)
                                camera()->panAboutViewCenter(currentPan - desiredPan_, CameraConstants::UP_VECTOR);
                        }));

            } else if (state.tyAxisValue > 0.0) {
                animators_.push_back(std::make_shared<FloatInterpolator>(
                        tilt,
                        clampTilt(tilt + CameraConstants::TILT_STEP),
                        &desiredTilt_,
                        [this]() {
                            float currentTilt = toDegrees(
                                    std::acos(QVector3D::dotProduct(camera()->viewVector().normalized(),
                                                                    CameraConstants::UP_VECTOR)));

                            if (desiredTilt_ != currentTilt)
                                camera()->tiltAboutViewCenter(desiredTilt_ - currentTilt);
                        }));

            } else if (state.tyAxisValue < 0.0) {
                animators_.push_back(std::make_shared<FloatInterpolator>(
                        tilt,
                        clampTilt(tilt - CameraConstants::TILT_STEP),
                        &desiredTilt_,
                        [this]() {
                            float currentTilt = toDegrees(
                                    std::acos(QVector3D::dotProduct(camera()->viewVector().normalized(),
                                                                    CameraConstants::UP_VECTOR)));

                            if (desiredTilt_ != currentTilt)
                                camera()->tiltAboutViewCenter(desiredTilt_ - currentTilt);
                        }));
            }
        }
    }

    updateAnimators();
}


void CameraController::updateAnimators() {

    for (const auto &animator : animators_)
        animator->updateTarget();

    // Remove animators which have finished
    animators_.erase(std::remove_if(
            animators_.begin(),
            animators_.end(),
            [](std::shared_ptr<Interpolator> i) { return i->animationIsFinished(); }), animators_.end());
}
