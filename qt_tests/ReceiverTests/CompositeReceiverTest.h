
#ifndef GS_MATTERHORN_COMPOSITERECEIVERTEST_H
#define GS_MATTERHORN_COMPOSITERECEIVERTEST_H

#include <QtTest/QtTest>
#include "DummyReceiver.h"
#include <DataHandlers/Receiver/CompositeReceiver.h>


static std::vector<DataPacket>
makePacketVector(uint32_t from, uint32_t to, FlyableType type = FlyableType::ROCKET) {


    std::vector<DataPacket> v{};

    for (uint32_t i = from; i < to + 1; i++) {
        v.emplace_back(DataPacket{0, i, type});
    }

    return v;
}

static std::vector<DataPacket>
makePacketVector(uint32_t seqNum, FlyableType type = FlyableType::ROCKET) {

    std::vector<DataPacket> v{};
    v.emplace_back(DataPacket{0, seqNum, type});
    return v;
}

static void
assertSequenceEquality(const std::list<std::unique_ptr<DataPacket>> &l, const std::vector<uint32_t> &v) {

    QCOMPARE(l.size(), v.size());

    auto it = l.begin();

    for (size_t i = 0; i < v.size(); i++) {
        QCOMPARE((*it++)->sequenceNumber_, v[i]);
    }
}

static void
assertMultiSequenceEquality(const std::list<std::unique_ptr<DataPacket>> &l, const std::vector<std::pair<uint32_t, FlyableType>> &v) {

    QCOMPARE(l.size(), v.size());

    auto it = l.begin();

    for (size_t i = 0; i < v.size(); i++) {
        QCOMPARE((*it)->sequenceNumber_, v[i].first);
        QCOMPARE((*it++)->flyableType_, v[i].second);
    }
}

class CompositeReceiverTests : public QObject {

Q_OBJECT

private:
    std::unique_ptr<CompositeReceiver> compositeReceiver;

    DummyReceiver *primaryReceiver;
    DummyReceiver *backupReceiver;
    unsigned int limitLag;
    std::vector<FlyableType> typeList;

private slots:

    void initTestCase() {
        limitLag = 10;
        typeList = {FlyableType::ROCKET, FlyableType::PAYLOAD};
    }

    void init() {
        compositeReceiver = std::make_unique<CompositeReceiver>(
                std::move(std::make_unique<DummyReceiver>()),
                std::move(std::make_unique<DummyReceiver>()),
                limitLag
        );

        // Get a handle on the receivers which are unique_ptr's
        primaryReceiver = dynamic_cast<DummyReceiver *>(&(*compositeReceiver->primaryReceiver_));
        backupReceiver = dynamic_cast<DummyReceiver *>(&(*compositeReceiver->backupReceiver_));
    }

    void monoVehicleSimpleMergeStepIsCorrect() {

        for (auto flyableType : typeList) {
            primaryReceiver->registerPackets(makePacketVector(1, 4, flyableType));
            backupReceiver->registerPackets(makePacketVector(2, 4, flyableType));
            assertSequenceEquality(compositeReceiver->pollData(), {1, 2, 3, 4});
        }

    }

    void multiVehicleSimpleMergeStepIsCorrect() {
        primaryReceiver->registerPackets(makePacketVector(1, FlyableType::ROCKET));
        backupReceiver->registerPackets(makePacketVector(2, FlyableType::ROCKET));
        primaryReceiver->registerPackets(makePacketVector(1, FlyableType::PAYLOAD));
        backupReceiver->registerPackets(makePacketVector(3, FlyableType::ROCKET));
        backupReceiver->registerPackets(makePacketVector(2, FlyableType::PAYLOAD));
        backupReceiver->registerPackets(makePacketVector(4, FlyableType::ROCKET));
        assertMultiSequenceEquality(compositeReceiver->pollData(),
                                    {{1, FlyableType::ROCKET},
                                     {1, FlyableType::PAYLOAD},
                                     {2, FlyableType::ROCKET},
                                     {3, FlyableType::ROCKET},
                                     {2, FlyableType::PAYLOAD},
                                     {4, FlyableType::ROCKET}});
    }

    void monoVehicleAlternatingMergeStepIsCorrect() {

        for (auto flyableType : typeList) {
            std::vector<uint32_t> v;
            for (uint32_t i = 1; i < 1000; i += 2) {
                primaryReceiver->registerPackets(makePacketVector(i, flyableType));
                backupReceiver->registerPackets(makePacketVector(i + 1, flyableType));
                v.push_back(i);
                v.push_back(i + 1);
            }

            assertSequenceEquality(compositeReceiver->pollData(), v);
        }
    }

    void multiVehicleAlternatingMergeStepIsCorrect() {

        std::vector<std::pair<uint32_t, FlyableType>> v;
        for (uint32_t i = 1; i < 1000; i += 2) {
            primaryReceiver->registerPackets(makePacketVector(i, FlyableType::ROCKET));
            backupReceiver->registerPackets(makePacketVector(i, FlyableType::PAYLOAD));
            primaryReceiver->registerPackets(makePacketVector(i + 1, FlyableType::PAYLOAD));
            backupReceiver->registerPackets(makePacketVector(i + 1, FlyableType::ROCKET));
            v.push_back(std::make_pair(i, FlyableType::ROCKET));
            v.push_back(std::make_pair(i, FlyableType::PAYLOAD));
            v.push_back(std::make_pair(i + 1, FlyableType::PAYLOAD));
            v.push_back(std::make_pair(i + 1, FlyableType::ROCKET));
        }

        assertMultiSequenceEquality(compositeReceiver->pollData(), v);

    }

    void monoVehicleWaitingMergeStepIsCorrect() {

        for (auto flyableType : typeList) {
            primaryReceiver->registerPackets(makePacketVector(1, flyableType));
            backupReceiver->registerPackets(makePacketVector(1, flyableType));
            assertSequenceEquality(compositeReceiver->pollData(), {1});

            backupReceiver->registerPackets(makePacketVector(2, flyableType));
            assertSequenceEquality(compositeReceiver->pollData(), {2});

            // Packet 3 gets lost
            primaryReceiver->registerPackets(makePacketVector(4, 6, flyableType));
            assertSequenceEquality(compositeReceiver->pollData(), {});

            // Feeding backup receiver allows retrieval, should assume 3 is lost definitively
            backupReceiver->registerPackets(makePacketVector(7, 8, flyableType));
            assertSequenceEquality(compositeReceiver->pollData(), {4, 5, 6, 7, 8});
        }
    }

    void multiVehicleWaitingMergeStepIsCorrect() {

        primaryReceiver->registerPackets(makePacketVector(1, FlyableType::ROCKET));
        backupReceiver->registerPackets(makePacketVector(1, FlyableType::ROCKET));
        assertSequenceEquality(compositeReceiver->pollData(), {1});

        backupReceiver->registerPackets(makePacketVector(2, FlyableType::ROCKET));
        assertSequenceEquality(compositeReceiver->pollData(), {2});

        // Packet 3 gets lost
        primaryReceiver->registerPackets(makePacketVector(4, 6, FlyableType::ROCKET));
        assertSequenceEquality(compositeReceiver->pollData(), {});

        // Feeding backup receiver allows retrieval, should assume 3 is lost definitively
        backupReceiver->registerPackets(makePacketVector(1, 4, FlyableType::PAYLOAD));
        assertMultiSequenceEquality(compositeReceiver->pollData(),
                                    {{1, FlyableType::PAYLOAD},
                                     {2, FlyableType::PAYLOAD},
                                     {3, FlyableType::PAYLOAD},
                                     {4, FlyableType::ROCKET},
                                     {4, FlyableType::PAYLOAD},
                                     {5, FlyableType::ROCKET},
                                     {6, FlyableType::ROCKET}});

    }

    void monoVehicleLagIsLimited() {
        for (auto flyableType : typeList) {
            primaryReceiver->registerPackets(makePacketVector(1, flyableType));
            assertSequenceEquality(compositeReceiver->pollData(), {1});

            // Packet 2 gets lost
            primaryReceiver->registerPackets(makePacketVector(3, limitLag + 1, flyableType));

            assertSequenceEquality(compositeReceiver->pollData(), {});

            // Adding new packet causes fast-forward
            primaryReceiver->registerPackets(makePacketVector(12, flyableType));
            assertSequenceEquality(compositeReceiver->pollData(), {3, 4, 5, 6, 7, 8, 9, 10, 11, 12});
        }
    }

    void multiVehicleLagIsLimited() {
        primaryReceiver->registerPackets(makePacketVector(1, FlyableType::ROCKET));
        assertSequenceEquality(compositeReceiver->pollData(), {1});

        // Packet 2 gets lost
        primaryReceiver->registerPackets(makePacketVector(3, limitLag / 2 + 1, FlyableType::ROCKET));
        primaryReceiver->registerPackets(makePacketVector(1, limitLag / 2, FlyableType::PAYLOAD));

        assertSequenceEquality(compositeReceiver->pollData(), {});

        // Adding new packet causes fast-forward
        primaryReceiver->registerPackets(makePacketVector(7, FlyableType::ROCKET));
        assertMultiSequenceEquality(compositeReceiver->pollData(),
                                    {{3, FlyableType::ROCKET},
                                     {4, FlyableType::ROCKET},
                                     {5, FlyableType::ROCKET},
                                     {6, FlyableType::ROCKET},
                                     {1, FlyableType::PAYLOAD},
                                     {2, FlyableType::PAYLOAD},
                                     {3, FlyableType::PAYLOAD},
                                     {4, FlyableType::PAYLOAD},
                                     {5, FlyableType::PAYLOAD},
                                     {7, FlyableType::ROCKET}});

    }

    void monoVehiclePacketWithLowerSequenceNumberGetsDiscarded() {
        for (auto flyableType : typeList) {
            // Test when only one receiver has data
            primaryReceiver->registerPackets(makePacketVector(1, 5, flyableType));
            primaryReceiver->registerPackets(makePacketVector(2, 5, flyableType));
            primaryReceiver->registerPackets(makePacketVector(1, 4, flyableType));
            primaryReceiver->registerPackets(makePacketVector(5, 10, flyableType));
            primaryReceiver->registerPackets(makePacketVector(6, 8, flyableType));
            assertSequenceEquality(compositeReceiver->pollData(), {1, 2, 3, 4, 5, 6, 7, 8, 9, 10});

            // Test when both receiver have data
            primaryReceiver->registerPackets(makePacketVector(11, 13, flyableType));
            primaryReceiver->registerPackets(makePacketVector(16, 19, flyableType));
            backupReceiver->registerPackets(makePacketVector(13, 15, flyableType));
            backupReceiver->registerPackets(makePacketVector(13, 18, flyableType));
            backupReceiver->registerPackets(makePacketVector(17, 20, flyableType));
            assertSequenceEquality(compositeReceiver->pollData(), {11, 12, 13, 14, 15, 16, 17, 18, 19, 20});
        }
    }

    void multiVehiclePacketWithLowerSequenceNumberGetsDiscarded() {

        primaryReceiver->registerPackets(makePacketVector(1, 4, FlyableType::ROCKET));
        primaryReceiver->registerPackets(makePacketVector(1, 4, FlyableType::PAYLOAD));
        primaryReceiver->registerPackets(makePacketVector(1, 4, FlyableType::ROCKET));
        primaryReceiver->registerPackets(makePacketVector(1, 4, FlyableType::PAYLOAD));
        backupReceiver->registerPackets(makePacketVector(2, 5, FlyableType::ROCKET));
        backupReceiver->registerPackets(makePacketVector(2, 5, FlyableType::PAYLOAD));
        backupReceiver->registerPackets(makePacketVector(2, 5, FlyableType::ROCKET));
        backupReceiver->registerPackets(makePacketVector(2, 5, FlyableType::PAYLOAD));
        assertMultiSequenceEquality(compositeReceiver->pollData(),
                                    {{1, FlyableType::ROCKET},
                                     {2, FlyableType::ROCKET},
                                     {3, FlyableType::ROCKET},
                                     {4, FlyableType::ROCKET},
                                     {1, FlyableType::PAYLOAD},
                                     {2, FlyableType::PAYLOAD},
                                     {3, FlyableType::PAYLOAD},
                                     {4, FlyableType::PAYLOAD},
                                     {5, FlyableType::ROCKET},
                                     {5, FlyableType::PAYLOAD}});

    }
};

#endif //GS_MATTERHORN_COMPOSITERECEIVERTEST_H
