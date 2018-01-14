#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

#include "paquete_m.h"

using namespace omnetpp;

class Fuente: public cSimpleModule
{
    private:
        cMessage* sendEvent;
        simtime_t startTime;
        int seq;
        int numPaquetesEnviados;
        int totalPaquetes;

    public:
        virtual ~Fuente();

    protected:
        virtual void handleMessage(cMessage *msg) override;
        virtual void initialize() override;
        virtual paquete * generaPaquete();
};

Define_Module(Fuente);
Fuente::~Fuente(){
    //cancelAndDelete(packet);
}
void Fuente::initialize(){
    startTime=(simtime_t) par("interArrivalsTime");
    sendEvent = new cMessage("sendEvent");
    scheduleAt(startTime, sendEvent);
    seq=0;
    numPaquetesEnviados=0;
    totalPaquetes=(int)par("n_paquetes");
}
void Fuente::handleMessage(cMessage * msg){
    paquete *pqt = generaPaquete();
    send(pqt,"out");
    numPaquetesEnviados++;
    if (numPaquetesEnviados < totalPaquetes)
    {
        scheduleAt(simTime()+(simtime_t) par("interArrivalsTime"),sendEvent);
    }
}

paquete * Fuente::generaPaquete(){
    char nombrePaquete[15];
    sprintf(nombrePaquete,"msg-%d",seq++);
    paquete *msg = new paquete(nombrePaquete,0);
    msg -> setSeq(seq);
    msg -> setBitLength((int)par("packet_length"));
    msg -> setTimestamp(simTime());
    return msg;
}
