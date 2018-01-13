#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

class Node : public cSimpleModule
{
  protected:
    virtual void forwardMessage(cMessage *msg);
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual int getOutPort();
  private:
    std::vector<int> probabilities;
};

Define_Module(Node);

void Node::initialize()
{
    std::vector<int> distances; // holds result

    //Leer las probabilidades de salida por cada puerta de salida.
    const char *probs = par("probabilities");
    cStringTokenizer tokenizer(probs);
    while (tokenizer.hasMoreTokens())
        probabilities.push_back(atoi(tokenizer.nextToken()));
}

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
    int port = getOutPort();
    send(msg,"out",port);
    //EV<< "Nodo-"<<getIndex() << " Enviando por el puerto-"<<port;
    //EV << "Forwarding message " << port << " on port out[" << port << "]\n";
//    send(msg, "out",port);
}

int Node::getOutPort(){
    int port = 0;
    int rand = uniform (0,1);
    while (rand > probabilities[port])
    {
        port++;
        rand = rand - probabilities[port];
    }
    return port;
}
