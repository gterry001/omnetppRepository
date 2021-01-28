/*
 * NodeInter.cc
 *
 *  Created on: 4 dic. 2019
 *      Author: Adrian
 */
#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include <random>
#include "TerryMsg_m.h"

using namespace omnetpp;

class NodeInter : public cSimpleModule
{
    private:

    //proof2
   // cQueue *cola[1];
   // cChannel *channel[1];
    //real and proof1
  cQueue *cola[2];
  cChannel *channel[2];
          TerryMsg *event;
          simtime_t timeout = 1;
    int muestras = 100;
        int seqnum = 0;
            double lambda = 2;
            double TamMedio;
    double prob;



    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
        virtual void sendPacket(int gateIndex,TerryMsg *msg);
        virtual void forwardNew(TerryMsg *newMsg);
        virtual void forwardCont(int gateIndex);
        virtual void MsgLost();

};

Define_Module(NodeInter);

void NodeInter::initialize() {


    prob = (double) par("probability");
    channel[1] = gate("outPort",1) -> getTransmissionChannel();
       channel[0] = gate("outPort",0) -> getTransmissionChannel();
    cola[0] = new cQueue("queueZero");
   cola[1] = new cQueue("colaOne");
       event = new TerryMsg("event");
       event->setKind(3);

}

void NodeInter::handleMessage(cMessage *msg)
{

    if(msg != event)
    {
    TerryMsg *newMsg = check_and_cast<TerryMsg *> (msg);
                cGate *arrivalGate = newMsg -> getArrivalGate();
                int arrGateInd;
                arrGateInd = arrivalGate -> getIndex();
                EV << "Packet info: " + std::to_string(arrGateInd) + "\n";
                WATCH(arrGateInd);
                if (newMsg -> getCreado()) {

                      EV << "own packet\n";
                       newMsg -> setCreado(false);
                      forwardNew(newMsg);
                     return;
                                      }
                  else
                       {
                           if (newMsg -> getKind() == 1) {
                               if (newMsg -> hasBitError()) {
                                   EV << "ERROR, notifying it\n";
                                   TerryMsg *error = new TerryMsg("error");
                                               error -> setKind(3);
                                               send(error, "outPort", arrGateInd);
                               }
                               else {
                                   EV << "No ERROR, notifying it\n";
                                               TerryMsg *ack = new TerryMsg("ACK");
                                               ack -> setKind(2);
                                               send(ack, "outPort", arrGateInd);
                                   forwardNew(newMsg);
                               }
                           }
                           else if (newMsg -> getKind() == 2) {
                               EV << "ACK from next node\n";
                               if (cola[arrGateInd] -> isEmpty())
                                   EV << "An acknowledgment arrived without corresponding packet in the queue\n";
                               else {

                                   cola[arrGateInd] -> pop();
                                   forwardCont(arrGateInd);
                               }
                           }
                           else {
                               EV << "Error notification arrived\n";
                               forwardCont(arrGateInd);
                           }
                       }

    }
   // else
  //      EV<<"RTX packet";
   //     MsgLost();


}

void NodeInter::sendPacket(int gateIndex,TerryMsg *msg) {
    if (channel[gateIndex] -> isBusy()) {
        EV << "The channel is busy\n";
    } else {

        TerryMsg *newMsg = check_and_cast<TerryMsg *> (msg -> dup());
        send(newMsg, "outPort", gateIndex);
       // scheduleAt(simTime()+timeout,event);
    }
}

void NodeInter::forwardCont(int gateIndex) {
    if (cola[gateIndex] -> isEmpty())
        EV << "Queue is empty\n";
    else {

        TerryMsg *newMsg = check_and_cast<TerryMsg *> (cola[gateIndex] -> front());
        sendPacket(gateIndex,newMsg);

    }
}
void NodeInter::MsgLost() {
    int gateIndex;
    if (cola[0] -> isEmpty())
        EV << "Queue is empty\n";
    else {

        TerryMsg *newMsg = check_and_cast<TerryMsg *> (cola[0] -> front());
        cGate *arrGate= newMsg->getArrivalGate();
        gateIndex = arrGate->getIndex();
        sendPacket(gateIndex,newMsg);
        EV<<"Packet already RTX";

    }
}

void NodeInter::forwardNew(TerryMsg *newMsg) {
    int gateIndex;
    double randomNumber = ((double) rand() / (RAND_MAX));
    if (randomNumber > prob)
        gateIndex = 0;
    else
        gateIndex = 1;

    if (cola[gateIndex] -> isEmpty()) {
        EV << "Queue is empty, send packet and wait\n";
        cola[gateIndex] -> insert(newMsg);
        sendPacket(gateIndex,newMsg);
    } else {
        EV << "Queue is not empty, add to back and wait\n";
        cola[gateIndex] -> insert(newMsg);
    }
}

