#define DIMLIB
#include <dis.hxx>
#include "tokenstring.hxx"
//#include <iostream>
//using namespace std;
#include <time.h>
//#include <sys/timeb.h>

DimClientExitHandler *DimServer::itsClientExit = 0;
DimExitHandler *DimServer::itsExit = 0;
DimErrorHandler *DimServer::itsSrvError = 0;
char *DimServer::itsName = 0;
char *DimServer::clientName = 0;
char *DimServer::dimDnsNode = 0;
int DimServer::autoStart = 1;
//int DimServer::itsNServices = 0;

extern "C" {
extern void dis_init();

static void user_routine( void *tagp, void **buf, int *size, int *first_time)
{
//	int *tag = (int *)tagp;
//	int id = *tag;
	DimService *t;

	if(first_time){}
//	t = (DimService *)id_get_ptr(id, SRC_DIS);
	t = *(DimService **)tagp;
	if( t->itsServiceHandler ) {
		t->itsServiceHandler->itsService = t;
		DimCore::inCallback = 2;
		t->itsServiceHandler->serviceHandler();
		DimCore::inCallback = 0;
	}
	else
	{
		DimCore::inCallback = 2;
		t->serviceHandler();
		DimCore::inCallback = 0;
	}
	if( t->itsType == DisSTRING)
			t->itsSize = (int)strlen((char *)t->itsData)+1;
	*buf = t->itsData;
	*size = t->itsSize;
}
}

void DimService::declareIt(char *name, char *format, DimServiceHandler *handler, DimServerDns *dns)
{
//	itsTagId = 0;
	itsDns = dns;
	itsName = new char[(int)strlen(name)+1];
	itsDataSize = 0;
	strcpy( itsName, name);
	if(handler)
		itsServiceHandler = handler;
	else
		itsServiceHandler = 0;
//	itsTagId = id_get((void *)this, SRC_DIS);
	dis_init();
	if(itsDns == 0)
	{
		DISABLE_AST
		itsId = dis_add_service( name, format, NULL, 0, 
//				user_routine, itsTagId);
				user_routine, (dim_long)this);
		ENABLE_AST
		DimServer::start();
	}
	else
	{
		DISABLE_AST
		itsId = dis_add_service_dns( itsDns->getDnsId(), name, format, NULL, 0, 
//				user_routine, itsTagId);
				user_routine, (dim_long)this);
		ENABLE_AST
//		itsDns->addServiceId(itsId);
		DimServer::start(itsDns);
	}
}

void DimService::storeIt(void *data, int size)
{
	DISABLE_AST
	if(!itsId)
	{
		ENABLE_AST
		return;
	}
	if(!itsDataSize)
	{
		itsData = new char[size];
		itsDataSize = size;
	}
	else if(itsDataSize < size)
	{
		delete[] (char *)itsData;
		itsData = new char[size];
		itsDataSize = size;
	}
	memcpy(itsData, data, (size_t)size);
	itsSize = size;
	ENABLE_AST
}

extern "C" {
static void command_routine( void *tagp, void *buf, int *size)
{
//	int *tag = (int *)tagp;
//	int id = *tag;
	DimCommand *t;

//	t = (DimCommand *)id_get_ptr(id, SRC_DIS);
	t = *(DimCommand **)tagp;
	t->itsData = buf;
	t->itsSize = *size;
	t->secs = 0;
	if( t->itsCommandHandler ) {
		t->itsCommandHandler->itsCommand = t;
		DimCore::inCallback = 2;
		t->itsCommandHandler->commandHandler();
		DimCore::inCallback = 0;
	}
	else
	{
		DimCore::inCallback = 2;
		t->commandHandler();
		DimCore::inCallback = 0;
	}
	t->itsData = 0;
	t->itsSize = 0;
}
}

void DimCommand::declareIt(char *name, char *format, DimCommandHandler *handler, DimServerDns *dns)
{
//	itsTagId = 0;
	itsId = 0;
	itsDns = dns;
	itsName = new char[(int)strlen(name)+1];
	strcpy( itsName, name);
	itsFormat = new char[(int)strlen(format)+1];
	strcpy( itsFormat, format);
	currCmnd = 0;
	if(handler)
		itsCommandHandler = handler;
	else
		itsCommandHandler = 0;
//	itsTagId = id_get((void *)this, SRC_DIS);
	dis_init();
	if(!itsDns)
	{
		DISABLE_AST
		itsId = dis_add_cmnd( name, format, command_routine,
//				itsTagId);
				(dim_long)this);
		ENABLE_AST
		DimServer::start();
	}
	else
	{
		DISABLE_AST
		itsId = dis_add_cmnd_dns( itsDns->getDnsId(), name, format, command_routine,
//			itsTagId);
			(dim_long)this);
		ENABLE_AST
//		itsDns->addServiceId(itsId);
		DimServer::start(itsDns);
	}
}

extern "C" {
/*
static void timeout_rout(DimRpc *t)
{
	sleep(t->itsTimeout);
	t->itsKilled = 1;
}
*/
static void rpcin_routine( void *tagp, void *buf, int *size)
{
	time_t tt1 = 0, tt2 = 0;

//	int *tag = (int *)tagp;
//	int id = *tag;
	DimRpc *t;
	int tout, clientId, ids[2];
//	long tid;

//	t = (DimRpc *)id_get_ptr(id, SRC_DIS);
	t = *(DimRpc **)tagp;
	t->itsDataIn = buf;
	t->itsSizeIn = *size;
	clientId = dis_get_conn_id();
	tout = dis_get_timeout(t->itsIdOut, clientId);
	t->itsTimeout = tout;
//	tid = 0;
	if(tout > 0)
	{
		tt1 = time((time_t *)0);
		t->itsKilled = 0;
//		dtq_start_timer(t->itsTimeout,(void(*)(void *))timeout_rout,(void *)t);
//		tid = dim_start_thread((void(*)(void *))timeout_rout,(void *)t);
	}
	DimCore::inCallback = 2;
	t->rpcHandler();
	DimCore::inCallback = 0;
	t->itsDataIn = 0;
	t->itsSizeIn = 0;
	if(tout > 0)
	{
		tt2 = time((time_t *)0);
		if((tt2 - tt1) > tout)
			t->itsKilled = 1;
	}
	if(!t->itsKilled)
	{
//		if(tid)
//		{
//			dtq_stop_timer((void *)t);
//			dim_stop_thread(tid);
//		}
		ids[0] = clientId;
		ids[1] = 0;
		dis_selective_update_service(t->itsIdOut, ids);
	}
}

}

extern "C" {
static void rpcout_routine( void *tagp, void **buf, int *size, int *first_time)
{
//	int *tag = (int *)tagp;
//	int id = *tag;
	DimRpc *t;

	if(first_time){}
//	t = (DimRpc *)id_get_ptr(id, SRC_DIS);
	t = *(DimRpc**)tagp;
	*buf = t->itsDataOut;
	*size = t->itsSizeOut;
}
}

void DimRpc::declareIt(char *name, char *formatin, char *formatout, DimServerDns *dns)
{
//	itsTagId = 0;
	itsIdIn = 0;
	itsIdOut = 0;
	itsDns = dns;
	itsName = new char[(int)strlen(name)+1];
	strcpy( itsName, name);
	itsNameIn = new char[(int)strlen(name)+1+10];
	strcpy( itsNameIn, name);
	strcat(itsNameIn,(char *)"/RpcIn");
	itsNameOut = new char[(int)strlen(name)+1+10];
	strcpy( itsNameOut, name);
	strcat(itsNameOut,(char *)"/RpcOut");
	itsDataOut = new char[1];
	itsDataOutSize = itsSizeOut = 1;
	itsKilled = 0;
	itsTimeout = 0;
	
//	itsTagId = id_get((void *)this, SRC_DIS);
	dis_init();
	if(!itsDns)
	{
		DISABLE_AST
		itsIdIn = dis_add_cmnd( itsNameIn, formatin, 
//			rpcin_routine, itsTagId);
			rpcin_routine, (dim_long)this);
		itsIdOut = dis_add_service( itsNameOut, formatout, 0,0, 
//			rpcout_routine, itsTagId);
			rpcout_routine, (dim_long)this);
		ENABLE_AST
		DimServer::start();
	}
	else
	{
		DISABLE_AST
		itsIdIn = dis_add_cmnd_dns( itsDns->getDnsId(), itsNameIn, formatin, 
//			rpcin_routine, itsTagId);
			rpcin_routine, (dim_long)this);
		itsIdOut = dis_add_service_dns( itsDns->getDnsId(), itsNameOut, formatout, 0,0, 
//			rpcout_routine, itsTagId);
			rpcout_routine, (dim_long)this);
		ENABLE_AST
//		itsDns->addServiceId(itsIdIn);
//		itsDns->addServiceId(itsIdOut);
		DimServer::start(itsDns);
	}
}

void DimRpc::storeIt(void *data, int size)
{
	DISABLE_AST
	if(!itsIdIn)
	{
		ENABLE_AST
		return;
	}
	if(!itsDataOutSize)
	{
		itsDataOut = new char[size];
		itsDataOutSize = size;
	}
	else if(itsDataOutSize < size)
	{
		delete[] (char *)itsDataOut;
		itsDataOut = new char[size];
		itsDataOutSize = size;
	}
	memcpy(itsDataOut, data, (size_t)size);
	itsSizeOut = size;
	ENABLE_AST
}

extern "C" {
static void client_exit_user_routine(int*);
static void exit_user_routine(int*);
static void srv_error_user_routine(int, int, char*);
}

DimServerDns::DimServerDns(const char *node)
{
	init(node, 0);
}
	
DimServerDns::DimServerDns(const char *node, int port)
{
	init(node, port);
}

DimServerDns::DimServerDns(const char *node, int port, char *name)
{
	init(node, port);
	DimServer::start(this, name);
}
	
#define DisDnsIdBlock 100

void DimServerDns::init(const char *node, int port)
{
//	if(!itsNode)
//	{
		itsNode = new char[(int)strlen(node)+1];
		strcpy(itsNode,node);
//	}
	itsPort = port;
	autoStart = 1;
	itsName = 0;
	itsServiceIdList = new int[DisDnsIdBlock];
	itsServiceIdListSize = DisDnsIdBlock;
	itsNServiceIds = 0;
//	itsNServices = 0;
	itsDnsId = DimServer::addDns(node, port);
}

void DimServerDns::addServiceId(int id)
{
	int *tmp;

	DISABLE_AST
	if((itsNServiceIds + 2) > itsServiceIdListSize)
	{
		tmp = new int[itsServiceIdListSize + DisDnsIdBlock];
		memcpy(tmp, itsServiceIdList, (size_t)itsServiceIdListSize*sizeof(int));
		delete itsServiceIdList;
		itsServiceIdList = tmp;
		itsServiceIdListSize += DisDnsIdBlock;
	}
	itsServiceIdList[itsNServiceIds] = id;
	itsServiceIdList[itsNServiceIds+1] = 0;
	itsNServiceIds++;
	ENABLE_AST
}

int *DimServerDns::getServiceIdList()
{
	int *list;
	if(itsNServiceIds)
		list = itsServiceIdList;
	else
		list = 0;
	itsNServiceIds = 0;
	return list;
}

DimServerDns::~DimServerDns()
{
	if(itsName)
	{
		DimServer::stop(this);
//		if(itsName)
//			delete[] itsName;
	}
//	if(itsNode)
		delete[] itsNode;
}

dim_long DimServerDns::getDnsId()
{
	return itsDnsId;
}

void DimServerDns::setName(const char *name)
{
	if(!itsName)
	{
		itsName = new char[(int)strlen(name)+1];
		strcpy(itsName,name);
	}
}

void DimServerDns::clearName()
{
	if(itsName)
	{
		delete[] itsName;
		itsName = 0;
	}
}

char *DimServerDns::getName()
{
	return itsName;
}

void DimServerDns::autoStartOn()
{
	autoStart = 1;
}

void DimServerDns::autoStartOff()
{
	autoStart = 0;
}

int DimServerDns::isAutoStart()
{
	return autoStart;
}

DimServer::DimServer()
{
	itsClientExit = this; 
	itsExit = this;
	itsSrvError = this;
//	itsNServices = 0;
}

DimServer::~DimServer() 
{
	if(itsName)
	{
		dis_stop_serving();
		delete[] itsName;
	}
	if(clientName)
		delete[] clientName;
	if(dimDnsNode)
		delete[] dimDnsNode;
}

void DimServer::start(const char *name)
{
	if(!itsName)
	{
		itsName = new char[(int)strlen(name)+1];
		strcpy(itsName,name);
	}
	dis_start_serving(itsName);
}

void DimServer::start(DimServerDns *dns, const char *name)
{
	dim_long dnsid;

	dis_init();
	{
	DISABLE_AST
	dns->setName(name);
	dnsid = dns->getDnsId();
	dis_start_serving_dns(dnsid, (char *)name /*, dns->getServiceIdList()*/);
	ENABLE_AST
	}
}
/*
void DimServer::threadHandler()
{
	int oldNServices;

	while(1)
	{
		oldNServices = itsNServices;
		usleep(100000);
		if(oldNServices == itsNServices)
			break;
	}
cout << "Starting " << itsNServices << endl;
	{
		DISABLE_AST
		dis_start_serving(itsName);
		itsNServices = 0;
		ENABLE_AST
	}

}
*/
void DimServer::start()
{
//	itsNServices++;
	if((itsName) && (autoStart))
	{
//		DimThread::start();
		dis_start_serving(itsName);
	}
}

void DimServer::start(DimServerDns *dns)
{
	dim_long dnsid;
	char *name;
	int isAuto;

	dis_init();
	{
	DISABLE_AST
//	dns->itsNServices++;

	name = dns->getName();
	dnsid = dns->getDnsId();
	isAuto = dns->isAutoStart();
	if((name) && (isAuto))
	{
//		DimThread::start();
		dis_start_serving_dns(dnsid, (char *)name /*, dns->getServiceIdList()*/);
	}
	ENABLE_AST
	}
}

void DimServer::stop()
{
	dis_stop_serving();
	if(itsName)
	{
		delete[] itsName;
		itsName = 0;
	}
}

void DimServer::stop(DimServerDns *dns)
{
	dis_stop_serving_dns(dns->getDnsId());
	dns->clearName();
}

void DimServer::autoStartOn()
{
	autoStart = 1;
}

void DimServer::autoStartOff()
{
	autoStart = 0;
}
	
int DimServer::getClientId()
{
	if(!clientName)
		clientName = new char[128];
	clientName[0] = '\0';
	return dis_get_client(clientName);
}
	
char *DimServer::getClientName()
{
	if(!clientName)
		clientName = new char[128];
	clientName[0] = '\0';
	dis_get_client(clientName);
	return(clientName);
}
/*
char *DimServer::getClientServices()
{
	int id;
	if((id = dis_get_conn_id()))
		return dis_get_client_services(id);
	return (char *)0;
}

char *DimServer::getClientServices(int clientId)
{
	return dis_get_client_services(clientId);
}
*/
char **DimServer::getClientServices()
{
	static TokenString *data = 0;
	int id, len = 0, index = 0;
	char *services;
	static char** list = 0;
	char *sep;

	if(data)
	{
		delete data;
		data = 0;
	}
	if(list)
	{
		delete[] list;
		list = 0;
	}
	if((id = dis_get_conn_id()))
	{
		services = dis_get_client_services(id);
		if(services)
		{
			data = new TokenString(services,(char *)"\n");
			len = data->getNTokens();
			list = new char*[len];
			while(data->getToken(list[index]))
			{
				data->getToken(sep);
				index++;
			}
		}
	}
	if(!len)
		list = new char*[1];
	list[index] = 0;
	return list;
}

void DimServer::setClientExitHandler(int clientId)
{
	dis_set_client_exit_handler(clientId, 1);
}

void DimServer::clearClientExitHandler(int clientId)
{
	dis_set_client_exit_handler(clientId, 0);
}

void DimServer::addClientExitHandler(DimClientExitHandler *handler)
{
	if(handler == 0)
	{
		dis_add_client_exit_handler(0);
		DimServer::itsClientExit = 0;
	}
	else
	{
		DimServer::itsClientExit = handler;
		dis_add_client_exit_handler(client_exit_user_routine);
	}
}

void DimServer::addClientExitHandler()
{
	DimServer::itsClientExit = this;
	dis_add_client_exit_handler(client_exit_user_routine);
}

void DimServer::addExitHandler(DimExitHandler *handler)
{
	if(handler == 0)
	{
		dis_add_exit_handler(0);
		DimServer::itsExit = 0;
	}
	else
	{
		DimServer::itsExit = handler;
		dis_add_exit_handler(exit_user_routine);
	}
}

void DimServer::addErrorHandler(DimErrorHandler *handler)
{
	if(handler == 0)
	{
		dis_add_error_handler(0);
		DimServer::itsSrvError = 0;
	}
	else
	{
		DimServer::itsSrvError = handler;
		dis_add_error_handler(srv_error_user_routine);
	}
}

int DimServer::setDnsNode(const char *node) 
{
	return dis_set_dns_node((char *)node); 
}

int DimServer::setDnsNode(const char *node, int port) 
{
	dis_set_dns_port(port);
	return dis_set_dns_node((char *)node); 
}

dim_long DimServer::addDns(const char *node, int port) 
{
	return dis_add_dns((char *)node, port); 
}
char *DimServer::getDnsNode() 
{
	if(!dimDnsNode)
		dimDnsNode = new char[256];
	if(dis_get_dns_node(dimDnsNode))
		return dimDnsNode;
	else
		return 0; 
}

int DimServer::getDnsPort() 
{
	return dis_get_dns_port();
}

void DimServer::setWriteTimeout(int secs)
{
	dim_set_write_timeout(secs); 
}

int DimServer::getWriteTimeout() 
{
	return dim_get_write_timeout();
}

void DimServer::addExitHandler()
{
	DimServer::itsExit = this;
	dis_add_exit_handler(exit_user_routine);
}

void DimServer::addErrorHandler()
{
	DimServer::itsSrvError = this;
	dis_add_error_handler(srv_error_user_routine);
}

int DimServer::inCallback()
{
	if(DimCore::inCallback)
		return 1;
	return 0;
}

extern "C" {
static void client_exit_user_routine(int *idp)
{
	int id = *idp;

	id++;
	DimCore::inCallback = 2;
	DimServer::itsClientExit->clientExitHandler();
	DimCore::inCallback = 0;
}

static void exit_user_routine(int *idp)
{
//	int id = *idp;

//	id++;
	DimCore::inCallback = 2;
	DimServer::itsExit->exitHandler(*idp);
	DimCore::inCallback = 0;
}

static void srv_error_user_routine(int severity, int code, char *msg)
{

	DimCore::inCallback = 2;
	if(DimServer::itsSrvError != 0)
		DimServer::itsSrvError->errorHandler(severity, code, msg);
	DimCore::inCallback = 0;
}

}


DimService::DimService()
{
//	itsTagId = 0;
	itsId = 0;
	itsName = 0;
}

DimService::DimService(const char *name, int &value) 
{
	itsId = 0;
	itsName = 0;
	itsData = &value;
	itsSize = sizeof(int);
	itsType = DisINT;
	declareIt((char *)name, (char *)"L", 0, 0);
}

DimService::DimService(const char *name, float &value)
{
	itsId = 0;
	itsName = 0;
	itsData = &value;
	itsSize = sizeof(float);
	itsType = DisFLOAT;
	declareIt((char *)name, (char *)"F", 0, 0);
}

DimService::DimService(const char *name, double &value)
{
	itsId = 0;
	itsName = 0;
	itsData = &value;
	itsSize = sizeof(double);
	itsType = DisDOUBLE;
	declareIt((char *)name, (char *)"D", 0, 0);
}

DimService::DimService(const char *name, longlong &value)
{
	itsId = 0;
	itsName = 0;
	itsData = &value;
	itsSize = sizeof(longlong);
	itsType = DisXLONG;
	declareIt((char *)name, (char *)"X", 0, 0);
}

DimService::DimService(const char *name, short &value)
{
	itsId = 0;
	itsName = 0;
	itsData = &value;
	itsSize = sizeof(short);
	itsType = DisSHORT;
	declareIt((char *)name, (char *)"S", 0, 0);
}

DimService::DimService(const char *name, char *string)
{
	itsId = 0;
	itsName = 0;
	itsData = string;
	itsSize = (int)strlen(string)+1;
	itsType = DisSTRING;
	declareIt((char *)name, (char *)"C", 0, 0);
}

DimService::DimService(const char *name, char *format, void *structure, int size)
{
	itsId = 0;
	itsName = 0;
	itsData = structure;
	itsSize = size;
	itsType = DisPOINTER;
	declareIt((char *)name, (char *)format, 0, 0);
}

DimService::DimService(const char *name, char *format, DimServiceHandler *handler)
{
	itsId = 0;
	itsName = 0;
	itsData = 0;
	itsSize = 0;
	itsType = DisPOINTER;
	declareIt((char *)name, (char *)format, handler, 0);
}

DimService::DimService(const char *name, const char *format, void *structure, int size)
{
	itsId = 0;
	itsName = 0;
	itsData = structure;
	itsSize = size;
	itsType = DisPOINTER;
	declareIt((char *)name, (char *)format, 0, 0);
}

DimService::DimService(const char *name, const char *format, DimServiceHandler *handler)
{
	itsId = 0;
	itsName = 0;
	itsData = 0;
	itsSize = 0;
	itsType = DisPOINTER;
	declareIt((char *)name, (char *)format, handler, 0);
}

// with Dns

DimService::DimService(DimServerDns *dns, const char *name, int &value) 
{
	itsId = 0;
	itsName = 0;
	itsData = &value;
	itsSize = sizeof(int);
	itsType = DisINT;
	declareIt((char *)name, (char *)"L", 0, dns);
}

DimService::DimService(DimServerDns *dns, const char *name, float &value)
{
	itsId = 0;
	itsName = 0;
	itsData = &value;
	itsSize = sizeof(float);
	itsType = DisFLOAT;
	declareIt((char *)name, (char *)"F", 0, dns);
}

DimService::DimService(DimServerDns *dns, const char *name, double &value)
{
	itsId = 0;
	itsName = 0;
	itsData = &value;
	itsSize = sizeof(double);
	itsType = DisDOUBLE;
	declareIt((char *)name, (char *)"D", 0, dns);
}

DimService::DimService(DimServerDns *dns, const char *name, longlong &value)
{
	itsId = 0;
	itsName = 0;
	itsData = &value;
	itsSize = sizeof(longlong);
	itsType = DisXLONG;
	declareIt((char *)name, (char *)"X", 0, dns);
}

DimService::DimService(DimServerDns *dns, const char *name, short &value)
{
	itsId = 0;
	itsName = 0;
	itsData = &value;
	itsSize = sizeof(short);
	itsType = DisSHORT;
	declareIt((char *)name, (char *)"S", 0, dns);
}

DimService::DimService(DimServerDns *dns, const char *name, char *string)
{
	itsId = 0;
	itsName = 0;
	itsData = string;
	itsSize = (int)strlen(string)+1;
	itsType = DisSTRING;
	declareIt((char *)name, (char *)"C", 0, dns);
}

DimService::DimService(DimServerDns *dns, const char *name, char *format, void *structure, int size)
{
	itsId = 0;
	itsName = 0;
	itsData = structure;
	itsSize = size;
	itsType = DisPOINTER;
	declareIt((char *)name, (char *)format, 0, dns);
}

DimService::DimService(DimServerDns *dns, const char *name, char *format, DimServiceHandler *handler)
{
	itsId = 0;
	itsName = 0;
	itsData = 0;
	itsSize = 0;
	itsType = DisPOINTER;
	declareIt((char *)name, (char *)format, handler, dns);
}


DimService::DimService(DimServerDns *dns, const char *name, const char *format, void *structure, int size)
{
	itsId = 0;
	itsName = 0;
	itsData = structure;
	itsSize = size;
	itsType = DisPOINTER;
	declareIt((char *)name, (char *)format, 0, dns);
}

DimService::DimService(DimServerDns *dns, const char *name, const char *format, DimServiceHandler *handler)
{
	itsId = 0;
	itsName = 0;
	itsData = 0;
	itsSize = 0;
	itsType = DisPOINTER;
	declareIt((char *)name, (char *)format, handler, dns);
}


DimService::~DimService()
{
	DISABLE_AST
	if(itsName)
		delete[] itsName;
	if(itsDataSize)
		delete[] (char *)itsData;
//	if(itsTagId)
//		id_free(itsTagId, SRC_DIS);
	if(itsId)
		dis_remove_service( itsId );
	itsId = 0;
	ENABLE_AST
}

int DimService::updateService()
{
	if(!itsId)
		return 0;
	return dis_update_service( itsId );
}

int DimService::updateService( int &value )
{
	if(!itsId)
		return 0;
	if( itsType == DisINT)
	{
		itsData = &value;
		return dis_update_service( itsId );
	}
	return -1;
}

int DimService::updateService( float &value )
{
	if(!itsId)
		return 0;
	if( itsType == DisFLOAT) {
		itsData = &value;
		return dis_update_service( itsId );
	}
	return -1;
}

int DimService::updateService( double &value )
{
	if(!itsId)
		return 0;
	if( itsType == DisDOUBLE) {
		itsData = &value;
		return dis_update_service( itsId );
	}
	return -1;
}

int DimService::updateService( longlong &value )
{
	if(!itsId)
		return 0;
	if( itsType == DisXLONG)
	{
		itsData = &value;
		return dis_update_service( itsId );
	}
	return -1;
}

int DimService::updateService( short &value )
{
	if(!itsId)
		return 0;
	if( itsType == DisSHORT)
	{
		itsData = &value;
		return dis_update_service( itsId );
	}
	return -1;
}

int DimService::updateService( char *string )
{
	if(!itsId)
		return 0;
	if( itsType == DisSTRING)
	{
		itsData = string;
		itsSize = (int)strlen(string)+1;
		return dis_update_service( itsId );
	}
	return -1;
}
	
int DimService::updateService( void *structure, int size )
{
	if(!itsId)
		return 0;
	if( itsType == DisPOINTER)
	{
		itsData = structure;
		itsSize = size;
		return dis_update_service( itsId );
	}
	return -1;
}
	
int DimService::selectiveUpdateService(int *cids)
{
	if(!itsId)
		return 0;
	if( cids == 0)
	{
		int ids[2];
		ids[0] = DimServer::getClientId();
		ids[1] = 0;
		return dis_selective_update_service( itsId, ids );
	} 
	return dis_selective_update_service( itsId, cids );
}

int DimService::selectiveUpdateService( int &value, int *cids)
{
	if(!itsId)
		return 0;
	if( itsType == DisINT)
	{
		itsData = &value;
		if( cids == 0)
		{
			int ids[2];
			ids[0] = DimServer::getClientId();
			ids[1] = 0;
			return dis_selective_update_service( itsId, ids );
		} 
		return dis_selective_update_service( itsId, cids );
	}
	return -1;
}

int DimService::selectiveUpdateService( float &value, int *cids )
{
	if(!itsId)
		return 0;
	if( itsType == DisFLOAT)
	{
		itsData = &value;
		if( cids == 0)
		{
			int ids[2];
			ids[0] = DimServer::getClientId();
			ids[1] = 0;
			return dis_selective_update_service( itsId, ids );
		} 
		return dis_selective_update_service( itsId, cids );
	}
	return -1;
}

int DimService::selectiveUpdateService( double &value, int *cids )
{
	if(!itsId)
		return 0;
	if( itsType == DisDOUBLE)
	{
		itsData = &value;
		if( cids == 0)
		{
			int ids[2];
			ids[0] = DimServer::getClientId();
			ids[1] = 0;
			return dis_selective_update_service( itsId, ids );
		} 
		return dis_selective_update_service( itsId, cids );
	}
	return -1;
}

int DimService::selectiveUpdateService( longlong &value, int *cids )
{
	if(!itsId)
		return 0;
	if( itsType == DisXLONG) 
	{
		itsData = &value;
		if( cids == 0)
		{
			int ids[2];
			ids[0] = DimServer::getClientId();
			ids[1] = 0;
			return dis_selective_update_service( itsId, ids );
		} 
		return dis_selective_update_service( itsId, cids );
	}
	return -1;
}

int DimService::selectiveUpdateService( short &value, int *cids )
{
	if(!itsId)
		return 0;
	if( itsType == DisSHORT) 
	{
		itsData = &value;
		if( cids == 0)
		{
			int ids[2];
			ids[0] = DimServer::getClientId();
			ids[1] = 0;
			return dis_selective_update_service( itsId, ids );
		} 
		return dis_selective_update_service( itsId, cids );
	}
	return -1;
}

int DimService::selectiveUpdateService( char *string, int *cids )
{
	if(!itsId)
		return 0;
	if( itsType == DisSTRING)
	{
		itsData = string;
		itsSize = (int)strlen(string)+1;
		if( cids == 0)
		{
			int ids[2];
			ids[0] = DimServer::getClientId();
			ids[1] = 0;
			return dis_selective_update_service( itsId, ids );
		} 
		return dis_selective_update_service( itsId, cids );
	}
	return -1;
}
	
int DimService::selectiveUpdateService( void *structure, int size, int *cids )
{
	if(!itsId)
		return 0;
	if( itsType == DisPOINTER)
	{
		itsData = structure;
		itsSize = size;
		if( cids == 0)
		{
			int ids[2];
			ids[0] = DimServer::getClientId();
			ids[1] = 0;
			return dis_selective_update_service( itsId, ids );
		} 
		return dis_selective_update_service( itsId, cids );
	}
	return -1;
}
	
void DimService::setQuality(int quality)
{
	if(!itsId)
		return;
	dis_set_quality( itsId, quality );
}

void DimService::setTimestamp(int secs, int millisecs)
{ 
	if(!itsId)
		return;
	dis_set_timestamp( itsId, secs, millisecs );
}

void DimService::setData(void *data, int size)
{
	storeIt(data, size);
}

void DimService::setData(int &data)
{
	storeIt(&data, sizeof(int));
}

void DimService::setData(float &data)
{
	storeIt(&data, sizeof(float));
}

void DimService::setData(double &data)
{
	storeIt(&data, sizeof(double));
}

void DimService::setData(longlong &data)
{
	storeIt(&data, sizeof(longlong));
}

void DimService::setData(short &data)
{
	storeIt(&data, sizeof(short));
}

void DimService::setData(char *data)
{
	storeIt(data, (int)strlen(data)+1);
}

char *DimService::getName()
{
	return itsName;
}

int DimService::getTimeout(int clientId)
{
	return dis_get_timeout(itsId, clientId);
}

int DimService::getNClients()
{
	return dis_get_n_clients( itsId );
}


CmndInfo::CmndInfo(void *data, int datasize, int tsecs, int tmillisecs)
{
	itsData = new char[datasize];
	itsDataSize = datasize;
	secs = tsecs;
	millisecs = tmillisecs;
	memcpy(itsData, data, (size_t)datasize);
}

CmndInfo::~CmndInfo()
{
	delete[] (char *)itsData;
}


DimCommand::DimCommand(const char *name, char *format)
{
	declareIt( (char *)name, (char *)format, 0, 0);
}

DimCommand::DimCommand(const char *name, char *format, DimCommandHandler *handler)
{
	declareIt( (char *)name, (char *)format, handler, 0);
}

DimCommand::DimCommand(DimServerDns *dns, const char *name, char *format)
{
	declareIt( (char *)name, (char *)format, 0, dns);
}

DimCommand::DimCommand(DimServerDns *dns, const char *name, char *format, DimCommandHandler *handler)
{
	declareIt( (char *)name, (char *)format, handler, dns);
}


DimCommand::DimCommand(const char *name, const char *format)
{
	declareIt( (char *)name, (char *)format, 0, 0);
}

DimCommand::DimCommand(const char *name, const char *format, DimCommandHandler *handler)
{
	declareIt( (char *)name, (char *)format, handler, 0);
}

DimCommand::DimCommand(DimServerDns *dns, const char *name, const char *format)
{
	declareIt( (char *)name, (char *)format, 0, dns);
}

DimCommand::DimCommand(DimServerDns *dns, const char *name, const char *format, DimCommandHandler *handler)
{
	declareIt( (char *)name, (char *)format, handler, dns);
}

int DimCommand::getNext()
{
	CmndInfo *cmndptr;
	if(currCmnd)
	{
		delete currCmnd;
		currCmnd = 0;
		itsData = 0;
		itsSize = 0;
	}
	if ((cmndptr = (CmndInfo *)itsCmndList.removeHead()))
	{
		currCmnd = cmndptr;
		itsData = currCmnd->itsData;
		itsSize = currCmnd->itsDataSize;
		secs = currCmnd->secs;
		millisecs = currCmnd->millisecs;
		return(1);
	}
	return(0);
}

int DimCommand::hasNext()
{
	if ((CmndInfo *)itsCmndList.getHead())
	{
		return(1);
	}
	return(0);
}

void *DimCommand::getData()
{
	return itsData;
}

int DimCommand::getInt()
{
	return *(int *)itsData;
}

float DimCommand::getFloat()
{
	return *(float *)itsData;
}

double DimCommand::getDouble()
{
	return *(double *)itsData;
}

longlong DimCommand::getLonglong()
{
	return *(longlong *)itsData;
}

short DimCommand::getShort()
{
	return *(short *)itsData;
}

char *DimCommand::getString()
{
	return (char *)itsData;
}

int DimCommand::getSize()
{
	return itsSize;
}

char *DimCommand::getFormat()
{
	return itsFormat;
}

int DimCommand::getTimestamp()
{

	if(secs == 0)
	{
		DISABLE_AST
		if(itsId)
			dis_get_timestamp(itsId, &secs, &millisecs);
		ENABLE_AST
	}
	return(secs);
}

int DimCommand::getTimestampMillisecs()
{
	return(millisecs);
}

void DimCommand::commandHandler() 
{
	CmndInfo *cmndptr;
	int tsecs, tmillisecs;

	tsecs = getTimestamp();
	tmillisecs = getTimestampMillisecs();
	cmndptr = new CmndInfo(getData(), getSize(), tsecs, tmillisecs);
	itsCmndList.add(cmndptr);
}

char *DimCommand::getName()
{
	return itsName;
}

DimCommand::~DimCommand()
{
	DISABLE_AST
	delete[] itsName;
	delete[] itsFormat;
//	if(itsTagId)
//		id_free(itsTagId, SRC_DIS);
	if(itsId)
		dis_remove_service( itsId );
	itsId = 0;
	ENABLE_AST
}

DimRpc::DimRpc()
{
}

DimRpc::DimRpc(const char *name, const char *formatin, const char *formatout)
{
	declareIt( (char *)name, (char *)formatin, (char *)formatout, 0);
}

DimRpc::DimRpc(DimServerDns *dns, const char *name, const char *formatin, const char *formatout)
{
	declareIt( (char *)name, (char *)formatin, (char *)formatout, dns);
}

DimRpc::~DimRpc()
{
	DISABLE_AST
	delete[] itsName;
	delete[] itsNameIn;
	delete[] itsNameOut;
//	if(itsTagId)
//		id_free(itsTagId, SRC_DIS);
	if(itsIdIn)
		dis_remove_service( itsIdIn );
	if(itsIdOut)
		dis_remove_service( itsIdOut );
	itsIdIn = 0;
	itsIdOut = 0;
	ENABLE_AST
}

void *DimRpc::getData()
{
	return itsDataIn;
}

int DimRpc::getInt()
{
	return *(int *)itsDataIn;
}

float DimRpc::getFloat()
{
	return *(float *)itsDataIn;
}

double DimRpc::getDouble()
{
	return *(double *)itsDataIn;
}

longlong DimRpc::getLonglong()
{
	return *(longlong *)itsDataIn;
}

short DimRpc::getShort()
{
	return *(short *)itsDataIn;
}

char *DimRpc::getString()
{
	return (char *)itsDataIn;
}

int DimRpc::getSize()
{
	return itsSizeIn;
}

void DimRpc::setData(void *data, int size)
{
	storeIt(data,size);
}

void DimRpc::setData(int &data)
{
	storeIt(&data,sizeof(int));
}

void DimRpc::setData(float &data)
{
	storeIt(&data,sizeof(float));
}

void DimRpc::setData(double &data)
{
	storeIt(&data,sizeof(double));
}

void DimRpc::setData(longlong &data)
{
	storeIt(&data,sizeof(longlong));
}

void DimRpc::setData(short &data)
{
	storeIt(&data,sizeof(short));
}

void DimRpc::setData(char *data)
{
	storeIt(data,(int)strlen(data)+1);
}

char *DimRpc::getName()
{
	return itsName;
}
