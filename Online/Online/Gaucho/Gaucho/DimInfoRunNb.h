#include "dic.hxx"
#include <string>


class DimInfoRunNb : public DimInfo {
  
public : 
	DimInfoRunNb(std::string RunNbSvcname);
	virtual ~DimInfoRunNb();
	int getRunNb();

private:
  void infoHandler();
  
  int m_data;
  bool m_hasData;   // true after histo has been filled first time
  // private methods to fill the histograms
  void setRunNb();

};
