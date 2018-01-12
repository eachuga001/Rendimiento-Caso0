#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

#include "paquete_m.h"

using namespace omnetpp;

class Fuente: public cSimpleModule
{
    private:
        int numSeq;
        paquete *packet;
        simtime_t start_time;
        //cChannel * txChannel;

    public:
        virtual ~Fuente();

    protected:
        virtual void handleMessage(cMessage *msg) override;
        virtual void initialize() override;

};

Define_Module(Fuente);
Fuente::~Fuente(){
    cancelAndDelete(packet);
}
void Fuente::initialize(){

    if (strcmp("Source",getName()) == 0) {
            //txChannel = gate("out")-> getTransmissionChannel();
        packet = new paquete();
            numSeq=0;
            start_time = 0;
            scheduleAt(start_time,packet);
            //char msgname[20];
            //sprintf(msgname, "Mensaje-%d Src-%d", numSeq++,getIndex());
            //msg1 = new cMessage(msgname);
            //scheduleAt(start_time,msg1);
        }

}
void Fuente::handleMessage(cMessage * msg){

        char msgname[20];
        sprintf(msgname, "Mensaje-%d", numSeq++);
        paquete *newPacket = new paquete(msgname,0);
        newPacket->setBitLength(1024);
        EV<< "Source-"<<getIndex() << " Enviando" << newPacket;
        send(newPacket,"out");
        scheduleAt(simTime()+exponential(0.5),packet);

        //cMessage *msg2 = new cMessage(msgname);

        //send(msg2,"out");
        //EV << "Forwarding mensaje" << msg2 ;
        //simtime_t txFinishTime = txChannel->getTransmissionFinishTime();//se pregunta al canal cuando ha acabado de transmitir
        //scheduleAt(txFinishTime+exponential(100),msg1);//Se envia cuando el cana ha acabado

}
