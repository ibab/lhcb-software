#include "dic.hxx"
#include <string>


/*
  Class DimInfoHistos
  A DimInfo that retrieve a histogram published by Gaucho
  and fill a root histogram
  
  Author: J. Helder Lopes, 27/07/2006
  
*/

class DimInfoTitle : public DimInfo {
  
public : 
	DimInfoTitle(std::string commentSvcname);
	virtual ~DimInfoTitle();
	char* getTitle();

private:
  void infoHandler();// Overloaded method infoHandler, called whenever commands arrive 
  
  // ROOT histogram

	
  char* m_data;
	bool m_hasData;   // true after histo has been filled first time
  // private methods to fill the histograms
  void setTitleData();

};
