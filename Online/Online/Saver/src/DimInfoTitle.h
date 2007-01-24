#include "dic.hxx"
#include <string>


/*
  Class DimInfoHistos
  A DimInfo that retrieves the title of an object published by Gaucho
  
  Author: E. van Herwijnen, 24/01/2007
  
*/

class DimInfoTitle : public DimInfo {
  
public : 
	DimInfoTitle(std::string commentSvcname, int rtime);
	DimInfoTitle(std::string commentSvcname);
	virtual ~DimInfoTitle();
	char* getTitle();

private:
  void infoHandler();// Overloaded method infoHandler, called whenever commands arrive 
  // refresh interval
	int m_rtime;
  
  // ROOT histogram

	
  char* m_data;
	bool m_hasData;   // true after histo has been filled first time
  // private methods to fill the histograms
  void setTitleData();

};
