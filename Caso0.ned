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
    gates:
        output out;
}

simple Node
{
    parameters:
        @display("i=block/routing");
    gates:
        input in[];
        output out[];
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
            datarate = 100kbps;
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
        Source[0].out --> Channel --> nodo[0].in++;
        nodo[0].out++ --> Channel --> nodo[1].in++;
        nodo[0].out++ --> Channel --> nodo[4].in++;
        //nodo2		
        Source[1].out --> Channel --> nodo[1].in++;
        nodo[1].out++ --> Channel --> nodo[2].in++;
        nodo[1].out++ --> Channel --> nodo[3].in++;
        //nodo3
        nodo[2].out++ --> Channel --> Receptor[0].in;
        //nodo4
        nodo[3].out++ --> Channel --> Receptor[1].in;
        //nodo5
        Source[2].out --> Channel --> nodo[4].in++;
        nodo[4].out++ --> Channel --> nodo[3].in++;
        nodo[4].out++ --> Channel --> nodo[1].in++;


}
