#include "dic.hxx"
#include <string>

//forward declaration
class TH1;
class DimTimerHistos;

/*
  Class DimInfoHistos
  A DimInfo that retrieve a histogram published by Gaucho
  and fill a root histogram
  
  Author: J. Helder Lopes, 16/12/2006
  J. Helder Lopes,  2006/12/26: Added 1D profile viewing
  
*/

class DimInfoHistos : public DimInfo {
  
public : 
	DimInfoHistos(std::string hSvcname, int rtime);
	DimInfoHistos(std::string hSvcname);
	virtual ~DimInfoHistos();
	TH1* getHisto();
private:
  void infoHandler();// Overloaded method infoHandler, called whenever commands arrive 
  // refresh interval
	int m_rtime;
  
  // ROOT histogram
	std::string m_histoname;
        std::string m_histType;
	TH1* m_hist;
	int m_dimension;
	
  float* m_data;
	bool m_hasData;   // true after histo has been filled first time
  // private methods to fill the histograms
  void setH1Data();
  void setH2Data();
  void set1DProfData();

};
