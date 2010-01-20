#include "dic.hxx"
#include <string>


class DimInfoRunNb : public DimInfo {
  
public : 
	DimInfoRunNb(std::string RunNbSvcname,std::string slice);
	virtual ~DimInfoRunNb();
	int getRunNb();

private:
  void infoHandler();
  int m_runNumber;
  int m_data;
  bool m_hasData;   // true after histo has been filled first time
  // private methods to fill the histograms
  void setRunNb();
  std::string m_slice;
};
