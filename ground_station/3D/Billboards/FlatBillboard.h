
#ifndef GS_MATTERHORN_FLATBILLBOARD_H
#define GS_MATTERHORN_FLATBILLBOARD_H


#include <Qt3DCore/QTransform>
#include <Qt3DCore/QEntity>

class FlatBillboard : public Qt3DCore::QEntity {
Q_OBJECT
public:
    explicit FlatBillboard(const QString &texture, float width, float height, Qt3DCore::QNode *parent);

private:
    Qt3DCore::QTransform *transform_;
};


#endif //GS_MATTERHORN_FLATBILLBOARD_H
