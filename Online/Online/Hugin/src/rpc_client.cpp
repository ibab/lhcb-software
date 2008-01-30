#include <dic.hxx>
#include <iostream>
using namespace std;

class Rpc : public DimRpcInfo
{
public:
  Rpc(char *nam) : DimRpcInfo(nam,-1)
  {};
public:
};


int main()
{
  char rpcValue[100];
	char *hug;
  int *ports;
  int dports[21];
  char *tells[20];
	Rpc *rpcCB;
  rpcCB = new Rpc("ConfDBHuginTells");
  DimInfo *prts=new DimInfo("ConfDBHuginPorts",dports,sizeof(dports));
  hug = "tthugin04|ROBoard";
	while(1)
	{
		cout << "Sending " << rpcValue << endl; 
		rpcCB->setData(hug);
    char *ret=(char *)rpcCB->getData();
    ports = (int*)prts->getData();
    int siz =rpcCB->getSize();
    int ntel = 0;
    int tellpos=0;
    while (1)
    {
      if (ret[tellpos]!=0)
      {
        tells[ntel] = ret+tellpos;
        ntel++;
        tellpos+= strlen(ret+tellpos)+1;
      }
      else
      {
        break;
      }
    }
    printf("for Hugin %s Mask:%0x:\n",hug,ports[20]);
    for (int i=0;i<ntel;i++)
    {
      printf ("%s on port %i\n",tells[i],ports[i]);
    }
    sleep(20);
	}
	return 0;
}

