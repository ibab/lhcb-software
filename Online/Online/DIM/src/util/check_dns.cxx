#include <iostream>
#include <dic.hxx>
using namespace std;

int main()
{
	int version = 0;
	DimCurrentInfo dns("DIS_DNS/VERSION_NUMBER",10,-1);
	
	version = dns.getInt();
	if(version == -1)
		cout << "DNS not running" << endl;
	else
		cout << "DNS running" << endl;
	return(0);
}
