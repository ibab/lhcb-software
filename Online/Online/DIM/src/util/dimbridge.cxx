#include <dic.hxx>
#include <dis.hxx>
#include <dim.h>
#include <iostream>
using namespace std;

static int no_link = 0xdeaddead;
static char from_node[64], to_node[64], bridge_name[64];

class BridgeService: public DimInfo, public SLLItem 
{
  char srv_name[256];
  char srv_format[256];
  int declared;
  DimService *srv;
  void *srv_data;
  int srv_size;
  int cmnd;
  int found;

  void infoHandler() {
    srv_data = DimInfo::getData();
    srv_size = DimInfo::getSize();
    if(*(int *)srv_data == no_link)
	{
		if(srv)
			delete(srv);
		declared = 0;
//  		cout << "Disconnecting bridge for: " << srv_name << endl;
	}
    else if(!declared)
    {
//		DimServer::setDnsNode(to_node);
		srv = new DimService(srv_name, srv_format, srv_data, srv_size);
		DimServer::start(bridge_name);
		declared = 1;
//		DimClient::setDnsNode(from_node);
	}
    else
	{
		srv->updateService(srv_data, srv_size);
	}
  }

  public:
    BridgeService(char *name, char *format):
		DimInfo(name, &no_link, sizeof(no_link)), declared(0), srv(0)
		{ 
			strcpy(srv_format, format);
			strcpy(srv_name, name);
			cmnd = 0;
			found = 1;
//			cout << "Bridging Service: " << name << endl;
		}
	BridgeService(char *name, char *format, int rate):
		DimInfo(name, rate, &no_link, sizeof(no_link)), declared(0), srv(0)
		{ 
			strcpy(srv_format, format);
			strcpy(srv_name, name);
			cmnd = 0;
			found = 1;
//			cout << "Bridging Service: " << name << ", rate = " << rate << " seconds" << endl;
		}
  	~BridgeService()
  	{
  		if(declared)
  			delete(srv);
  		declared = 0;
//  		cout << "Stopped bridge for: " << srv_name << endl;
  	}
    char *getName() { return srv_name; };
    void clear() { found = 0;};
    void set() {found = 1;};
    int find() {return found;};
    int isCmnd() { return cmnd;};
};

class BridgeCommand: public DimCommand, public SLLItem 
{
  char srv_name[256];
  char srv_format[256];
  int declared;
  DimService *srv;
  void *srv_data;
  int srv_size;
  int cmnd;
  int found;

  void commandHandler() {
    srv_data = DimCommand::getData();
    srv_size = DimCommand::getSize();
    DimClient::sendCommandNB(srv_name, srv_data, srv_size);
  }

  public:
    BridgeCommand(char *name, char *format):
		DimCommand(name, format) 
		{ 
			DimServer::start(bridge_name);
			cmnd = 1;
			found = 1;
			strcpy(srv_name, name);
//			cout << "Bridging Command: " << name << endl;
		}
    char *getName() { return srv_name; };
    void clear() { found = 0;};
    void set() {found = 1;};
    int find() {return found;};
    int isCmnd() { return cmnd;};
};

void print_usage()
{
	cout << "Usage: DimBridge [from_node] to_node services [time_interval]" << endl;
	cout << "    from_node - by default DIM_DNS_NODE" << endl;
	cout << "    to_node - the complete node name of the new DNS" << endl;
	cout << "    services - the list of service names (wildcards allowed)" << endl;
	cout << "    time_interval - the interval in seconds to be used for updating the services" << endl;
}

int main(int argc, char **argv)
{
char services[132];
DimBrowser dbr;
char *service, *format, *p;
int type, known;
BridgeService *ptrs, *aux_ptrs;
BridgeCommand *ptrc, *aux_ptrc;
SLList lists, listc;
int rate = 0;

	if(argc < 3)
    {
		print_usage();
    }
	if( argc == 3)
    {
		strcpy(from_node, DimClient::getDnsNode());
		strcpy(to_node, argv[1]);
		strcpy(services, argv[2]);
    }
	if (argc == 4)
	{
		if(sscanf(argv[3],"%d", &rate))
		{
			strcpy(from_node, DimClient::getDnsNode());
			strcpy(to_node, argv[1]);
			strcpy(services, argv[2]);
		}
		else
		{
			rate = 0;
			strcpy(from_node, argv[1]);
			strcpy(to_node, argv[2]);
			strcpy(services, argv[3]);
		}
    }
	if(argc == 5)
	{
		strcpy(from_node, argv[1]);
		strcpy(to_node, argv[2]);
		strcpy(services, argv[3]);
		sscanf(argv[4],"%d", &rate);
    }

	cout << "Starting DimBridge from "<<from_node<<" to "<<to_node<<" for "<< services;
	if(rate)
		cout << " interval = " << rate << endl; 
	else
		cout << endl;

	strcpy(bridge_name,"Bridge_");
	strcat(bridge_name, from_node);
	if( (p = strchr(bridge_name,'.')) )
		*p = '\0';
#ifndef WIN32
	sprintf(p,"_%d",getpid());
#else
	sprintf(p,"_%d",_getpid());
#endif
	DimClient::setDnsNode(from_node);
	DimServer::setDnsNode(to_node);
	while(1)
	{
		ptrs = (BridgeService *)lists.getHead();
		while(ptrs)
		{
			ptrs->clear();
			ptrs = (BridgeService *)lists.getNext();
		}
		ptrc = (BridgeCommand *)listc.getHead();
		while(ptrc)
		{
			ptrc->clear();
			ptrc = (BridgeCommand *)listc.getNext();
		}
		dbr.getServices(services);
		while( (type = dbr.getNextService(service, format)) )
		{
			known = 0;
			ptrs = (BridgeService *)lists.getHead();
			while(ptrs)
			{
				if(!strcmp(ptrs->getName(), service))
				{
					known = 1;
					ptrs->set();
					break;
				}
				ptrs = (BridgeService *)lists.getNext();
			}
			ptrc = (BridgeCommand *)listc.getHead();
			while(ptrc)
			{
				if(!strcmp(ptrc->getName(), service))
				{
					known = 1;
					ptrc->set();
					break;
				}
				ptrc = (BridgeCommand *)listc.getNext();
			}
			if(strstr(service,"DIS_DNS"))
				known = 1;
			if(!known)
			{
				if(type == DimSERVICE)
				{
				  if(!rate)
					ptrs = new BridgeService(service, format);
				  else
					ptrs = new BridgeService(service, format, rate);
				  lists.add(ptrs);
				}
				else if (type == DimCOMMAND)
				{
//					DimClient::setDnsNode(to_node);
					ptrc = new BridgeCommand(service, format);
					listc.add(ptrc);
//					DimClient::setDnsNode(from_node);
				}
			}
		}
		ptrs = (BridgeService *)lists.getHead();
		while(ptrs)
		{
			aux_ptrs = 0;
			if(!ptrs->find())
			{
				lists.remove(ptrs);
				aux_ptrs = ptrs;
			}
			ptrs = (BridgeService *)lists.getNext();
			if(aux_ptrs)
			{
				delete aux_ptrs;
			}
		}
		ptrc = (BridgeCommand *)listc.getHead();
		while(ptrc)
		{
			aux_ptrc = 0;
			if(!ptrc->find())
			{
				listc.remove(ptrc);
				aux_ptrc = ptrc;
			}
			ptrc = (BridgeCommand *)listc.getNext();
			if(aux_ptrc)
			{
				delete aux_ptrc;
			}
		}
		sleep(2);
	}
	return 1;
}







