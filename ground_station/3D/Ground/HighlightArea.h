
#ifndef GS_MATTERHORN_HIGHTLIGHTAREA_H
#define GS_MATTERHORN_HIGHTLIGHTAREA_H

#include <QObject>
#include <Qt3DExtras>
#include <Qt3DRender>
#include <Qt3DCore>

class HighlightArea : public Qt3DCore::QEntity {
Q_OBJECT
public:
    explicit HighlightArea(Qt3DRender::QParameter *heightParameter, Qt3DCore::QNode *parent);

    void updatePos(const QVector2D &pos);

public slots:

private:
    Qt3DCore::QTransform *transform_;
};


#endif //GS_MATTERHORN_HIGHTLIGHTAREA_H
