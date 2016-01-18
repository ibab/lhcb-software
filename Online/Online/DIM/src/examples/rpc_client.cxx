#include <dic.hxx>
#include <iostream>
using namespace std;
#include <stdio.h>

class Rpc : public DimRpcInfo
{
public:
	void rpcInfoHandler() {
		dim_print_date_time();
		cout << "Callback RPC Received : " << getInt() << endl;
	}
	Rpc(const char *name) :	DimRpcInfo(name, 1, -1) {};
};

typedef struct tst{
	char str1[16];
	int ints[5];
	char str2[18];
	float floats[4];
	int int1;
	float floats1[16];
} MyStruct;

class RpcStruct : public DimRpcInfo
{
public:
	void rpcInfoHandler() {
		MyStruct *val;
		val = (MyStruct *)getData();
		cout << "Callback RPC Received : " << endl;
		cout << val->str1 << " " << val->str2 << " " << val->int1 << endl;
	}
	RpcStruct(const char *name) :	DimRpcInfo(name, (char *)"dead") {};
};

void do_work(void *tag)
{
	DimRpcInfo *myRpc;
//	Rpc *myRpc;
	char name[64];
	int out, in;
	
	sprintf(name,"TESTRPC%ld/INT",(dim_long)tag);
	myRpc = new DimRpcInfo(name, 10, -1);
//	myRpc = new Rpc(name);

	out = 1;
	while(1)
	{
		sleep(5);
//		cout << "RPC Sent : " << out << endl;
		myRpc->setData(out);
		in = myRpc->getInt();
dim_lock();
dim_print_date_time();
cout << "Instance "<<(dim_long)tag<<" sent "<<out<< " got "<<in <<endl;
dim_unlock();
		out++;
	}
}

void do_workCB()
{
//	DimRpcInfo *myRpc;
	Rpc *myRpc;
	char name[64];
	int out;
	
	sprintf(name,"TESTRPC/INT");
	myRpc = new Rpc(name);
//	myRpc = new Rpc(name);

	out = 1;
	while(1)
	{
		dim_print_date_time();
		cout << "RPC Sent : " << out << endl;
		myRpc->setData(out);
		out++;
		sleep(5);
	}
}

int main()
{
	int i;

	dim_init();
//	DimClient::setNoDataCopy();

	for(i = 0; i < 1; i++)
	{
		dim_start_thread(do_work,(void *)i);
	}
//	do_workCB();
	while(1)
		pause();
	/*
	int rpcValue = 0;
//	DimRpcInfo rpc("TESTRPC/INT",-1);
	Rpc rpcCB("TESTRPC/INT");
	RpcStruct rpcStruct("TESTRPC/STRUCT");
	MyStruct myStruct;

	strcpy(myStruct.str1,"hello");
	strcpy(myStruct.str2,"world");
	myStruct.int1 = 1;
	while(1)
	{
		dim_print_date_time();
		cout << "Sending " << rpcValue << endl; 
		rpcCB.setData(rpcValue);
		rpcValue++;
//		sleep(5);
		rpcStruct.setData(&myStruct, sizeof(myStruct));
		myStruct.int1++;
//		rpc.setData(rpcValue);
//		rpcValue = rpc.getInt();
//		cout << "RPC Received : " << rpcValue << endl;
		sleep(10);
	}
	return 0;
	*/
}
