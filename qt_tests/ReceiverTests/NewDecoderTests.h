
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

        QCOMPARE(DecodingState::SEEKING_FRAMESTART, decoder.currentState());

        for (size_t i = 0; i < rssiResponse.size(); i++) {
            decoder.processByte(rssiResponse[i]);

            if (i == 0)
                QCOMPARE(DecodingState::PARSING_AT_COMMAND_HEADER, decoder.currentState());

            if (i == 2)
                QCOMPARE(DecodingState::PARSING_AT_COMMAND, decoder.currentState());

            if (i == 8)
                QCOMPARE(DecodingState::PARSING_AT_COMMAND_CHECKSUM, decoder.currentState());
        }

        QVERIFY(decoder.datagramReady());
    }
};

#endif //GS_MATTERHORN_NEWDECODERTESTS_H
