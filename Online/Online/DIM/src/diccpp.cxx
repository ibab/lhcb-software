#define DIMLIB
#include <dic.hxx>
#include <stdio.h>

char *DimClient::dimDnsNode = 0;
DimErrorHandler *DimClient::itsCltError = 0;
char *DimClient::serverName = 0;
int DimClient::dicNoCopy = 0;

extern "C" {
static void user_routine(void *tagp, void *bufp, int *size)
{
//	int *tag = (int *)tagp;
	char *buf = (char *)bufp;
//	int id = *tag;
	DimInfo *t;

//	t = (DimInfo *)id_get_ptr(id, SRC_DIC);
	t = * (DimInfo **)tagp;
	if(DimClient::getNoDataCopy() == 0)
	{
		if(!t->itsDataSize)
		{
			t->itsData = new char[*size];
			t->itsDataSize = *size;
		}
		else if(t->itsDataSize < *size)
		{
			delete[] (char *)(t->itsData);
			t->itsData = new char[*size];
			t->itsDataSize = *size;
		}
		memcpy(t->itsData, buf, *size);
	}
	else
	{
		t->itsData = buf;
	}
	t->itsSize = *size;
	if(t->itsHandler)
	{
		t->itsHandler->itsService = t;
		DimCore::inCallback = 1;
		t->itsHandler->infoHandler();
		DimCore::inCallback = 0;
	}
	else
	{
		DimCore::inCallback = 1;
		t->infoHandler();
		DimCore::inCallback = 0;
	}
}
}

void DimInfo::infoHandler()
{
	char *data;
	if(DimClient::getNoDataCopy() == 1)
	{
		data = (char *)itsData;
		if(!itsDataSize)
		{
			itsData = new char[itsSize];
			itsDataSize = itsSize;
		}
		else if(itsDataSize < itsSize)
		{
			delete[] (char *)(itsData);
			itsData = new char[itsSize];
			itsDataSize = itsSize;
		}
		memcpy(itsData, data, itsSize);
	}
}

void DimInfo::doIt()
{
	dim_init();
	DISABLE_AST

//	itsTagId = id_get((void *)this, SRC_DIC);
	itsId = dic_info_service(itsName,itsType,itsTime, 0, 0,
//		user_routine, itsTagId, 
		user_routine, (long)this, 
		itsNolinkBuf, itsNolinkSize);
	ENABLE_AST
}

int DimInfo::getQuality()
{
	return dic_get_quality(itsId);
}

int DimInfo::getTimestamp()
{

	dic_get_timestamp(itsId, &secs, &millisecs);
	return(secs);
}

int DimInfo::getTimestampMillisecs()
{
	return(millisecs);
}


char *DimInfo::getFormat()
{
	char *def;
	int len = 0, new_len;

	if(itsFormat)
	{
		len = strlen(itsFormat)+1;
		if(len > 1)
			return itsFormat;
	}
	def = dic_get_format(itsId);
	new_len = strlen(def)+1;
	if(new_len > len)
	{
		if(itsFormat)
			delete[] itsFormat;
		itsFormat = new char[strlen(def)+1];
	}
	strcpy(itsFormat, def);
	return itsFormat;
}

void DimInfo::timerHandler()
{
//	itsTagId = id_get((void *)this, SRC_DIC);
	itsId = dic_info_service(itsName,itsType,itsTime, 0, 0,
//		user_routine, itsTagId, 
		user_routine, (long)this, 
		itsNolinkBuf, itsNolinkSize);
}

void DimInfo::subscribe(char *name, int time, void *nolink, int nolinksize,
	DimInfoHandler *handler)
{
	itsId = 0;
	itsData = 0;
	itsFormat = 0;
	itsHandler = handler;
	itsDataSize = 0;
	itsSize = 0;
	itsNolinkBuf = 0;
	itsNolinkSize = 0;
	itsName = 0;
	if(!name)
	{
		return;
	}
	itsName = new char[strlen(name)+1];
	strcpy(itsName,name);
	if(nolinksize > 0)
	{
		itsNolinkBuf = new char[nolinksize];
		itsNolinkSize = nolinksize;
		memcpy(itsNolinkBuf, nolink, nolinksize);
	}
	if(!time)
	{
		itsType = MONITORED;
		itsTime = 0;
	}	
	else if(time > 0)
	{
		itsType = MONITORED;
		itsTime = time;
	}
	else
	{
		itsType = ONCE_ONLY;
		itsTime = 30;
	}
	doIt();
}


DimInfo::~DimInfo()
{
//	if(itsTagId)
//		id_free(itsTagId, SRC_DIC);
	if(itsId)
		dic_release_service(itsId);
	if(itsNolinkSize)
		delete[] (char *)itsNolinkBuf;
	if(itsDataSize)
		delete[] (char *)itsData;
	if(itsName)
		delete[] itsName;
	if(itsFormat)
		delete[] itsFormat;
}

void *DimInfo::getData()
{
//	if(!this->itsSize)
//		return itsNolinkBuf;
/*
	if(DimClient::getNoDataCopy() == 1)
	{
		if(!DimCore::inCallback)
			return (void *)0;
	}
*/
	return this->itsData;
}

void DimStampedInfo::doIt()
{
	dim_init();
	DISABLE_AST
//	itsTagId = id_get((void *)this, SRC_DIC);
	itsId = dic_info_service_stamped(itsName,itsType,itsTime, 0, 0,
//		user_routine, itsTagId, 
		user_routine, (long)this, 
		itsNolinkBuf, itsNolinkSize);
	ENABLE_AST
}

void DimStampedInfo::subscribe(char *name, int time, void *nolink, int nolinksize,
	DimInfoHandler *handler)
{
	itsId = 0;
	itsData = 0;
	itsFormat = 0;
	itsHandler = handler;
	itsDataSize = 0;
	itsSize = 0;
	itsNolinkBuf = 0;
	itsNolinkSize = 0;
	itsName = 0;
	if(!name)
	{
		return;
	}
	itsName = new char[strlen(name)+1];
	strcpy(itsName,name);
	if(nolinksize > 0)
	{
		itsNolinkBuf = new char[nolinksize];
		itsNolinkSize = nolinksize;
		memcpy(itsNolinkBuf, nolink, nolinksize);
	}
	if(!time)
	{
		itsType = MONITORED;
		itsTime = 0;
	}	
	else if(time > 0)
	{
		itsType = MONITORED;
		itsTime = time;
	}
	else
	{
		itsType = ONCE_ONLY;
		itsTime = 30;
	}
	doIt();
}

DimStampedInfo::~DimStampedInfo()
{
}

void DimUpdatedInfo::doIt()
{
	dim_init();
	DISABLE_AST
//	itsTagId = id_get((void *)this, SRC_DIC);
	itsId = dic_info_service_stamped(itsName,itsType,itsTime, 0, 0,
//		user_routine, itsTagId, 
		user_routine, (long)this, 
		itsNolinkBuf, itsNolinkSize);
	ENABLE_AST
}

void DimUpdatedInfo::subscribe(char *name, int time, void *nolink, int nolinksize,
	DimInfoHandler *handler)
{
	itsId = 0;
	itsData = 0;
	itsFormat = 0;
	itsHandler = handler;
	itsDataSize = 0;
	itsSize = 0;
	itsNolinkBuf = 0;
	itsNolinkSize = 0;
	itsName = 0;
	if(!name)
	{
		return;
	}
	itsName = new char[strlen(name)+1];
	strcpy(itsName,name);
	if(nolinksize > 0)
	{
		itsNolinkBuf = new char[nolinksize];
		itsNolinkSize = nolinksize;
		memcpy(itsNolinkBuf, nolink, nolinksize);
	}
	if(!time)
	{
		itsType = MONIT_ONLY;
		itsTime = 0;
	}	
	else if(time > 0)
	{
		itsType = UPDATE;
		itsTime = time;
	}
	doIt();
}

DimUpdatedInfo::~DimUpdatedInfo()
{
}

extern "C" {
static void data_user_routine(void *tagp, void *bufp, int *size)
{
//	int *tag = (int *)tagp;
	char *buf = (char *)bufp;
//	int id = *tag;
	DimCurrentInfo *t;

//	t = (DimCurrentInfo *)id_get_ptr(id, SRC_DIC);
	t = * (DimCurrentInfo **)tagp;
	if(!t->itsDataSize)
	{
		t->itsData = new char[*size];
		t->itsDataSize = *size;
	}
	else if(t->itsDataSize < *size)
	{
		delete[] (char *)(t->itsData);
		t->itsData = new char[*size];
		t->itsDataSize = *size;
	}
	memcpy(t->itsData, buf, *size);
	t->itsSize = *size;
	t->wakeUp = 1;
#ifdef __VMS
	sys$wake(0,0);
#endif
#ifdef WIN32
	wake_up();
#endif
}
}

void DimCurrentInfo::subscribe(char *name, int time, void *nolink, int nolinksize)
{

	int timeout;

//	itsTagId = 0;
//	itsId = 0;
	itsData = 0;
//	itsFormat = 0;
//	itsHandler = handler;
	itsDataSize = 0;
	itsSize = 0;
	itsNolinkBuf = 0;
	itsNolinkSize = 0;
	itsName = 0;
	if(!name)
	{
		return;
	}
	itsName = new char[strlen(name)+1];
	strcpy(itsName,name);
	if(nolinksize > 0)
	{
		itsNolinkBuf = new char[nolinksize];
		itsNolinkSize = nolinksize;
		memcpy(itsNolinkBuf, nolink, nolinksize);
	}
	if(!time)
		timeout = 10;
	else
		timeout = time;
	wakeUp = 0;
//	itsTagId = id_get((void *)this, SRC_DIC);
	dic_info_service(itsName,ONCE_ONLY,timeout, 0, 0,
//		data_user_routine, itsTagId, 
		data_user_routine, (long)this, 
		itsNolinkBuf, itsNolinkSize);
}

DimCurrentInfo::~DimCurrentInfo()
{
//	if(itsId)
//		dic_release_service(itsId);
	if(itsNolinkSize)
		delete[] (char *)itsNolinkBuf;
	if(itsDataSize)
		delete[] (char *)itsData;
	if(itsName)
		delete[] itsName;
//	if(itsFormat)
//		delete[] itsFormat;
/*
	delete[] (char *)itsNolinkBuf;

//	if(itsTagId)
//		id_free(itsTagId, SRC_DIC);
	if(itsDataSize)
		delete[] (char *)itsData;
	delete[] itsName;
*/
}

void *DimCurrentInfo::getData()
{
	while(!wakeUp)
	{
#ifdef __VMS
		sys$hiber();
#else
		dim_wait();
#endif
	}
	return this->itsData;
}

extern "C" {
static void cmnd_done(void *tagp, int *result)
{
//	int *tag = (int *)tagp;
//	int id = *tag;
	DimCmnd *t;

//	t = (DimCmnd *)id_get_ptr(id, SRC_DIC);
	t = *(DimCmnd **)tagp;
	t->result = *result;
	t->wakeUp = 1;
#ifdef __VMS
	sys$wake(0,0);
#endif
#ifdef WIN32
	wake_up();
#endif
}
}

int DimCmnd::send(char *name, void *data, int datasize) 
{
//	int id;
	if(DimCore::inCallback) 
	{
		dic_cmnd_service(name, data, datasize);
		return(1);
	}
	else
	{
		wakeUp = 0;
//		id = id_get((void *)this, SRC_DIC);
		dic_cmnd_callback(name, data, datasize, 
//			cmnd_done, id);
			cmnd_done, (long)this);
		while(!wakeUp)
		{
#ifdef __VMS
			sys$hiber();
#else
			dim_wait();
#endif
		}
//		id_free(id, SRC_DIC);
		return(result);
	}
}

void DimCmnd::sendNB(char *name, void *data, int datasize) 
{
	dic_cmnd_service(name, data, datasize);
}

extern "C" {
static void rpc_user_routine(void *tagp, void *bufp, int *sizep)
{
//	int *tag = (int *)tagp;
	char *buf = (char *)bufp;
	int size = *sizep;
//	int id = *tag;
	DimRpcInfo *t;
	int quality;

//	t = (DimRpcInfo *)id_get_ptr(id, SRC_DIC);
	t = *(DimRpcInfo **)tagp;
	quality = dic_get_quality(0);
	if(quality == -1)
	{
		buf = (char *)t->itsNolinkBuf;
		size = t->itsNolinkSize;
	}
	if(DimClient::getNoDataCopy() == 0)
	{
		if(!t->itsDataSize)
		{
			t->itsData = new char[size];
			t->itsDataSize = size;
		}
		else if(t->itsDataSize < size)
		{
			delete[] (char *)(t->itsData);
			t->itsData = new char[size];
			t->itsDataSize = size;
		}
	}
	if(!t->itsConnected)
	{
		t->itsConnected = 1;
	}
	if(t->itsWaiting)
	{
		t->stop();
//dim_print_date_time();
//printf("DIM RPC: Stopped Timer, Data Received for %s\n", t->getName());
		if(DimClient::getNoDataCopy() == 0)
			memcpy(t->itsData, buf, size);
		else
			t->itsData = buf;
		t->itsSize = size;
		t->wakeUp = 1;
		if(t->itsInit)
		{
			t->itsWaiting = 1;
			t->itsHandler->rpcInfoHandler();
		}
		if(t->itsWaiting != 2)
			t->itsWaiting = 0;
	}
#ifdef __VMS
	sys$wake(0,0);
#endif
#ifdef WIN32
	wake_up();
#endif
}
}

void DimRpcInfo::timerHandler()
{
	char *buf;
	int size;
		
	buf = (char *)itsNolinkBuf;
	size = itsNolinkSize;

	if(DimClient::getNoDataCopy() == 0)
	{
		if(!itsDataSize)
		{
			itsData = new char[size];
			itsDataSize = size;
		}
		else if(itsDataSize < size)
		{
			delete[] (char *)(itsData);
			itsData = new char[size];
			itsDataSize = size;
		}
	}
	if(itsWaiting)
	{
		if(DimClient::getNoDataCopy() == 0)
			memcpy(itsData, buf, size);
		else
			itsData = buf;
		itsSize = size;
//dim_print_date_time();
//printf("DIM RPC: Timer fired, No Data Received for %s\n", itsName);
		wakeUp = 1;
		if(itsInit)
		{
			itsWaiting = 1;
			itsHandler->rpcInfoHandler();
		}
		if(itsWaiting != 2)
			itsWaiting = 0;
	}
#ifdef __VMS
	sys$wake(0,0);
#endif
#ifdef WIN32
	wake_up();
#endif
}

void DimRpcInfo::rpcInfoHandler()
{
	char *data;
	if(DimClient::getNoDataCopy() == 1)
	{
		data = (char *)itsData;
		if(!itsDataSize)
		{
			itsData = new char[itsSize];
			itsDataSize = itsSize;
		}
		else if(itsDataSize < itsSize)
		{
			delete[] (char *)(itsData);
			itsData = new char[itsSize];
			itsDataSize = itsSize;
		}
		memcpy(itsData, data, itsSize);
	}
}

void DimRpcInfo::subscribe(char *name, void *data, int size,
	void *nolink, int nolinksize, int timeout)
{

	itsId = 0;
//	itsTagId = 0;
	itsInit = 0;
	itsWaiting = 0;
	itsName = new char[strlen(name)+1];
	strcpy(itsName,name);
	itsHandler = this;
	itsDataSize = 0;
	itsData = 0;
	itsDataOutSize = 0;
	itsDataOut = 0;
	itsNolinkBuf = new char[nolinksize];
	itsNolinkSize = nolinksize;
	memcpy(itsNolinkBuf, nolink, nolinksize);
	itsNameOut = new char[strlen(name)+1+10];
	strcpy(itsNameOut,name);
	strcat(itsNameOut,(char *)"/RpcIn");
	itsNameIn = new char[strlen(name)+1+10];
	strcpy(itsNameIn,name);
	strcat(itsNameIn,(char *)"/RpcOut");
	itsTimeout = timeout;
	dim_init();
	{
		if(!itsId)
		{
//			itsTagId = id_get((void *)itsHandler, SRC_DIC);

//			itsId = dic_info_service_stamped(itsNameIn,MONIT_ONLY,itsTimeout, 
			itsConnected = 0;
//			itsId = dic_info_service_stamped(itsNameIn,MONITORED,itsTimeout, 
			itsId = dic_info_service_stamped(itsNameIn,MONIT_FIRST,itsTimeout, 
				0, 0,
//				rpc_user_routine, itsTagId, 
				rpc_user_routine, (long)itsHandler, 
				itsNolinkBuf, itsNolinkSize);
//			dim_usleep(200000);
			itsInit = 1;
		}
	}
	if(size)
	{
		doIt(data, size);
	}
}
	
void DimRpcInfo::doIt(void *data, int size)
{
	int ret;

	wakeUp = 0;
	if(DimClient::getNoDataCopy() == 0)
	{
		if(!itsDataOut)
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
	}
	else
	{
		itsDataOut = data;
	}
	while(!itsConnected)
		dim_wait();
	itsWaiting = 1;
	if(itsTimeout)
		start(itsTimeout);
//dim_print_date_time();
//printf("DIM RPC: Started Timer for %s - %d secs\n", itsName, itsTimeout);
	ret = DimClient::sendCommand(itsNameOut, itsDataOut, size); 
	if(!ret)
	{
		if(itsTimeout)
			stop();
//dim_print_date_time();
//printf("DIM RPC: Stopped Timer, Command failed for %s\n", itsName);
//		rpc_user_routine((int *)&itsTagId, itsNolinkBuf, &itsNolinkSize);
		rpc_user_routine((long *)&itsHandler, itsNolinkBuf, &itsNolinkSize);
	}
/*
	else
	{
		if(itsTimeout)
			start(itsTimeout);
	}
*/
}

void *DimRpcInfo::getData()
{
	while(!wakeUp)
	{
#ifdef __VMS
		sys$hiber();
#else
		dim_wait();
#endif
	}
/*
	if(DimClient::getNoDataCopy() == 1)
	{
		if(!DimCore::inCallback)
			return (void *)0;
	}
*/
	return this->itsData;
}

DimRpcInfo::~DimRpcInfo() 
{
//	if(itsTagId)
//		id_free(itsTagId, SRC_DIC);
//dim_print_date_time();
//printf("DIM RPC: Deleting RPC and Timer for %s\n", itsName);
	if(itsId)
		dic_release_service(itsId);
	delete[] (char *)itsNolinkBuf;
	if(itsDataSize)
		delete[] (char *)itsData;
	if(itsDataOutSize)
		delete[] (char *)itsDataOut;
	delete[] itsName;
	delete[] itsNameIn;
	delete[] itsNameOut;
}

DimBrowser::DimBrowser()
{
	int i;
	for(i = 0; i<5; i++)
	{
		itsData[i] = 0;
		itsData[i] = 0;
	}
	currIndex = -1;
	none = 0;
	browserRpc = 0;
}

DimBrowser::~DimBrowser()
{
	int i;
	for(i = 0; i<5; i++)
	{
		if(itsData[i])
			delete itsData[i];
	}
	if(browserRpc)
		delete browserRpc;
}

int DimBrowser::getServices(const char * serviceName) 
{
	return getServices(serviceName, 0);
}

int DimBrowser::getServices(const char * serviceName, int timeout) 
{
	char *str;

//	DimRpcInfo rpc((char *)"DIS_DNS/SERVICE_INFO",(char *)"\0");
//	rpc.setData((char *)serviceName);
//	str = rpc.getString();
	if(!browserRpc)
		browserRpc = new DimRpcInfo((char *)"DIS_DNS/SERVICE_INFO",timeout,(char *)"\0");
	browserRpc->setData((char *)serviceName);
	str = browserRpc->getString();	
	if(itsData[0])
		delete itsData[0];
	itsData[0] = new TokenString(str,(char *)"|\n"); 
	currIndex = 0;
	if(!itsData[0]->getNTokens())
		return(0);
	return( itsData[0]->getNTokens((char *)"\n") + 1); 
}

int DimBrowser::getServers() 
{
	return getServers(0);
}

int DimBrowser::getServers(int timeout) 
{
	char *str, *pid_str;
	int size, totsize;
	DimCurrentInfo srv((char *)"DIS_DNS/SERVER_LIST", timeout, (char *)"\0");
	str = srv.getString();
	size = strlen(str)+1;
	totsize = srv.getSize();

	if(itsData[1])
		delete itsData[1];
	itsData[1] = new TokenString(str,(char *)"|@\n"); 
	currIndex = 1;
	if(!str[0])
		return(0);
	if(totsize > size)
	{
		pid_str = str + strlen(str) + 1;
		if(itsData[4])
			delete itsData[4];
		itsData[4] = new TokenString(pid_str,(char *)"|"); 
	}
	return(itsData[1]->getNTokens((char *)"|") +1); 
}

int DimBrowser::getServerServices(const char *serverName) 
{
	return getServerServices(serverName, 0);
}

int DimBrowser::getServerServices(const char *serverName, int timeout) 
{
	char *str;
	char *name = new char[strlen(serverName) + 20];
	strcpy(name,(char *)serverName);
	strcat(name,(char *)"/SERVICE_LIST");
	DimCurrentInfo srv(name, timeout, (char *)"\0");
	delete[] name;
	str = srv.getString();	
	if(itsData[2])
		delete itsData[2];
	itsData[2] = new TokenString(str,(char *)"|\n"); 
	currIndex = 2;
	if(!itsData[2]->getNTokens())
		return(0);
	return(itsData[2]->getNTokens((char *)"\n") + 1); 
}

int DimBrowser::getServerClients(const char *serverName)
{
	return getServerClients(serverName, 0);
}

int DimBrowser::getServerClients(const char *serverName, int timeout) 
{
	char *str;
	char *name = new char[strlen(serverName) + 20];
	strcpy(name,(char *)serverName);
	strcat(name,(char *)"/CLIENT_LIST");
	DimCurrentInfo srv(name, timeout, (char *)"\0");
	delete[] name;
	str = srv.getString();	
	if(itsData[3])
		delete itsData[3];
	itsData[3] = new TokenString(str,(char *)"|@\n"); 
	currIndex = 3;
	return(itsData[3]->getNTokens((char *)"@") ); 
}
	
int DimBrowser::getNextService(char *&service, char *&format)
{
	int ret, type;

	service = format = &none;
	ret = itsData[0]->getToken(currToken);
	if(!ret) return 0;
	service = currToken;
	ret = itsData[0]->getToken(currToken);
	if(!itsData[0]->cmpToken((char *)"|"))
		return 0;
	ret = itsData[0]->getToken(currToken);
	if(!itsData[0]->cmpToken((char *)"|"))
	{
		format = currToken;
		ret = itsData[0]->getToken(currToken);
		if(!itsData[0]->cmpToken((char *)"|"))      
			return 0;
	}
	ret = itsData[0]->getToken(currToken);
	type = DimSERVICE;
	if(!itsData[0]->cmpToken((char *)"\n"))
	{
		if(itsData[0]->cmpToken((char *)"CMD"))
			type = DimCOMMAND;
		if(itsData[0]->cmpToken((char *)"RPC"))
			type = DimRPC;
		ret = itsData[0]->getToken(currToken);
		if(!itsData[0]->cmpToken((char *)"\n"))
			return 0;
	}
	return type;
}
	
int DimBrowser::getNextServer(char *&server, char *&node)
{
	int ret;

	server = node = &none;
	ret = itsData[1]->getToken(currToken);
	if(!ret) return 0;
	server = currToken;
	ret = itsData[1]->getToken(currToken);
	if(!itsData[1]->cmpToken((char *)"@"))
		return 0;
	while(1)
	{
		ret = itsData[1]->getToken(currToken);
		node = currToken;
		itsData[1]->pushToken();
		ret = itsData[1]->getToken(currToken);
		if(itsData[1]->cmpToken((char *)"@"))
		{
			strcat(server,"@");
			strcat(server,node);
		}
		else
			break;
	}
	if(!itsData[1]->cmpToken((char *)"|"))
		itsData[1]->popToken();
	return 1;
}

int DimBrowser::getNextServer(char *&server, char *&node, int &pid)
{
	int ret, lpid = 0;
	char *tok;

	ret = getNextServer(server, node);
	if(ret && itsData[4])
	{
		ret = itsData[4]->getToken(tok);
		if(ret)
		{
			sscanf(tok,"%d",&lpid);
			pid = lpid;
		}
	}
	if(!ret) 
		return 0;
	ret = itsData[4]->getToken(tok);
	return 1;
}
	
int DimBrowser::getNextServerService(char *&service, char *&format)
{
	int ret, type;

	service = format = &none;
	ret = itsData[2]->getToken(currToken);
	if(!ret) return 0;
	service = currToken;
	ret = itsData[2]->getToken(currToken);
	if(!itsData[2]->cmpToken((char *)"|"))
		return 0;
	ret = itsData[2]->getToken(currToken);
	if(!itsData[2]->cmpToken((char *)"|"))
	{
		format = currToken;
		ret = itsData[2]->getToken(currToken);
		if(!itsData[2]->cmpToken((char *)"|"))
			return 0;
	}
	ret = itsData[2]->getToken(currToken);
	type = DimSERVICE;
	if(!itsData[2]->cmpToken((char *)"\n"))
	{
		if(itsData[2]->cmpToken((char *)"CMD"))
			type = DimCOMMAND;
		if(itsData[2]->cmpToken((char *)"RPC"))
			type = DimRPC;
		ret = itsData[2]->getToken(currToken);
		if(!itsData[2]->cmpToken((char *)"\n"))
			return 0;
	}
	return type;
}
	
int DimBrowser::getNextServerClient(char *&client, char *&node)
{
	int ret;

	client = node = &none;
	ret = itsData[3]->getToken(currToken);
	if(!ret) return 0;
	client = currToken;
	ret = itsData[3]->getToken(currToken);
	if(!itsData[3]->cmpToken((char *)"@"))
		return 0;
	ret = itsData[3]->getToken(currToken);
	node = currToken;
	itsData[3]->pushToken();
	ret = itsData[3]->getToken(currToken);
	if(!itsData[3]->cmpToken((char *)"|"))
		itsData[3]->popToken();
	return 1;
}

DimClient::DimClient()
{
	itsCltError = this;
}

DimClient::~DimClient() 
{
	if(dimDnsNode)
		delete[] dimDnsNode;
}

int DimClient::sendCommand(const char *name, int data)
{ 
	DimCmnd a;
	return a.send((char *)name, &data, sizeof(int));
}

int DimClient::sendCommand(const char *name, float data)
{ 
	DimCmnd a;
	return a.send((char *)name, &data, sizeof(float));
}

int DimClient::sendCommand(const char *name, double data)
{ 
	DimCmnd a;
	return a.send((char *)name, &data, sizeof(double));
}

int DimClient::sendCommand(const char *name, longlong data)
{ 
	DimCmnd a;
	return a.send((char *)name, &data, sizeof(longlong));
}

int DimClient::sendCommand(const char *name, short data)
{ 
	DimCmnd a;
	return a.send((char *)name, &data, sizeof(short));
}

int DimClient::sendCommand(const char *name, const char *data)
{ 
	DimCmnd a;
	return a.send((char *)name, (char *)data, strlen(data)+1);
}

int DimClient::sendCommand(const char *name, void *data, int datasize)
{
	DimCmnd a;
	return a.send((char *)name, data, datasize);
}

void DimClient::sendCommandNB(const char *name, int data)
{
	DimCmnd a;
	a.sendNB((char *)name, &data, sizeof(int));
}

void DimClient::sendCommandNB(const char *name, float data)
{
	DimCmnd a;
	a.sendNB((char *)name, &data, sizeof(float));
}

void DimClient::sendCommandNB(const char *name, double data)
{
	DimCmnd a;
	a.sendNB((char *)name, &data, sizeof(double));
}

void DimClient::sendCommandNB(const char *name, longlong data)
{
	DimCmnd a;
	a.sendNB((char *)name, &data, sizeof(longlong));
}

void DimClient::sendCommandNB(const char *name, short data)
{
	DimCmnd a;
	a.sendNB((char *)name, &data, sizeof(short));
}

void DimClient::sendCommandNB(const char *name, char *data)
{
	DimCmnd a;
	a.sendNB((char *)name, data, strlen(data)+1);
}

void DimClient::sendCommandNB(const char *name, void *data, int datasize)
{
	DimCmnd a;
	a.sendNB((char *)name, data, datasize);
}

int DimClient::setExitHandler(const char *srvName)
{
	DimCmnd a;
	int ret, tag = 1;
	char *name = new char[strlen(srvName) + 20];
	strcpy(name,(char *)srvName);
	strcat(name,"/SET_EXIT_HANDLER");
	ret =  a.send(name, &tag, sizeof(int));
	delete[] name;
	return ret;
}

int DimClient::killServer(const char *srvName)
{
	DimCmnd a;
	int ret, tag = 1;
	char *name = new char[strlen(srvName) + 20];
	strcpy(name,(char *)srvName);
	strcat(name,"/EXIT");
	ret = a.send(name, &tag, sizeof(int));
	delete[] name;
	return ret;
}

int DimClient::setDnsNode(const char *node)
{
	dic_set_dns_node((char *)node);
	dic_close_dns();
	return 1;
}

int DimClient::setDnsNode(const char *node, int port)
{
	dic_set_dns_port(port);
	dic_set_dns_node((char *)node);
	dic_close_dns();
	return 1;
}

char *DimClient::getDnsNode()
{
	if(!dimDnsNode)
		dimDnsNode = new char[256];
	if(dic_get_dns_node(dimDnsNode))
		return dimDnsNode;
	else
		return 0;
}

int DimClient::getDnsPort() 
{
	return dic_get_dns_port();
}

void DimClient::setNoDataCopy()
{
	dicNoCopy = 1;
}

int DimClient::getNoDataCopy()
{
	return dicNoCopy;
}

extern "C" {
//static void clt_error_user_routine(char*, int);
static void clt_error_user_routine(int severity, int code, char *msg)
{

	DimCore::inCallback = 2;
	if(DimClient::itsCltError != 0)
		DimClient::itsCltError->errorHandler(severity, code, msg);
	DimCore::inCallback = 0;
}
}

void DimClient::addErrorHandler(DimErrorHandler *handler)
{
	if(handler == 0)
	{
		dic_add_error_handler(0);
		DimClient::itsCltError = 0;
	}
	else
	{
		DimClient::itsCltError = handler;
		dic_add_error_handler(clt_error_user_routine);
	}
}

void DimClient::addErrorHandler()
{
	DimClient::itsCltError = this;
	dic_add_error_handler(clt_error_user_routine);
}

int DimClient::getServerId()
{
	if(!serverName)
		serverName = new char[128];
	serverName[0] = '\0';
	return dic_get_server(serverName);
}

int DimClient::getServerPid()
{
	int pid, ret;

	ret = dic_get_server_pid(&pid);
	if(!ret)
		return 0;
	return pid;
}

char *DimClient::getServerName()
{
	if(!serverName)
		serverName = new char[128];
	serverName[0] = '\0';
	dic_get_server(serverName);
	return(serverName);
}

/*
char *DimClient::getServerServices(int serverId)
{
	return dic_get_server_services(serverId);
}

char *DimClient::getServerServices()
{
	int id;
	if((id = dic_get_conn_id()))
		return dic_get_server_services(id);
	return (char *)0;
}
*/
char **DimClient::getServerServices()
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
	if((id = dic_get_conn_id()))
	{
		services = dic_get_server_services(id);
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

int DimClient::inCallback()
{
	if(DimCore::inCallback)
		return 1;
	return 0;
}
