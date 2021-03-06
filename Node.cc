#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

#include "sendMessage_m.h"
#include "paquete_m.h"

using namespace omnetpp;

class Node : public cSimpleModule
{
  protected:
    virtual void forwardMessage(cMessage *msg);
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual int getOutPort();
    virtual void sendPacket(cMessage* packet,int port);
    virtual void finish() override;
  private:
    std::vector<double> probabilities;
    cQueue* txQueues;
    sendMessage* sendEvents;
    enum state {idle=0,active=1};
    state* estado;
    double* realProbabilities;
    int receivedPackets = 0;
    int n_salidas;
};

Define_Module(Node);

void Node::initialize()
{
    //Leer las probabilidades de salida por cada puerta de salida.
    const char* probs = par("probabilities");
    EV << "probabilidades: " << probs << endl;
    cStringTokenizer tokenizer(probs," ");
    while (tokenizer.hasMoreTokens())
    {
        double probabilidad;
        std::string token = tokenizer.nextToken();
        int delimitador = token.find("/");
        if (delimitador != -1)
        {
            std::string numeradorString = token.substr(0,delimitador);
            double numerador = strtod(numeradorString.c_str(),NULL);
            std::string denominadorString = token.substr(delimitador+1,token.npos);
            double denominador = strtod(denominadorString.c_str(),NULL);
            probabilidad = numerador/denominador;
        }

        else
        {
            probabilidad = strtod(token.c_str(),NULL);
        }
        probabilities.push_back(probabilidad);
    }

    EV << "Las probabilidades del nodo son: ";
    for (int i = 0; i<probabilities.size();i++)
    {
        EV << i << " ";
    }
    EV << endl;

    n_salidas = (int) par("n_salidas");
    realProbabilities = new double [n_salidas];
    std::fill_n(realProbabilities, n_salidas, 0);
    txQueues = new cQueue [n_salidas];
    sendEvents = new sendMessage[n_salidas];
    estado = new state[n_salidas];
    for (int i=0; i < n_salidas;i++)
    {
        sendEvents[i].setPort(i);
        estado[i] = idle;
    }

}

void Node::handleMessage(cMessage *msg)
{

    // Message arrived.
    EV << "Message " << msg << " arrived.\n";
    //delete msg;
    // We need to forward the message.
    if (msg -> isSelfMessage())
    {
        sendMessage* sendEvent = check_and_cast <sendMessage *>(msg);
        int port = sendEvent->getPort();
        if (txQueues[port].isEmpty() == false)
        {
            cMessage *message = check_and_cast<cMessage *>(txQueues[port].pop());

            sendPacket(message, port);
        }

        else
        {
            estado[port] = idle;
        }
    }

    else
    {
        receivedPackets += 1;
        forwardMessage(msg);
    }

}

void Node::forwardMessage(cMessage *msg)
{
    int port = getOutPort();
    realProbabilities[port] += 1;
    if (estado[port] == idle)
    {
        estado[port]=active;
        sendPacket(msg,port);
    }

    else
    {
        txQueues[port].insert(msg);
    }
    //EV<< "Nodo-"<<getIndex() << " Enviando por el puerto-"<<port;
    //EV << "Forwarding message " << port << " on port out[" << port << "]\n";
//    send(msg, "out",port);
}

int Node::getOutPort(){
    int port = 0;
    double rand = uniform (0,1);
    while (rand >= probabilities[port])
    {
        rand = rand - probabilities[port];
        port++;
    }
    return port;
}

void Node::sendPacket(cMessage* packet,int port)
{
    paquete* pack = check_and_cast<paquete*>(packet);
    std::string nodesVisited = pack->getNodesVisited();
    if (nodesVisited.compare("")!=0)
    {
        nodesVisited.append("-");
    }
    const char* nodeName = (const char*) par("nodeName");
    nodesVisited.append(nodeName);
    pack->setNodesVisited(nodesVisited.c_str());
    send(packet -> dup(), "out",port);
    cChannel* txChannel = gate("out",port)->getChannel();
    simtime_t time = std::max(txChannel->getTransmissionFinishTime(), simTime());
    scheduleAt(time, &sendEvents[port]);
    delete (packet);
}

void Node::finish()
{
    EV << "Las probabilidades reales del nodo son: ";
    for (int i = 0; i<n_salidas;i++)
    {
        double probability = realProbabilities[i]/receivedPackets;
        EV << probability << " ";
    }
    EV << endl;
}
