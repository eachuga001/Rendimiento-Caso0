#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

class Node : public cSimpleModule
{
  protected:
    virtual void forwardMessage(cMessage *msg);
    //virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;

    int setProbability(int numNode,cMessage *msg);
    int getRoute(double p);
};

Define_Module(Node);

/*void Txc10::initialize()
{
    if (getIndex() == 0) {
        // Boot the process scheduling the initial message as a self-message.
        char msgname[20];
        sprintf(msgname, "tic-%d", getIndex());
        cMessage *msg = new cMessage(msgname);
        scheduleAt(0.0, msg);
    }
}*/

void Node::handleMessage(cMessage *msg)
{

    // Message arrived.
    EV << "Message " << msg << " arrived.\n";
    //delete msg;
    // We need to forward the message.
    forwardMessage(msg);

}

void Node::forwardMessage(cMessage *msg)
{
    // In this example, we just pick a random gate to send it on.
    // We draw a random number between 0 and the size of gate `out[]'.
    //int n = gateSize("out");
    //int k = intuniform(0, n-1);

    //int port = getRoute(0.25);
    int port = setProbability(getIndex(),msg);

    EV<< "Nodo-"<<getIndex() << " Enviando por el puerto-"<<port;
    //EV << "Forwarding message " << port << " on port out[" << port << "]\n";
//    send(msg, "out",port);
}

int Node::setProbability(int numNode,cMessage *msg){
    int port;
    switch(numNode){
    case 0:
        port = getRoute(0.25);
        send(msg, "out",port);
        break;
    case 1:
        port = getRoute(0.33);
        send(msg, "out",port);
        break;
    case 2:
        port = 0;
        send(msg, "out",port);
        break;
    case 3:
        port=0;
        send(msg, "out",port);
        break;
    case 4:
        port = getRoute(0.5);
        send(msg, "out",port);
        break;
    }
    return port;
}

int Node::getRoute(double p){
    //Mayor probabilidad para el puerto 1
    int outPort;
    double random;
    random = uniform(0,1);
    //EV << "randomNum = " << random;
    if(random<p)
        outPort = 0;
    else
        outPort=1;

    //EV << " The port is: " << outPort;
    return outPort;
}
