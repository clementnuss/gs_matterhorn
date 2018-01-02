
#ifndef GS_MATTERHORN_CAMERACONTROLLER_H
#define GS_MATTERHORN_CAMERACONTROLLER_H


#include <Qt3DExtras/QAbstractCameraController>


class CameraController : public Qt3DExtras::QAbstractCameraController {

public:
    explicit CameraController(Qt3DCore::QNode *parent = nullptr);

    ~CameraController();

    float zoomInLimit() const;

    void setZoomInLimit(float zoomInLimit);


private:
    void moveCamera(const Qt3DExtras::QAbstractCameraController::InputState &state, float dt) override;

    bool arrowPressed_;
};


#endif //GS_MATTERHORN_CAMERACONTROLLER_H
