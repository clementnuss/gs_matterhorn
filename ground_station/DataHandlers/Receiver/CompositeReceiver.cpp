
#include <DataStructures/datastructs.h>
#include <iostream>
#include "CompositeReceiver.h"

CompositeReceiver::CompositeReceiver(std::unique_ptr<IReceiver> primaryReceiver, std::unique_ptr<IReceiver> backupReceiver, const unsigned int limitLag) :
        primaryReceiver_{std::move(primaryReceiver)},
        backupReceiver_{std::move(backupReceiver)},
        primaryQueue_{},
        backupQueue_{},
        mergeQueue_{},
        limitLag_{limitLag},
        seqMap_{} {

    for (size_t i = 0; i < FlyableType::Count; i++) {

        seqMap_.emplace(
                std::make_pair(static_cast<FlyableType>(i), 1)
        );

    }
}

template<class T>
std::list<std::unique_ptr<T>>
consumeAndMove(std::list<std::unique_ptr<T>> *q) {
    std::list<std::unique_ptr<T>> l;

    while (!q->empty()) {
        l.emplace_back(std::move(q->front()));
        q->pop_front();
    }

    return l;
}

std::list<std::unique_ptr<DataPacket>>
CompositeReceiver::pollData() {

    primaryQueue_.splice(primaryQueue_.end(), primaryReceiver_->pollData());
    backupQueue_.splice(backupQueue_.end(), backupReceiver_->pollData());

    mergePacketQueuesStep();

    return consumeAndMove(&mergeQueue_);
}

inline bool
isNotFresh(std::list<std::unique_ptr<DataPacket>> *q, const uint32_t &index, const FlyableType &fType) {
    return !q->empty() && q->front()->sequenceNumber_ < index && q->front()->flyableType_ == fType;
}

void
CompositeReceiver::mergePacketQueuesStep() {

    std::unique_ptr<DataPacket> *e1;
    std::unique_ptr<DataPacket> *e2;
    std::unique_ptr<DataPacket> *elementSelector;
    std::list<std::unique_ptr<DataPacket>> *queueSelector;

    // If both queue contain elements retrieve as much as possible
    while (!primaryQueue_.empty() && !backupQueue_.empty()) {

        e1 = &primaryQueue_.front();
        e2 = &backupQueue_.front();

        // Select element with minimal sequence number
        if ((*e1)->sequenceNumber_ <= (*e2)->sequenceNumber_) {
            elementSelector = e1;
            queueSelector = &primaryQueue_;
        } else {
            elementSelector = e2;
            queueSelector = &backupQueue_;
        }


        // Update sequence index
        FlyableType fType = (*elementSelector)->flyableType_;
        uint32_t seqNum = (*elementSelector)->sequenceNumber_;

        // Only considers packets more recent that the last considered
        if (seqNum >= seqMap_[fType]) {
            seqMap_[fType] = seqNum + 1;

            // Add element to merge queue
            mergeQueue_.push_back(std::move(*elementSelector));
        }

        // Remove from selected queue
        queueSelector->pop_front();

        // Trim both queues to remove potential duplicate packets
        if (isNotFresh(&primaryQueue_, seqMap_[fType], fType))
            primaryQueue_.pop_front();

        if (isNotFresh(&backupQueue_, seqMap_[fType], fType))
            backupQueue_.pop_front();

    }


    // Check if at least one of the queues can be used
    elementSelector = nullptr;
    uint32_t elementSeqNum{0};

    while (!primaryQueue_.empty() || !backupQueue_.empty()) {
        if (!primaryQueue_.empty()) {
            elementSelector = &primaryQueue_.front();
            queueSelector = &primaryQueue_;
        } else if (!backupQueue_.empty()) {
            elementSelector = &backupQueue_.front();
            queueSelector = &backupQueue_;
        }

        FlyableType fType = (*elementSelector)->flyableType_;
        elementSeqNum = (*elementSelector)->sequenceNumber_;

        // Is the element the next element we are seeking ?
        if (elementSeqNum < seqMap_[fType]) {

            //std::cerr << "The element sequence number was less than the index, this means a packet arrived out of order ! " << std::endl;
            queueSelector->pop_front();

        } else if (elementSeqNum == seqMap_[fType]) {

            seqMap_[fType]++;
            addToMergeQueueAndPop(elementSelector, queueSelector);

        } else {

            if (queueSelector->size() < limitLag_) {
                // If we haven't received much packets we wait until the new poll call, maybe the backup receiver will have it by then.
                return;
            } else {
                //Otherwise we consider that packet to be lost and move forward
                seqMap_[fType] = (*elementSelector)->sequenceNumber_ + 1;
                addToMergeQueueAndPop(elementSelector, queueSelector);
            }
        }
    };
}

void
CompositeReceiver::startup() {
    primaryReceiver_->startup();
    backupReceiver_->startup();
}

bool
CompositeReceiver::isReplayReceiver() {
    return false;
}

void
CompositeReceiver::sendCommand(const uint8_t *, size_t) {

}

void
CompositeReceiver::addToMergeQueueAndPop(std::unique_ptr<DataPacket> *packet, std::list<std::unique_ptr<DataPacket>> *queue) {
    mergeQueue_.push_back(std::move(*packet));
    queue->pop_front();
}

CompositeReceiver::~CompositeReceiver() = default;

