
#ifndef GS_MATTERHORN_FORWARDRENDERER_H
#define GS_MATTERHORN_FORWARDRENDERER_H


#include <QObject>
#include <Qt3DExtras>
#include <Qt3DRender>
#include <Qt3DCore/QNode>

class ForwardRenderer : public Qt3DRender::QTechniqueFilter {
Q_OBJECT
public:
    explicit ForwardRenderer(Qt3DExtras::Qt3DWindow *view, Qt3DCore::QNode *parent = 0);

public slots:

private:
};


#endif //GS_MATTERHORN_FORWARDRENDERER_H
