#include <dic.hxx>

class Service : public DimInfo
{
  int n_bad;
  int n_good;

	void infoHandler()
	{
	  char *ptr;
	  ptr = (char *)getData();
	  //	  cout << getName() << " received " << ptr << endl;
	  if(ptr[0] == '-')
	    {
	      n_bad = 1;
	      n_good = 0;
	    }
	  if(ptr[0] == 'h')
	    {
	      n_good = 1;
	      n_bad = 0;
	    }
	}
public :
	Service(char *name) : DimInfo(name,"--") 
		{n_bad = 0; n_good = 0;}
	int getNgood() {return n_good;}
	int getNbad() {return n_bad;}
};


int main(int argc, char *argv[])
{
	int i, n, msgSize, nServices = 0;
	Service **services;
	float mps,tpm;
	DimBrowser br;
	char name[132], *format;

	sscanf(argv[1],"%d",&nServices);
	services = new Service*[nServices];
	for(i = 0; i < nServices; i++)
	{
	  services[i] = 0;
	}
	for(i = 0; i < nServices; i++)
	{
	  sprintf(name,"BENCH_SERVICE_%03d",i);
	  services[i] = new Service(name);
	}
	while(1)
	  {
	    sleep(10);
	    n = 0;
	    for(i = 0; i < nServices; i++)
	      {
		if(services[i])
		  n += services[i]->getNgood();
	      }
	    cout << "N Good = "<< n << endl;
	    n = 0;
	    for(i = 0; i < nServices; i++)
	      {
		if(services[i])
		  n += services[i]->getNbad();
	      }
	    cout << "N bad = "<< n << endl;
	}
}
