#ifndef Adder_Adder_H
#define Adder_Adder_H 1

// Include files
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/Property.h"
#include "DimInfoHistos.h"

#include<time.h>

/** @class Adder


*/

// Forward declarations

class Adder : public Algorithm {
public:
  /// Constructor of this form must be provided
  Adder(const std::string& name, ISvcLocator* pSvcLocator); 

  /// Three mandatory member functions of any algorithm
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
private:  
  std::string m_nodename; 
  std::vector<std::string> m_histogramname; 
  std::vector<std::string> m_algorithmname; 
  std::vector<std::string> m_taskname; 
  int m_refreshtime;
  std::string m_dimclientdns;   
  int counter;
  int icount;
  int icount2d;
  int icountp;
  int svcsize;
  //we only call the dimbrowser once, so store found service names in these arrays
  std::vector<std::vector <std::string> > hSvcname;
  std::vector<std::vector <std::string> >  hSvcname2d;
  std::vector<std::vector <std::string> > pSvcname;
  std::vector<std::vector <std::string> > commentSvcname;
  
  //the number of histograms found
  std::vector<int> nbof1dhistos;
  std::vector<int> nbof2dhistos;
  std::vector<int> nbofphistos; 
  
  //arrays with Dim buffers of each histogram service found
  std::vector<std::vector<DimInfoHistos*> > hinfo;
  std::vector<std::vector<DimInfoHistos*> > hinfo2d;
  std::vector<std::vector<DimInfoHistos*> > pinfo;
};

#endif    // Adder_Adder_H
