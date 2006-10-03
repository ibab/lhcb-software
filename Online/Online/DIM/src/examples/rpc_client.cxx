#include <dic.hxx>
#include <iostream>
using namespace std;

class Rpc : public DimRpcInfo
{
public:
	void rpcInfoHandler() {
		cout << "Callback RPC Received : " << getInt() << endl;
	}
	Rpc(char *name) :	DimRpcInfo(name, -1) {};
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
	RpcStruct(char *name) :	DimRpcInfo(name, "dead") {};
};

int main()
{
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
		cout << "Sending " << rpcValue << endl; 
		rpcCB.setData(rpcValue);
		rpcValue++;
		rpcStruct.setData(&myStruct, sizeof(myStruct));
		myStruct.int1++;
//		rpc.setData(rpcValue);
//		rpcValue = rpc.getInt();
//		cout << "RPC Received : " << rpcValue << endl;
		sleep(1);
	}
	return 0;
}
