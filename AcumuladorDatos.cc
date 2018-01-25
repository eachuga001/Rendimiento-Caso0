#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

#include "paquete_m.h"

using namespace omnetpp;

class AcumuladorDatos : public cSimpleModule{
    public:
        virtual ~AcumuladorDatos();

    protected:
        virtual void handleMessage(cMessage *msg) override;
        virtual void finish() override;
        virtual void initialize() override;
    private:
        int receivedPackets = 0;
        simtime_t delayTotal = 0;
        struct rutaPaquetes
        {
            simtime_t delay;
            int numPaquetes;
        };
        std::map<std::string,rutaPaquetes> mapa;

        cLongHistogram delayStats;
        cOutVector delayVector;
};

Define_Module(AcumuladorDatos);

AcumuladorDatos::~AcumuladorDatos(){
    //cancelAndDelete(primerPaq);
}

void AcumuladorDatos::initialize()
{
    delayVector.setName("Delay");
}

void AcumuladorDatos::handleMessage(cMessage *msg){
    paquete* pack = check_and_cast<paquete*>(msg);
    simtime_t delay = simTime() - msg->getTimestamp();
    std::string ruta = pack->getNodesVisited();

    std::map<std::string,struct rutaPaquetes>::iterator it;
    it = mapa.find(ruta);
    if (it != mapa.end())
    {
        it->second.delay += delay;
        it->second.numPaquetes += 1;
    }

    else
    {
       struct rutaPaquetes rp = {delay,1};
       mapa[ruta]=rp;
    }
    delayTotal += delay;
    receivedPackets++;
    delayVector.record(delay);
    delete(msg);
}

void AcumuladorDatos::finish()
{
    double delayTotalDouble = delayTotal.dbl();
    double delayMedio = delayTotalDouble / receivedPackets;
    EV << "Delay medio de todos los receptores: " << delayMedio << endl;
    EV << "Numero de paquetes recibidos: " << receivedPackets << endl;

    for (std::map<std::string,struct rutaPaquetes>::iterator it = mapa.begin();it!=mapa.end();++it)
    {
        simtime_t delayMedioRuta = it->second.delay / it->second.numPaquetes;
        EV << "El delay medio de la ruta " << it->first << " es " << delayMedioRuta << endl;
    }
}
