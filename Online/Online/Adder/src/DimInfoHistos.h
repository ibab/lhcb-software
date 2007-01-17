#include "dic.hxx"
#include <string>

//forward declaration
class TH1;
class TH2;
class TProfile;

/*
  Class DimInfoHistos
  A DimInfo that retrieve a histogram published by Gaucho
  and fill a root histogram
  
  Author: J. Helder Lopes, 27/07/2006
  
*/

class DimInfoHistos : public DimInfo {
  
public : 
	DimInfoHistos(std::string hSvcname, int rtime);
	DimInfoHistos(std::string hSvcname);
	virtual ~DimInfoHistos();
	TH1* getHisto();
	TH2* get2DHisto();
	TProfile* getpHisto();
private:
  void infoHandler();// Overloaded method infoHandler, called whenever commands arrive 
  // refresh interval
	int m_rtime;
  
  // ROOT histogram
	std::string m_histoname;
	TH1* m_hist;
	TH2* m_hist2d;
	TProfile* m_histp;
	int m_dimension;
	
  float* m_data;
	bool m_hasData;   // true after histo has been filled first time
  // private methods to fill the histograms
  void setH1Data();
  void setH2Data();
  void setHPData();
};
