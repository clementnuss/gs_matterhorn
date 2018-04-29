
#ifndef GS_MATTERHORN_TEXT3D_H
#define GS_MATTERHORN_TEXT3D_H


#include <Qt3DCore/QEntity>

enum class TextType {
    BOLD, LEGEND
};

class Text3D : public Qt3DCore::QEntity {
Q_OBJECT
public:
    explicit Text3D(const QString &text, TextType textType, const Qt3DRender::QCamera *const camera, const QVector3D &offsetToParent,
                    Qt3DCore::QNode *const parent);

public slots:

    void updateTransform();

private:
    const Qt3DRender::QCamera *const camera_;
    Qt3DCore::QTransform *transform_;
    QVector3D offset_;
    static const QVector3D basePosition_;
};

#endif //GS_MATTERHORN_TEXT3D_H
