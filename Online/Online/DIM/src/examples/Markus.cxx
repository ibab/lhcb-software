#include <iostream>
#include <dic.hxx>
using namespace std;

class DnsInfo;

class FloatInfo : public virtual DimInfoHandler
{
	DimInfo *fInfo;
	float no_link, *data;
	DnsInfo *m_dns;

protected:
    friend class DnsInfo;
	void addHandler()
	{
		if(!fInfo)
			fInfo = new DimInfo("SIMPLE_SERVICE", (void *)&no_link,
				sizeof(no_link), this);
	}
	void removeHandler()
	{
		if(fInfo)
		{
			delete fInfo;
			fInfo = 0;
		}
	}
public :
	FloatInfo()
	{
		fInfo = 0; 
		no_link = -1;
	};
	void initialize();
	void infoHandler()
	{
		data = (float *)getInfo()->getData();
		cout << "Received : " << *data << endl;
	}
};

class DnsInfo : public DimInfo
{

	char *data;
	FloatInfo &fInfo;

	void infoHandler()
	{
		data = getString();
		cout << "Received : " << data << endl;
		if(data[0] == '+')
		{
			fInfo.addHandler();
		}
		else if (data[0] == '-')
		{
			fInfo.removeHandler();
		}
	}
public :
	DnsInfo(FloatInfo &f) : DimInfo("DIS_DNS/SERVER_LIST","DEAD"),
		fInfo(f) {}
    virtual ~DnsInfo()  {}
};

void FloatInfo::initialize()
{
	m_dns = new DnsInfo(*this);
}

main()
{
	FloatInfo *fInfo;

	fInfo = new FloatInfo();
	fInfo->initialize();

	while(1)
		pause();
}
