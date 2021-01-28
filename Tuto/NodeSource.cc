/*
 * NodeSource.cc
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

class NodeSource : public cSimpleModule
{
    private:
    int muestras = 100;
    int seqnum = 0;
        double lambda = 2;
        double TamMedio;

    protected:
        virtual void initialize() override;
        virtual std::vector<double> Salidas(double lambda, int muestras);
        virtual std::vector<double> Tamano(double mu, int muestras);
        virtual TerryMsg* CreaMsg();
        virtual void handleMessage(cMessage *msg) override;

};

Define_Module(NodeSource);

void NodeSource::initialize() {
    TamMedio = (double) par("TamMedio");
    std::vector<double> tams = Tamano(TamMedio, muestras);

    std::vector<double> sals = Salidas(lambda, muestras);

    for(int i = 0; i < sals.size(); i++) {

        TerryMsg *newMsg = CreaMsg();
        newMsg -> setBitLength(tams[i]);
        scheduleAt(sals[i], newMsg);
    }
}

std::vector<double> NodeSource::Salidas(double lambda, int muestras) {

    std::uniform_real_distribution<double> randomReal(0.0, 1.0);
    std::default_random_engine genNum(time(NULL));
    std::vector<double> sals(muestras);
    for(int i = 0; i < sals.size(); i++) {
        double numAle = randomReal(genNum);
        double num = (-1/lambda) * log(numAle);
        if (i != 0)
            sals[i] = sals[i - 1] + num;
        else
            sals[i] = num;
    }
    return sals;
}

std::vector<double> NodeSource::Tamano(double TamMedio, int muestras) {
    std::uniform_real_distribution<double> randomReal(0.0, 1.0);
    std::default_random_engine genNum(time(NULL));
    std::vector<double> tams(muestras);
    for(int i = 0; i < tams.size(); i++) {
        double numAle = randomReal(genNum);
        double num = (-TamMedio) * log(numAle);
        tams[i] = num;
        //tams[i] = 1000;
    }
    return tams;
}

TerryMsg* NodeSource::CreaMsg() {
    std::string nameMsg = "paquete::" + std::to_string(getId()) + "::" + std::to_string(seqnum);
    char *charnameMsg = new char[nameMsg.length() + 1];
    strcpy(charnameMsg, nameMsg.c_str());
    TerryMsg *newMsg = new TerryMsg(charnameMsg);
    newMsg -> setCreado(true);
    newMsg -> setKind(1);
    newMsg -> setSequenceNumber(seqnum);
    newMsg -> setOrigin(getId());
    seqnum++;
    return newMsg;
}
void NodeSource::handleMessage(cMessage *msg) {

            TerryMsg *newMsg = check_and_cast<TerryMsg *> (msg);
            send(newMsg, "outPort");
        }


