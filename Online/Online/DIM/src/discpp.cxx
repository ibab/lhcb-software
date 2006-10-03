#define DIMLIB
#include <dis.hxx>
#include "tokenstring.hxx"

DimClientExitHandler *DimServer::itsClientExit = 0;
DimExitHandler *DimServer::itsExit = 0;
DimErrorHandler *DimServer::itsSrvError = 0;
char *DimServer::itsName = 0;
char *DimServer::clientName = 0;
char *DimServer::dimDnsNode = 0;
int DimServer::autoStart = 1;

extern "C" {
static void user_routine( void *tagp, void **buf, int *size, int *first_time)
{
//	int *tag = (int *)tagp;
//	int id = *tag;
	DimService *t;

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
			t->itsSize = strlen((char *)t->itsData)+1;
	*buf = t->itsData;
	*size = t->itsSize;
}
}

void DimService::declareIt(char *name, char *format, DimServiceHandler *handler)
{
//	itsTagId = 0;
	itsName = new char[strlen(name)+1];
	itsDataSize = 0;
	strcpy( itsName, name);
	if(handler)
		itsServiceHandler = handler;
	else
		itsServiceHandler = 0;
//	itsTagId = id_get((void *)this, SRC_DIS);
	itsId = dis_add_service( name, format, NULL, 0, 
//			user_routine, itsTagId);
			user_routine, (long)this);

	DimServer::start();
}

void DimService::storeIt(void *data, int size)
{

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
	memcpy(itsData, data, size);
	itsSize = size;
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

void DimCommand::declareIt(char *name, char *format, DimCommandHandler *handler)
{
//	itsTagId = 0;
	itsName = new char[strlen(name)+1];
	strcpy( itsName, name);
	itsFormat = new char[strlen(format)+1];
	strcpy( itsFormat, format);
	currCmnd = 0;
	if(handler)
		itsCommandHandler = handler;
	else
		itsCommandHandler = 0;
//	itsTagId = id_get((void *)this, SRC_DIS);
	itsId = dis_add_cmnd( name, format, command_routine,
//			itsTagId);
			(long)this);
	DimServer::start();
}

extern "C" {
static void rpcin_routine( void *tagp, void *buf, int *size)
{
//	int *tag = (int *)tagp;
//	int id = *tag;
	DimRpc *t;
	int ids[2];

//	t = (DimRpc *)id_get_ptr(id, SRC_DIS);
	t = *(DimRpc **)tagp;
	t->itsDataIn = buf;
	t->itsSizeIn = *size;
	DimCore::inCallback = 2;
	t->rpcHandler();
	DimCore::inCallback = 0;
	t->itsDataIn = 0;
	t->itsSizeIn = 0;
	ids[0] = dis_get_conn_id();
	ids[1] = 0;
	dis_selective_update_service(t->itsIdOut, ids);
}
}

extern "C" {
static void rpcout_routine( void *tagp, void **buf, int *size, int *firt_time)
{
//	int *tag = (int *)tagp;
//	int id = *tag;
	DimRpc *t;

//	t = (DimRpc *)id_get_ptr(id, SRC_DIS);
	t = *(DimRpc**)tagp;
	*buf = t->itsDataOut;
	*size = t->itsSizeOut;
}
}

void DimRpc::declareIt(char *name, char *formatin, char *formatout)
{
//	itsTagId = 0;
	itsName = new char[strlen(name)+1];
	strcpy( itsName, name);
	itsNameIn = new char[strlen(name)+1+10];
	strcpy( itsNameIn, name);
	strcat(itsNameIn,(char *)"/RpcIn");
	itsNameOut = new char[strlen(name)+1+10];
	strcpy( itsNameOut, name);
	strcat(itsNameOut,(char *)"/RpcOut");
	itsDataOut = new char[1];
	itsDataOutSize = itsSizeOut = 1;
	
//	itsTagId = id_get((void *)this, SRC_DIS);
	itsIdIn = dis_add_cmnd( itsNameIn, formatin, 
//		rpcin_routine, itsTagId);
		rpcin_routine, (long)this);
	itsIdOut = dis_add_service( itsNameOut, formatout, 0,0, 
//		rpcout_routine, itsTagId);
		rpcout_routine, (long)this);
	DimServer::start();
}

void DimRpc::storeIt(void *data, int size)
{

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
	memcpy(itsDataOut, data, size);
	itsSizeOut = size;
}

extern "C" {
static void client_exit_user_routine(int*);
static void exit_user_routine(int*);
static void srv_error_user_routine(int, int, char*);
}

DimServer::DimServer()
{
	itsClientExit = this; 
	itsExit = this;
	itsSrvError = this;
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
		itsName = new char[strlen(name)+1];
		strcpy(itsName,name);
	}
	dis_start_serving(itsName);
}

void DimServer::start()
{
	if((itsName) && (autoStart))
		dis_start_serving(itsName);
}

void DimServer::stop()
{
	dis_stop_serving();
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
	DimServer::itsClientExit = handler;
	dis_add_client_exit_handler(client_exit_user_routine);
}

void DimServer::addClientExitHandler()
{
	DimServer::itsClientExit = this;
	dis_add_client_exit_handler(client_exit_user_routine);
}

void DimServer::addExitHandler(DimExitHandler *handler)
{
	DimServer::itsExit = handler;
	dis_add_exit_handler(exit_user_routine);
}

void DimServer::addErrorHandler(DimErrorHandler *handler)
{
	DimServer::itsSrvError = handler;
	dis_add_error_handler(srv_error_user_routine);
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
	DimServer::itsSrvError->errorHandler(severity, code, msg);
	DimCore::inCallback = 0;
}

}


DimService::DimService()
{
//	itsTagId = 0;
}

DimService::DimService(const char *name, int &value) 
{
	itsData = &value;
	itsSize = sizeof(int);
	itsType = DisINT;
	declareIt((char *)name, (char *)"L", 0);
}

DimService::DimService(const char *name, float &value)
{
	itsData = &value;
	itsSize = sizeof(float);
	itsType = DisFLOAT;
	declareIt((char *)name, (char *)"F", 0);
}

DimService::DimService(const char *name, double &value)
{
	itsData = &value;
	itsSize = sizeof(double);
	itsType = DisDOUBLE;
	declareIt((char *)name, (char *)"D", 0);
}

DimService::DimService(const char *name, longlong &value)
{
	itsData = &value;
	itsSize = sizeof(longlong);
	itsType = DisXLONG;
	declareIt((char *)name, (char *)"X", 0);
}

DimService::DimService(const char *name, short &value)
{
	itsData = &value;
	itsSize = sizeof(short);
	itsType = DisSHORT;
	declareIt((char *)name, (char *)"S", 0);
}

DimService::DimService(const char *name, char *string)
{
	itsData = string;
	itsSize = strlen(string)+1;
	itsType = DisSTRING;
	declareIt((char *)name, (char *)"C", 0);
}

DimService::DimService(const char *name, char *format, void *structure, int size)
{
	itsData = structure;
	itsSize = size;
	itsType = DisPOINTER;
	declareIt((char *)name, format, 0);
}

DimService::DimService(const char *name, char *format, DimServiceHandler *handler)
{
	itsData = 0;
	itsSize = 0;
	itsType = DisPOINTER;
	declareIt((char *)name, format, handler);
}

DimService::~DimService()
{
	delete[] itsName;
//	if(itsTagId)
//		id_free(itsTagId, SRC_DIS);
	dis_remove_service( itsId );
}

int DimService::updateService()
{
	return dis_update_service( itsId );
}

int DimService::updateService( int &value )
{
	if( itsType == DisINT)
	{
		itsData = &value;
		return dis_update_service( itsId );
	}
	return -1;
}

int DimService::updateService( float &value )
{
	if( itsType == DisFLOAT) {
		itsData = &value;
		return dis_update_service( itsId );
	}
	return -1;
}

int DimService::updateService( double &value )
{
	if( itsType == DisDOUBLE) {
		itsData = &value;
		return dis_update_service( itsId );
	}
	return -1;
}

int DimService::updateService( longlong &value )
{
	if( itsType == DisXLONG)
	{
		itsData = &value;
		return dis_update_service( itsId );
	}
	return -1;
}

int DimService::updateService( short &value )
{
	if( itsType == DisSHORT)
	{
		itsData = &value;
		return dis_update_service( itsId );
	}
	return -1;
}

int DimService::updateService( char *string )
{
	if( itsType == DisSTRING)
	{
		itsData = string;
		itsSize = strlen(string)+1;
		return dis_update_service( itsId );
	}
	return -1;
}
	
int DimService::updateService( void *structure, int size )
{
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
	if( itsType == DisSTRING)
	{
		itsData = string;
		itsSize = strlen(string)+1;
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
	dis_set_quality( itsId, quality );
}

void DimService::setTimestamp(int secs, int millisecs)
{ 
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
	storeIt(data, strlen(data)+1);
}

char *DimService::getName()
{
	return itsName;
}

int DimService::getTimeout(int clientId)
{
	return dis_get_timeout(itsId, clientId);
}

CmndInfo::CmndInfo(void *data, int datasize)
{
	itsData = new char[datasize];
	itsDataSize = datasize;
	memcpy(itsData, data, datasize);
}

CmndInfo::~CmndInfo()
{
	delete[] (char *)itsData;
}

DimCommand::DimCommand(const char *name, char *format)
{
	declareIt( (char *)name, format, 0);
}

DimCommand::DimCommand(const char *name, char *format, DimCommandHandler *handler)
{
	declareIt( (char *)name, format, handler);
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

void DimCommand::commandHandler() 
{
	CmndInfo *cmndptr;
	cmndptr = new CmndInfo(getData(), getSize());
	itsCmndList.add(cmndptr);
}

char *DimCommand::getName()
{
	return itsName;
}

DimCommand::~DimCommand()
{
	delete[] itsName;
	delete[] itsFormat;
//	if(itsTagId)
//		id_free(itsTagId, SRC_DIS);
	dis_remove_service( itsId );
}

DimRpc::DimRpc()
{
}

DimRpc::DimRpc(const char *name, const char *formatin, const char *formatout)
{
	declareIt( (char *)name, (char *)formatin, (char *)formatout);
}

DimRpc::~DimRpc()
{
	delete[] itsName;
	delete[] itsNameIn;
	delete[] itsNameOut;
//	if(itsTagId)
//		id_free(itsTagId, SRC_DIS);
	dis_remove_service( itsIdIn );
	dis_remove_service( itsIdOut );
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
	storeIt(data,strlen(data)+1);
}

char *DimRpc::getName()
{
	return itsName;
}
