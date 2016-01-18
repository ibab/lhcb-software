#include <dic.hxx>
#include <iostream>
#include <stdio.h>
using namespace std;

typedef struct{
	int bitset;
	char boolval;
	int intval;
	float floatval;
	char stringval[128];
}COMPLEXDATA;

class SimpleService : public DimInfo
{
	void infoHandler()
	{
		cout << "SimpleService : " << getFloat() << "\n" << endl;
	}
public :
	SimpleService(const char *name) : DimInfo(name, -1.0) {};
};

double no_link_darray[8] = {-1.0,0,0,0,0,0,0,0};

class DimDoubleArray : public DimInfo
{
	void infoHandler()
	{
		double *data;
		int i, len;

		data = (double *)getData();
		len = getSize()/sizeof(double);
		for(i = 0; i < len; i++)
		{
			cout << data[i] << endl;
		}
	}
public :
	DimDoubleArray(const char *name) : DimInfo(name, no_link_darray, 8*sizeof(double)) {};
};


class ComplexService : public DimInfo
{
	void infoHandler()
	{
		COMPLEXDATA *data;
		int i;
		unsigned mask = 0x80000000;

		data = (COMPLEXDATA *)getData();

		cout << "ComplexService : \n";
		cout << "\tbitset : ";
		for(i = 0; i < 32; i++)
		{
			if (data->bitset & mask)
				cout << "1";
			else 
				cout << "0";
			mask >>= 1;
		}
		cout << "\n";
		cout << "\tboolval : ";
		if(data->boolval)
				cout << "TRUE\n";
			else 
				cout << "FALSE\n";
		cout << "\tintval : "<< data->intval << "\n";
		cout << "\tfloatval : " << data->floatval << "\n";
		cout << "\tstringval : "<< data->stringval << "\n\n";
		cout << "Sending PVSS_SIMPLE_COMMAND : " << data->stringval << "\n" << endl;
		DimClient::sendCommand("PVSS_SIMPLE_COMMAND",data->stringval);
	}
public :
	ComplexService(const char *name) : DimInfo(name, -1.0) {};
};

class RpcService : public DimRpcInfo
{
public:
	void rpcInfoHandler() {
		int value;
		value = getInt();
		dim_print_date_time();
		cout << "RPC Service received: " << value << "\n" << endl;
	}
	RpcService(const char *name, int timeout) :	DimRpcInfo(name, timeout, -1) {};
};


int main()
{
	int value = 0;
	SimpleService simple("PVSS_SIMPLE_SERVICE");
	ComplexService complex("PVSS_COMPLEX_SERVICE");
	RpcService rpc("TESTRPC/INT", 25);
	DimDoubleArray dda("TEST_SRVC");

	while(1)
	{
		dim_print_date_time();
		value++;
		cout << "RPC Service sent: " << value << "\n" << endl;
		rpc.setData(value);
		value++;
		sleep(30);
	}
	return 0;
}
