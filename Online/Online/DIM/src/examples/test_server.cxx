#include <iostream>
#include <dis.hxx>
#ifndef WIN32
#include <unistd.h>
#endif
using namespace std;
#include <string>

class ErrorHandler : public DimErrorHandler
{
	void errorHandler(int severity, int code, char *msg)
	{
		int index = 0;
		char **services;
		if(code){}
		cout << severity << " " << msg << endl;
		services = DimServer::getClientServices();
		cout<< "from "<< DimServer::getClientName() << " services:" << endl;
		while(services[index])
		{
			cout << services[index] << endl;
			index++;
		}
	}
public:
	ErrorHandler() {DimServer::addErrorHandler(this);}
};

class ExitHandler : public DimExitHandler
{
	void exitHandler(int code)
	{
		cout << "exit code " << code << endl;
	}
public:
	ExitHandler() {DimServer::addExitHandler(this);}
};

class CmndServ : public DimCommand, public DimTimer
{
	DimService servstr;
	void commandHandler()
	{
		int index = 0;
		char **services;
		cout << "Command " << getString() << " received" << endl;
		servstr.updateService(getString()); 
		services = DimServer::getClientServices();
		cout<< "from "<< DimServer::getClientName() << " services:" << endl;
		while(services[index])
		{
			cout << services[index] << endl;
			index++;
		}
	}

public :
	CmndServ() : DimCommand("TEST/CMND","C"), 
				 servstr("TEST/STRVAL",(char *)"empty") {};
/*
	void handleIt()
	{
		int index = 0;
		char **services;
		dim_print_date_time();
		cout << "Command " << getString() << " received" << endl;
		cout << "time: "<<getTimestamp()<<" millies: "<<getTimestampMillisecs()<<endl;
		servstr.updateService(getString()); 
	}
*/
};

/*
class CmndServMany : public DimCommand
{
	void commandHandler()
	{
		cout << "Command " << getString() << " received" << endl;
	}
public :
	CmndServMany(char *name) : DimCommand(name,"C") {};
};
*/

void add_serv(const int & ival)
{
	DimService *abc;

	abc = new DimService("TEST/INTVAL_CONST",(int &)ival);
	if(abc){}
}

void add_serv_str(const string & s1)
{
	DimService *abc;

	abc = new DimService("TEST/STRINGVAL_CONST",(char *)s1.c_str());
	if(abc){}
}

DimService *bool_serv[10];
void add_serv_bool(const bool & boolval)
{

//	serv = new DimService("TEST/BOOLVAL_CONST",(short &)boolval);
	bool_serv[0] = new DimService("TEST/BOOLVAL_CONST","C:1", (void *)&boolval, 1);
	bool_serv[1] = new DimService("TEST/BOOLVAL_CONST1","C:1", (void *)&boolval, 1);
}

class ServWithHandler : public DimService
{
	int value;

	void serviceHandler()
	{
		value++;
//		setData(value);
	}
public :
	ServWithHandler(char *name) : DimService(name, value) { value = 0;};
};

int main()
{
	int ival = 0;
//	ErrorHandler errHandler;
//	ExitHandler exHandler;
//	DimServer::setDnsNode("axdes2.cern.ch");
	string s1;
	bool boolval;
	ServWithHandler *testServ;
	DimServerDns *newDns;
	char *extraDns = 0;
	DimService *new_servint;

//	DimService *dim = new DimService("test","C");
//	delete dim;

	DimServer::start("TEST");
	extraDns = DimUtil::getEnvVar((char *)"EXTRA_DNS_NODE");
	if(extraDns)
		newDns = new DimServerDns(extraDns, 0, (char *)"new_TEST");

//	int i, arr[15000];
//	DimService *servp;
//	DimCommand *cmndp;
//	char str[132];

//	float farr[4];
//	DimService *farrp;

	s1 = "hello";
	add_serv(ival);
	DimService servint("TEST/INTVAL",ival);

	if(extraDns)
	{
		new_servint = new DimService(newDns, "new_TEST/INTVAL",ival);
	}

	add_serv_str(s1);
	boolval = 0;
	add_serv_bool(boolval);
	CmndServ cmdsvr;

	testServ = new ServWithHandler((char *)"MY_NEW_TEST_SERVICE_WITH_HANDLER");
	if(testServ){}

	//	farr[0] = 1.2;
//	farr[1] = 2.3;
//	farrp = new DimService("/PCITCO147/sensors/fan/input","F", farr, sizeof(farr));


//	DimServer::autoStartOff();
//	DimServer::start("TEST");

//	for(i = 0; i < 15000; i++)
//	{
//		arr[i] = i;
//		sprintf(str,"ServiceManyTest/%05d",i);
//		servp = new DimService(str,arr[i]);
//		servp->setQuality(1);
//		servp->updateService(arr[i]);
////	DimServer::start("TEST");
//		sprintf(str,"CommandManyTest/%05d",i);
//		cmndp = new CmndServMany(str);
////	DimServer::start("TEST");
//	}

	while(1)
	{
		sleep(5);

//		while(cmdsvr.hasNext())
//		{
//			cmdsvr.getNext();
//			cmdsvr.handleIt();
//		}

		s1 = "hello1";
		if(!boolval)
			boolval = 1;
		else
			boolval = 0;
		ival++;
		bool_serv[1]->updateService();
		
//		int inCallback = DimServer::inCallback();
//		cout << "main: In callback "<< inCallback << endl; 
		servint.updateService();
		if(extraDns)
			new_servint->updateService();
	}
	return 0;
}

/*
int main()
{
DimService *servint, *newServint;
int ival = 0;

DimServer::start("TEST");
servint = new DimService("TEST/INTVAL",ival);
sleep(20);
DimServer::stop();
sleep(1);
DimServer::start("TESTOTHER");
newServint = new DimService("TESTOTHER/INTVAL",ival);
sleep(20);
DimServer::stop();
}
*/
