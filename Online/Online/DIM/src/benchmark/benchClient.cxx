#include <iostream>
using namespace std;
#include <dic.hxx>

#define TEST_TIME 10

char ServerName[128];
int MsgSize;
int Done = 0;
int NReceived;

class Service : public DimInfo
{
	void infoHandler()
	{
	  if(!Done)
	  {
		MsgSize = getSize();
		strcpy(ServerName, DimClient::getServerName());
		Done = 1;
	  }
	  NReceived++;
	}
public :
	Service(char *name) : DimInfo(name,(char *)"--") {/*nReceived = 0;*/}
};

int main(int argc, char *argv[])
{
	int i, nServices = 0;
	Service **services;
	float mps,tpm;
	DimBrowser br;
	char *name, *format, *cltptr, *srvptr, clientName[128];

	if(argc){}
	if(argv){}
	br.getServices("BENCH_SERVICE_*");

	while(br.getNextService(name, format)!= 0)
	{
		nServices++;
	}
	services = new Service*[nServices];
	i = 0;
	while(br.getNextService(name, format)!= 0)
	{
	  services[i++] = new Service(name);
	}
	dic_get_id(clientName);
	if((cltptr = strchr(clientName,'@')))
		cltptr++;
	sleep(5);
	NReceived = 0;

	sleep(TEST_TIME);

	mps = NReceived/TEST_TIME;
	if((srvptr = strchr(ServerName,'@')))
		srvptr++;
	cout << "Benchmark from "<< srvptr << " to " << cltptr << " :" << endl;
	cout << "Server publishes " << nServices << " services of " << MsgSize << " bytes each"<< endl;
	cout << "Result :" << endl;
	cout << "\tMessages/s = " << mps << endl;
	tpm = 1/(float)mps*1000;
	cout << "\tTime(ms)/message = " << tpm << endl;
	cout << "\tThroughput (Kb/s) = " << mps*MsgSize/1024 << endl;
	return 1;
}
