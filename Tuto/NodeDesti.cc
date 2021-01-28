/*
 * NodeDesti.cc
 *
 *  Created on: 4 dic. 2019
 *      Author: Adrian
 */
#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include "TerryMsg_m.h"

using namespace omnetpp;

class NodeDesti : public cSimpleModule
{
    protected:
        virtual void handleMessage(cMessage *msg) override;
};

Define_Module(NodeDesti);

void NodeDesti::handleMessage(cMessage *msg)
{
    TerryMsg *newMsg = (TerryMsg*) msg;
    cGate *arrivalGate = newMsg -> getArrivalGate();
    int arrGateInd;
    arrGateInd = arrivalGate -> getIndex();
    EV << "Packet arrived from gate " + std::to_string(arrGateInd) + "\n";
    char ruta[20];
           strcpy(ruta,newMsg->getRuta());
           sprintf(ruta,"%d",getId());
           newMsg->setRuta(ruta);
    if (newMsg -> getKind() == 1) { // 1: Packet
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

        }
    }
}






