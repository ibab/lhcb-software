#include <iostream>
#include <dic.hxx>
using namespace std;

class ErrorHandler : public DimErrorHandler
{
	void errorHandler(int severity, int code, char *msg)
	{
		int index = 0;
		char **services;
		cout << severity << " " << msg << endl;
		services = DimClient::getServerServices();
		cout<< "from "<< DimClient::getServerName() << " services:" << endl;
		while(services[index])
		{
			cout << services[index] << endl;
			index++;
		}
	}
public:
	ErrorHandler() {DimClient::addErrorHandler(this);}
};

class StrService : public DimInfo
{

	void infoHandler()
	{
		int index = 0;
		char **services;
//		cout << "Dns Node = " << DimClient::getDnsNode() << endl;
		cout << "Received STRVAL : " << getString() << endl;
		services = DimClient::getServerServices();
		cout<< "from "<< DimClient::getServerName() << " services:" << endl;
		while(services[index])
		{
			cout << services[index] << endl;
			index++;
		}
	}
public :
	StrService() : DimInfo("TEST/STRVAL","not available") {};
};

int main()
{
		
	ErrorHandler errHandler;
	StrService servstr;
	char *server, *ptr, *ptr1;
	DimBrowser br;
	int type, n;

//	DimClient::addErrorHandler(errHandler);
	n = br.getServices("*");
	cout << "found " << n << " services" << endl; 
	while((type = br.getNextService(ptr, ptr1))!= 0)
	{
		cout << "type = " << type << " - " << ptr << " " << ptr1 << endl;
	}
	br.getServers();
	while(br.getNextServer(server, ptr1))
	{
		cout << server << " @ " << ptr1 << endl;
	}

	br.getServerClients("DIS_DNS");
	while(br.getNextServerClient(ptr, ptr1))
	{
		cout << ptr << " @ " << ptr1 << endl;
	}
	DimInfo servint("TEST/INTVAL",-1); 
	while(1)
	{
		sleep(10);
		cout << "Current INTVAL : " << servint.getInt() << endl;
		DimClient::sendCommand("TEST/CMND","UPDATE_STRVAL");
	}
	return 0;
}
