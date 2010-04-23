#include "dic.hxx"
#include <string>


class DimInfoTCK : public DimInfo {
  
public : 
	DimInfoTCK(std::string TCKSvcname);
	virtual ~DimInfoTCK();
	unsigned int getTCK();

private:
  void infoHandler();
  unsigned int m_TCK;
  int m_data;
  bool m_hasData;   // true after histo has been filled first time
  // private methods to fill the histograms
  void setTCK();
};
