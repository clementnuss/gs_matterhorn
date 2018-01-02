
#include "CameraController.h"
#include "3DVisualisationConstants.h"
#include <Qt3DRender/QCamera>
#include <Qt3DExtras/Qt3DExtras>

CameraController::CameraController(Qt3DCore::QNode *parent)
        : Qt3DExtras::QAbstractCameraController(parent),
          arrowPressed_{false} {
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

void CameraController::moveCamera(const Qt3DExtras::QAbstractCameraController::InputState &state, float dt) {

    Qt3DRender::QCamera *theCamera = camera();

    if (theCamera == nullptr)
        return;

    const QVector3D upVector(0.0f, 1.0f, 0.0f);

    // Mouse input
    if (state.rightMouseButtonActive) {
        if (state.leftMouseButtonActive) {
            if (zoomDistance(camera()->position(), theCamera->viewCenter()) >
                CameraConstants::ZOOMIN_LIMIT * CameraConstants::ZOOMIN_LIMIT) {
                // Dolly up to limit
                theCamera->translate(QVector3D(0, 0, (state.ryAxisValue * linearSpeed()) * dt),
                                     theCamera->DontTranslateViewCenter);
            } else {
                theCamera->translate(QVector3D(0, 0, -0.5), theCamera->DontTranslateViewCenter);
            }
        } else {
            // Translate
            theCamera->translate(QVector3D(clampInputs(state.rxAxisValue, state.txAxisValue) * linearSpeed(),
                                           clampInputs(state.ryAxisValue, state.tyAxisValue) * linearSpeed(),
                                           0) * dt);
        }
        return;
    } else if (state.leftMouseButtonActive) {
        // Orbit
        theCamera->panAboutViewCenter((state.rxAxisValue * lookSpeed()) * dt, upVector);
        theCamera->tiltAboutViewCenter((state.ryAxisValue * lookSpeed()) * dt);
    }

    // Keyboard Input
    if (state.altKeyActive) {
        // Orbit
        theCamera->panAboutViewCenter((state.txAxisValue * lookSpeed()) * dt, upVector);
        theCamera->tiltAboutViewCenter((state.tyAxisValue * lookSpeed()) * dt);
    } else if (state.shiftKeyActive) {
        if (zoomDistance(camera()->position(), theCamera->viewCenter()) >
            CameraConstants::ZOOMIN_LIMIT * CameraConstants::ZOOMIN_LIMIT) {
            // Dolly
            theCamera->translate(QVector3D(0, 0, state.tyAxisValue * linearSpeed() * dt),
                                 theCamera->DontTranslateViewCenter);
        } else {
            theCamera->translate(QVector3D(0, 0, -0.5), theCamera->DontTranslateViewCenter);
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

            if (state.txAxisValue != 0.0 || state.tyAxisValue != 0.0) {
                arrowPressed_ = true;
            }

            if (state.txAxisValue > 0.0) {
                theCamera->panAboutViewCenter(CameraConstants::PAN_STEP, upVector);
            } else if (state.txAxisValue < 0.0) {
                theCamera->panAboutViewCenter(-CameraConstants::PAN_STEP, upVector);
            } else if (state.tyAxisValue > 0.0) {
                theCamera->tiltAboutViewCenter(CameraConstants::TILT_STEP);
            } else if (state.tyAxisValue < 0.0) {
                theCamera->tiltAboutViewCenter(-CameraConstants::TILT_STEP);
            }
        }
    }
}

