#ifndef DBROOTHIST_H
#define DBROOTHIST_H 1
#include "OnlineHistDB/OnlineRootHist.h"
#include "dic.hxx"
class TPad;


class DbRootHist : public OnlineRootHist, public DimInfo
{
 public:
  DbRootHist(std::string identifier,
	     std::string dimServiceName,
       int refreshTime,
	     int localInstance,
       OnlineHistDB* histogramDB);
     
  DbRootHist (const DbRootHist & ); // copy constructor
  DbRootHist & operator= (const DbRootHist &); // Assignment operator
     
  virtual ~DbRootHist();
  
// Histos live in pads, so each histo must have a home

  // Binning is only known from DIM, so create ROOT histo with DIM
  void initFromDim();
  
  // Once we have our ROOT histos, this will be called to get new values
  // TODO: if binning changes on the fly, reinit ...MonObj?
  void fillFromDim();
  
  // Flag enabling DIM infoHandler() callback body for filling histos
  void enableRefresh() { m_toRefresh = true; }
  void disableRefresh() { m_toRefresh = false; }
  
  // Clearing histos with an offset value and restoring them
  void enableClear();
  void disableClear();
  bool isCleared() { return m_cleared; }
  
  TString* histoRootName() {return m_histoRootName;}
  std::string hstype() { return m_hstype; }
  int instance() { return m_localInstance; }

  TH1*    rootHistogram;        // actual ROOT histo
  TPad*   hostingPad;           // Hosting pad for page layout  

private:
// TODO: have a stack of offset for bracketing
  TH1*      m_offsetHistogram;  // state of histo @ clr/rst
  
  int       m_refreshTime;      // DIM regular time interval  
  float*    m_histoDimData;     // dimbuffer
  void      infoHandler();      // overloaded from DimInfo
  TString*  m_histoRootName;    // generated ROOT histo name
  bool      m_toRefresh;        // flag for refresh/static histo
  bool      m_cleared;          // flag for clear/integrate
  std::string m_hstype;
  std::string m_hname;
  int m_localInstance;
};

#endif // DBROOTHIST_H
