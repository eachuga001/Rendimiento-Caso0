//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//
import ned.DatarateChannel;

simple Fuente
{
    parameters:
        @display("i=block/source");
        double interArrivalsTime = default(1.0);
    	int packet_length = default(100);
    	int n_paquetes = default(100);
    gates:
        output out;
}

simple Node
{
    parameters:
        @display("i=block/routing");
        int n_entradas @prompt("Numero de entradas") = default(1);
        int n_salidas  @prompt("Numero de salidas") = default(1);
        string probabilities @prompt("Probabilidades de escoger puerto de salida") = default("1");
    gates:
        input in[n_entradas];
        output out[n_salidas];
}

simple Receptor
{
    parameters:

    gates:
        input in;
}

network Caso0
{
    @display("bgb=557,310");
    types:
        channel Channel extends ned.DatarateChannel
        {
            datarate = 100Mbps;
            delay = 0ms;
        }
    submodules:

        Source[3]: Fuente;
        nodo[5]: Node;
        Receptor[2]: Receptor;
        
    connections:

//        Source.out --> DatarateChannel --> nodo.in++;
//        nodo.out++ --> DatarateChannel --> Receptor[0].in;
//        nodo.out++ --> DatarateChannel --> Receptor[1].in;

        //nodo1
        Source[0].out --> nodo[0].in++;
        nodo[0].out[0] --> Channel --> nodo[1].in[0];
        nodo[0].out[1] --> Channel --> nodo[4].in[0];
        //nodo2		
        Source[1].out --> nodo[1].in[1];
        nodo[1].out[0] --> Channel --> nodo[2].in[0];
        nodo[1].out[1] --> Channel --> nodo[3].in[0];
        //nodo3
        nodo[2].out[0] --> Receptor[0].in;
        //nodo4
        nodo[3].out[0] --> Receptor[1].in;
        
        //nodo5
        Source[2].out --> nodo[4].in++;
        nodo[4].out++ --> Channel --> nodo[3].in++;
        nodo[4].out++ --> Channel --> nodo[1].in++;


}
