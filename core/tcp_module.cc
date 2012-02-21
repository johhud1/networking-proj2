#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>


#include <iostream>

#include "Minet.h"

using std::cout;
using std::cerr;
using std::endl;

struct TCPState {
  std::ostream & Print(std::ostream &os) const { os <<"TCPState()"; return os;}

  friend std::ostream &operator<<(std::ostream &os, const TCPState& L) {
    return L.Print(os);
  }
};

int main(int argc, char *argv[])
{
  MinetHandle mux, sock;

  ConnectionList<TCPState> clist;

  MinetInit(MINET_TCP_MODULE);

  mux=MinetIsModuleInConfig(MINET_IP_MUX) ? MinetConnect(MINET_IP_MUX) : MINET_NOHANDLE;
  sock=MinetIsModuleInConfig(MINET_SOCK_MODULE) ? MinetAccept(MINET_SOCK_MODULE) : MINET_NOHANDLE;

  if (MinetIsModuleInConfig(MINET_IP_MUX) && mux==MINET_NOHANDLE) {
    MinetSendToMonitor(MinetMonitoringEvent("Can't connect to mux"));
    return -1;
  }

  if (MinetIsModuleInConfig(MINET_SOCK_MODULE) && sock==MINET_NOHANDLE) {
    MinetSendToMonitor(MinetMonitoringEvent("Can't accept from sock module"));
    return -1;
  }

  MinetSendToMonitor(MinetMonitoringEvent("tcp_module handling TCP traffic"));

  MinetEvent event;

  while (MinetGetNextEvent(event)==0) {
    if (event.eventtype!=MinetEvent::Dataflow 
	|| event.direction!=MinetEvent::IN) {
      cerr << "in 'Unknown event ignored block\n";
      MinetSendToMonitor(MinetMonitoringEvent("Unknown event ignored."));
    } else {
      if (event.handle==mux) {
	cerr << "handling event.handle of type mux\n";
	cerr << "printing event: " << event;
	SockRequestResponse req;
	unsigned short len;
	bool checksumok;
	//Packet p = req
	Packet p;
	MinetReceive(mux, p);
	//MinetReceive(mux,req);
	//cerr << "recieved req from mux, req: " << req;
	cerr << "the packet we recieve was: " << p << endl;
	SockRequestResponse repl;
	switch(req.type){
		case WRITE:
			cerr << "in req.type WRITE for mux\n";
			//TODO: WRITE code
			memcpy(&(repl.data), &(req.data), req.bytes);
			repl.bytes = req.bytes;
			repl.connection = req.connection;
			cerr << "sending repl up to sock: " << repl << endl;
			MinetSend(sock, repl);
			break;
		case CLOSE:
			cerr << "in req.type CLOSE For mux\n";
			//TODO: CLOSE code
			//MinetSend(sock, repl);
			break;
		case STATUS:
			cerr << "in req.type STATUS for mux\n";
			//TODO: STATUS code
			//MinetSend(sock, repl);
			break;
		case CONNECT:
		case ACCEPT:
		case FORWARD:
		break;
	}
      }
      
      if (event.handle==sock) {
	cerr << "handling event.handle of type sock\n";
	SockRequestResponse s;
	SockRequestResponse repl;
	MinetReceive(sock,s);
	/*
	unsigned tcphlen=TCPHeader::EstimateTCPHeaderLength(s);
	cerr << "Received Socket Request:" << s << endl;
	cerr << "estimated packet length (tcphlen): " << tcphlen << endl;
	Packet p(s.data.ExtractFront(tcphlen));
	*/
	switch(s.type){
		case CONNECT: 
			cerr << "in CONNECT for sockRequest\n";
			//MinetSend(mux, repl);
			//MinetSend(sock, repl);
		//TODO: fill in
		break;
		case STATUS:
			cerr << "in STATUS for sockRequest\n";
			//MinetSend(sock, repl);
			//TODO: STATUS code
		break;
		case ACCEPT:
			cerr << "in ACCEPT for sockRequest\n";
			repl.error = EOK;
			repl.type = STATUS;
			repl.bytes = 0;
			repl.connection = s.connection;
			cerr << "sending repl: " << repl << endl;
			//MinetSend(sock, repl);
			//TODO: accept code
		break;
		case WRITE:
			cerr << "in WRITE for sockRequest\n";
			//MinetSend(mux, repl);
			//TODO: write code
		break;
		case FORWARD:
			cerr << "in FORWARD for sockRequest\n";
			repl = s;
			repl.type = STATUS;
			repl.error = EOK;
			MinetSend(sock, repl);
			//TODO: forward code
		break;
		case CLOSE:
			cerr << "in CLOSE for sockRequest\n";
			//MinetSend(mux, repl);
			//TODO: close code
		break;
		default:
		{
			cerr << "ruhroh, in default case for sockRequest!!!\n";
			MinetSend(sock, repl);
		}
	}
      }
    }
  }
  cerr << "returning out of tcp_module\n";
  return 0;
}


	//OLD event.handle = mux SHIT

	/*
	ConnectionList<TCPState>::iterator cs = clist.FindMatching(c);
	if(cs!=clist.end()){
		cerr << "at end of clist\n";
		//tcph.GetHeaderLen(len);
		len -= TCP_HEADER_BASE_LENGTH;
		Buffer &data = p.GetPayload().ExtractFront(len);
		SockRequestResponse write(WRITE,
					(*cs).connection,
					data,
					len,
					EOK);
		if(!checksumok){
			MinetSendToMonitor(MinetMonitoringEvent("forwarding packet to sock even though checksum failed"));
		}
		MinetSend(sock, write);
	}
	else{
	  MinetSendToMonitor(MinetMonitoringEvent("Unknown port, sending ICMP error message"));
	  IPAddress source; iph.GetSourceIP(source);
	  ICMPPacket error(source,DESTINATION_UNREACHABLE,PORT_UNREACHABLE,p);
	  MinetSendToMonitor(MinetMonitoringEvent("ICMP error message has been sent to host"));
	  MinetSend(mux, error);
	}*/
