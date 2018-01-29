#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

#include "sendMessage_m.h"
#include "paquete_m.h"
#include "ack_m.h"

using namespace omnetpp;

class NodeSW : public cSimpleModule
{
  protected:
    virtual void forwardMessage(cMessage *msg);
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual int getOutPort();
    virtual void sendPacket(cMessage* packet,int port);
    virtual void sendAck(cMessage* packet);
    virtual void finish() override;
  private:
    std::vector<double> probabilities;
    cQueue* txQueues;
    sendMessage* timeoutMessages;
    enum state {idle=0,active=1};
    state* estado;
    double* realProbabilities;
    int receivedPackets = 0;
    int n_salidas;
    cMessage** currentMessage;
};

Define_Module(NodeSW);

void NodeSW::initialize()
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
    timeoutMessages = new sendMessage[n_salidas];
    estado = new state[n_salidas];
    for (int i=0; i < n_salidas;i++)
    {
        timeoutMessages[i].setPort(i);
        estado[i] = idle;
    }

    currentMessage = new cMessage *[n_salidas];
}

void NodeSW::handleMessage(cMessage *msg)
{
    EV << "Message " << msg << " arrived.\n";
    if (msg -> isSelfMessage())
    {
        sendMessage* timeoutMessage = check_and_cast <sendMessage *>(msg);
        int port = timeoutMessage->getPort();
        sendPacket(currentMessage[port],port);
    }

    else if (strcmp(msg-> getName(),"ack") == 0)
    {
        cGate* gate = msg->getArrivalGate();
        int port = gate -> getIndex();
        cancelEvent(&timeoutMessages[port]);
        if (txQueues[port].isEmpty())
        {
            estado[port] = idle;
        }
        else
        {
            currentMessage[port] = check_and_cast<cMessage*>(txQueues[port].pop());
            sendPacket(currentMessage[port], port);
        }
    }

    else
    {
        paquete* pack = check_and_cast<paquete*>(msg);
        std::string nodesVisited = pack->getNodesVisited();
        if (nodesVisited.compare("")!=0)
        {
            nodesVisited.append("-");
        }
        const char* nodeName = (const char*) par("nodeName");
        nodesVisited.append(nodeName);
        pack->setNodesVisited(nodesVisited.c_str());

        double p_paquete = par("p_paquete");
        const char* gateName = pack->getArrivalGate()->getName();
        if (strcmp(gateName,"inPaquetes") == 0)
        {
            p_paquete = 0;
        }

        if (p_paquete > uniform(0,1))
        {
            EV << "Paquete perdido" << endl;
        }

        else
        {
            receivedPackets += 1;
            forwardMessage(pack);
            if (strcmp(gateName,"inPaquetes") != 0)
            {
                sendAck(pack);
            }
        }
    }

}

void NodeSW::forwardMessage(cMessage *msg)
{
    int port = getOutPort();
    realProbabilities[port] += 1;
    if (estado[port] == idle)
    {
        estado[port]=active;
        currentMessage[port] = msg;
        sendPacket(msg,port);
    }

    else
    {
        txQueues[port].insert(msg);
    }
}

int NodeSW::getOutPort(){
    int port = 0;
    double rand = uniform (0,1);
    while (rand > probabilities[port])
    {
        rand = rand - probabilities[port];
        port++;
    }
    return port;
}

void NodeSW::sendPacket(cMessage* packet,int port)
{
    send(packet -> dup(), "out$o",port);
    cChannel* txChannel = gate("out$o",port)->getChannel();
    simtime_t timeout = par ("timeout");
    simtime_t time = txChannel->getTransmissionFinishTime() + timeout;
    if (probabilities.size() == 1)
    {
        if (txQueues[port].isEmpty())
        {
            estado[port] = idle;
        }
        else
        {
            currentMessage[port] = check_and_cast<cMessage*>(txQueues[port].pop());
            sendPacket(currentMessage[port], port);
        }
    }
    else
    {
        scheduleAt(time, &timeoutMessages[port]);
    }
}

void NodeSW::sendAck(cMessage* packet)
{
    cGate* gate = packet->getArrivalGate();
    int port = gate->getIndex();
    ack* ackMessage = new ack();
    ackMessage->setName("ack");
    send(ackMessage,"in$o",port);
}
void NodeSW::finish()
{
    EV << "Las probabilidades reales del nodo son: ";
    for (int i = 0; i<n_salidas;i++)
    {
        double probability = realProbabilities[i]/receivedPackets;
        EV << probability << " ";
    }
    EV << endl;
}
