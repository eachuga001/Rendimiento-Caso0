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
        double totalBitLength = 0;
        simtime_t totalInterArrivalsTime = 0;

    public:
        virtual ~Fuente();

    protected:
        virtual void handleMessage(cMessage *msg) override;
        virtual void initialize() override;
        virtual paquete * generaPaquete();
        virtual void finish() override;
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
        simtime_t interArrivalsTime= exponential((simtime_t)par("interArrivalsTime"));
        totalInterArrivalsTime += interArrivalsTime;
        scheduleAt(simTime()+ interArrivalsTime,sendEvent);
    }
}

paquete * Fuente::generaPaquete(){
    char nombrePaquete[15];
    sprintf(nombrePaquete,"msg-%d",seq);
    paquete *msg = new paquete(nombrePaquete,0);
    msg -> setSeq(seq);
    double bitLength = exponential((double)par("packet_length"));
    totalBitLength += bitLength;
    msg -> setBitLength(bitLength);
    msg -> setTimestamp(simTime());
    msg ->setNodesVisited("");
    seq++;
    return msg;
}

void Fuente::finish()
{
    EV << "El interArrivalsTime medio es: " << totalInterArrivalsTime/numPaquetesEnviados << endl;
    EV << "La longitud media de paquetes es: " << totalBitLength/numPaquetesEnviados << endl;
}
