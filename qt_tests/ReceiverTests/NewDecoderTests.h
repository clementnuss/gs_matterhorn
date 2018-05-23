
#ifndef GS_MATTERHORN_NEWDECODERTESTS_H
#define GS_MATTERHORN_NEWDECODERTESTS_H


#include <QtCore/QObject>
#include <DataHandlers/Receiver/Decoder.h>
#include <QtTest/qtestcase.h>

class NewDecoderTests : public QObject {

Q_OBJECT

private:

private slots:

    void decoderRespondsToRSSIRequest() {

        Decoder decoder{"test_decoder"};
        // XBee answer for RSSI packet
        // del, length, ft, fID, ATcom, st, resp, chk
        //  7E  00  06  88  01  44  42  00  50  A0
        std::vector<uint8_t> rssiResponse{0x7E, 0x00, 0x06, 0x88, 0x01, 0x44, 0x42, 0x00, 0x50, 0xA0};

        QVERIFY(dynamic_cast<SeekingFrameStart *>(decoder.currentState()));

        for (size_t i = 0; i < rssiResponse.size(); i++) {
            decoder.processByte(rssiResponse[i]);

            if (i == 0)
                QVERIFY(dynamic_cast<ParsingATCommandHeader *>(decoder.currentState()));

            if (i == 2)
                QVERIFY(dynamic_cast<ParsingATCommandPayload *>(decoder.currentState()));

            if (i == 8)
                QVERIFY(dynamic_cast<ParsingATCommandChecksum *>(decoder.currentState()));
        }

        QVERIFY(decoder.datagramReady());

        Datagram d = decoder.retrieveDatagram();
        RSSIResponse *r = dynamic_cast<RSSIResponse *>(PayloadDataConverter::toATCommandResponse(d.payloadData_));
        RSSIResponse response = *r;
        delete r;

        QCOMPARE(response.frameID_, 0x01);
        QCOMPARE(response.command_, 0x4442);
        QCOMPARE(response.status_, 0x00);
        QCOMPARE(response.value_, 0x50);
    }

    void invalidFrameTypeAfterDelimiterResetsMachine() {

        Decoder decoder{"test_decoder"};
        // XBee answer for RSSI packet
        // del, length, ft, fID, ATcom, st, resp, chk
        //  7E  00  06  88  01  44  42  00  50  A0
        std::vector<uint8_t> rssiResponse{0x7E, 0x00, 0x06, 0x00, 0x01, 0x44, 0x42, 0x00, 0x50, 0xA0};

        QVERIFY(dynamic_cast<SeekingFrameStart *>(decoder.currentState()));

        for (size_t i = 0; i < 4; i++) {
            decoder.processByte(rssiResponse[i]);

            if (i == 0)
                QVERIFY(dynamic_cast<ParsingATCommandHeader *>(decoder.currentState()));

            if (i == 2)
                QVERIFY(dynamic_cast<ParsingATCommandPayload *>(decoder.currentState()));

        }

        QVERIFY(dynamic_cast<SeekingFrameStart *>(decoder.currentState()));
    }

};

#endif //GS_MATTERHORN_NEWDECODERTESTS_H
