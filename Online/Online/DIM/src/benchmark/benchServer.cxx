#include <iostream>
#include <dis.hxx>
#ifdef WIN32
#include <process.h>
#endif
#include <stdio.h>

int main(int argc, char *argv[])
{
	int i, msgSize, nServices, pid;
	char *msg, servName[64];
	DimService **services;

	if(argc){}
	sscanf(argv[1],"%d",&msgSize);
	sscanf(argv[2],"%d",&nServices);
	msg = new char[msgSize];
	services = new DimService*[nServices];
	
	pid = getpid();
	for(i = 0; i < nServices; i++)
	{
	  sprintf(servName,"BENCH_SERVICE_%d_%03d",pid, i);
	  services[i] = new DimService(servName, "C", msg, msgSize);
	}
	sprintf(servName,"BENCH_%d",pid);
	DimServer::start(servName);
	while(1)
	{
	  for(i = 0; i < nServices; i++)
	  {
	    services[i]->updateService();
	  }
	}
	return 0;
}
