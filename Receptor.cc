#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

#include "paquete_m.h"

using namespace omnetpp;

class Receptor : public cSimpleModule{
    public:
        virtual ~Receptor();

    protected:
        virtual void handleMessage(cMessage *msg) override;
        virtual void finish() override;
    private:
        int receivedPackets = 0;
        simtime_t delayTotal = 0;
};

Define_Module(Receptor);

Receptor::~Receptor(){
    //cancelAndDelete(primerPaq);
}

void Receptor::handleMessage(cMessage *msg){
    paquete* pack = check_and_cast<paquete*>(msg);
    EV << "Mensaje recibido "<< msg <<" rx-" << getIndex() << endl;// << msg << " on port number " << getIndex() << "]\n";
    simtime_t delay = simTime() - msg->getTimestamp();
    EV << "El delay del paquete es: " << delay << endl;
    std::string ruta = pack->getNodesVisited();
    EV << "Ruta seguida por el paquete: " << ruta;
    delayTotal += delay;
    receivedPackets++;
    send(msg,"out");
}

void Receptor::finish()
{
    double delayTotalDouble = delayTotal.dbl();
    double delayMedio = delayTotalDouble / receivedPackets;
    EV << "Delay medio: " << delayMedio << endl;
    EV << "Numero de paquetes recibidos: " << receivedPackets << endl;
}
