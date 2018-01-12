#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

class Receptor : public cSimpleModule{
    public:
        virtual ~Receptor();

    protected:
        virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Receptor);

Receptor::~Receptor(){
    //cancelAndDelete(primerPaq);
}

void Receptor::handleMessage(cMessage *msg){
    EV << "Mensaje recibido "<< msg <<" rx-" << getIndex();// << msg << " on port number " << getIndex() << "]\n";
}
