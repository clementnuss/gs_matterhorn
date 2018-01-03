
#include "CameraController.h"
#include <Qt3DRender/QCamera>
#include <Qt3DExtras/Qt3DExtras>

CameraController::CameraController(Qt3DCore::QNode *parent)
        : Qt3DExtras::QAbstractCameraController(parent),
          keyboardHandler_{new Qt3DInput::QKeyboardHandler(this)},
          arrowPressed_{false},
          desiredPan_{0},
          desiredTilt_{90},
          azimuthalAngle_{0},
          polarAngle_{M_PI / 2.0f},
          viewCenter_{0, 0, 0},
          viewCenterOffset_{0, 0, 0},
          viewingDistance_{5000.0f},
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
    obsToCam = CameraConstants::VIEWING_DISTANCE_DEFAULT * obsToCam.normalized();

    animators_.push_back(std::make_shared<QVector3DInterpolator>(
            camera()->transform()->translation(),
            observablePos + obsToCam,
            &desiredPos_));

    animators_.push_back(std::make_shared<QVector3DInterpolator>(
            camera()->viewCenter(),
            observablePos,
            &desiredViewCenter_));

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

inline float clampViewingDistance(float vd) {
    return vd < CameraConstants::VIEWING_DISTANCE_MIN ?
           CameraConstants::VIEWING_DISTANCE_MIN :
           vd > CameraConstants::VIEWING_DISTANCE_MAX ?
           CameraConstants::VIEWING_DISTANCE_MAX : vd;
}

inline float clampPolarAngle(float tilt) {
    return tilt < CameraConstants::POLAR_MIN ?
           CameraConstants::POLAR_MIN :
           tilt > CameraConstants::POLAR_MAX ?
           CameraConstants::POLAR_MAX : tilt;
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
            viewingDistance_ = clampViewingDistance(viewingDistance_ + state.ryAxisValue * 300 * linearSpeed() * dt);
        } else {
            // Translate
            QVector3D rightVector = QVector3D::crossProduct(
                    cam->viewVector().normalized(),
                    cam->upVector().normalized()
            ).normalized();

            QVector3D upVector = QVector3D::crossProduct(
                    rightVector.normalized(),
                    cam->viewVector().normalized()
            ).normalized();

            viewCenterOffset_ +=
                    rightVector * state.rxAxisValue * linearSpeed() + upVector * state.ryAxisValue * linearSpeed();
        }
    } else if (state.leftMouseButtonActive) {
        // Orbit
        azimuthalAngle_ += state.rxAxisValue * lookSpeed() * dt;
        polarAngle_ = clampPolarAngle(polarAngle_ + state.ryAxisValue * lookSpeed() * dt);
    }

    if (state.txAxisValue == 0.0 && state.tyAxisValue == 0.0 && arrowPressed_) {
        arrowPressed_ = false;
    }

    if (!arrowPressed_) {

        if (state.txAxisValue != 0.0 || state.tyAxisValue != 0.0) {
            arrowPressed_ = true;
        }

        if (state.txAxisValue > 0.0) {
            animators_.push_back(std::make_shared<FloatInterpolator>(
                    azimuthalAngle_,
                    azimuthalAngle_ + CameraConstants::AZIMUTH_STEP,
                    &azimuthalAngle_)
            );
        } else if (state.txAxisValue < 0.0) {
            animators_.push_back(std::make_shared<FloatInterpolator>(
                    azimuthalAngle_,
                    azimuthalAngle_ - CameraConstants::AZIMUTH_STEP,
                    &azimuthalAngle_)
            );
        } else if (state.tyAxisValue > 0.0) {
            animators_.push_back(std::make_shared<FloatInterpolator>(
                    polarAngle_,
                    clampPolarAngle(polarAngle_ + CameraConstants::POLAR_STEP),
                    &polarAngle_)
            );

        } else if (state.tyAxisValue < 0.0) {
            animators_.push_back(std::make_shared<FloatInterpolator>(
                    polarAngle_,
                    clampPolarAngle(polarAngle_ - CameraConstants::POLAR_STEP),
                    &polarAngle_)
            );
        }
    }

    updateAnimators();
    placeCamera();
}

void CameraController::placeCamera() {

    float x = viewingDistance_ * std::sin(polarAngle_) * std::cos(azimuthalAngle_);
    float z = viewingDistance_ * std::sin(polarAngle_) * std::sin(azimuthalAngle_);
    float y = viewingDistance_ * std::cos(polarAngle_);

    camera()->setPosition(viewCenter_ + viewCenterOffset_ + QVector3D{x, y, z});
    camera()->setViewCenter(viewCenter_ + viewCenterOffset_);
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
