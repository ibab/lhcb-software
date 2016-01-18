#include <dis.hxx>
#include <dic.hxx>
#include <iostream>
using namespace std;
#include <stdio.h>

class RpcInt : public DimRpc
{
	int val;

	void rpcHandler()
	{
		val = getInt();
		dim_print_date_time();
		printf("Got RPC %d\n", val);
		val++;
//		usleep(700000);
		dim_print_date_time();
		printf("Answering RPC %d\n", val);
		setData(val);
	}
public:
	RpcInt(const char *name): DimRpc(name,"I","I") {val = 0;};
};

typedef struct tst{
	char str1[16];
	int ints[5];
	char str2[18];
	float floats[4];
	int int1;
	float floats1[16];
} MyStruct;

typedef struct jeffIn{
	char c1;
	char c2;
	char c3;
	char str[3000];
}JeffIn;

typedef struct jeffOut{
	int i1;
	char c1;
	char str[3000];
}JeffOut;

class RpcStruct : public DimRpc
{
	MyStruct *val;

	void rpcHandler()
	{
		val = (MyStruct *)getData();
		val->int1++;
		setData(val, sizeof(MyStruct));
	}
public:
	RpcStruct(const char *name): DimRpc(name,"C:16;I:5;C:18;F:4;I:1;F:16",
		"C:16;I:5;C:18;F:4;I:1;F:16") {val = 0;};
};

class JeffRpcStruct : public DimRpc
{
	JeffIn *pin;
	JeffOut pout;
	int counter;

	void rpcHandler()
	{
		pin = (JeffIn *)getData();
		pout.i1 = counter++;
		pout.c1 = pin->c1;
		strcpy(pout.str,pin->str);
		setData(&pout, (int)strlen(pout.str)+1+5);
	}
public:
	JeffRpcStruct(const char *name): DimRpc(name,"C:1;C:1;C:1;C",
		"I:1;C:1;C") {counter = 0;};
};

int main()
{
	RpcInt myRpcInt("TESTRPC/INT");
	RpcInt *myRpcP;
	RpcStruct myRpcStruct("TESTRPC/STRUCT");
	JeffRpcStruct jeffRpcStruct("TESTJEFF");
	int i;
	char name[64];

	for(i = 0; i < 10; i++)
	{
		sprintf(name,"TESTRPC%d/INT",i);
		myRpcP = new RpcInt(name);
	}
	DimServer::start("TESTRPC");
	while(1)
		pause();
	return 0;
}
