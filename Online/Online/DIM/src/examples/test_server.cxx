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
				 servstr("TEST/STRVAL","empty") {};
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
}

void add_serv_str(const string & s1)
{
	DimService *abc;

	abc = new DimService("TEST/STRINGVAL_CONST",(char *)s1.c_str());
}

void add_serv_bool(const bool & boolval)
{
	DimService *serv;

//	serv = new DimService("TEST/BOOLVAL_CONST",(short &)boolval);
	serv = new DimService("TEST/BOOLVAL_CONST","C:1", (void *)&boolval, 1);
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

/*
	int i, arr[15000];
	DimService *servp;
	DimCommand *cmndp;
	char str[132];
*/

	s1 = "hello";
	add_serv(ival);
	DimService servint("TEST/INTVAL",ival);
	add_serv_str(s1);
	boolval = 0;
	add_serv_bool(boolval);
	CmndServ cmdsvr;

	testServ = new ServWithHandler("MY_NEW_TEST_SERVICE_WITH_HANDLER");
	DimServer::start("TEST");

/*
//	DimServer::autoStartOff();
	DimServer::start("TEST");

	for(i = 0; i < 15000; i++)
	{
		arr[i] = i;
		sprintf(str,"ServiceManyTest/%05d",i);
		servp = new DimService(str,arr[i]);
		servp->setQuality(1);
		servp->updateService(arr[i]);
//	DimServer::start("TEST");
		sprintf(str,"CommandManyTest/%05d",i);
		cmndp = new CmndServMany(str);
//	DimServer::start("TEST");
	}
*/

	while(1)
	{
		sleep(5);
		s1 = "hello1";
		if(!boolval)
			boolval = 1;
		else
			boolval = 0;
		ival++;
		servint.updateService();
	}
	return 0;
}

