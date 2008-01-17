#ifndef Adder_Adder_H
#define Adder_Adder_H 1

// Include files
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/Property.h"
#include "DimInfoHistos.h"
#include "dis.hxx"
#include "Tim.h"


/** @class Adder Adder.h Adder/Adder.h
 * 
 * This program adds histograms published by tasks using
 * the Gaucho MonitorSvc and publishes them. For user
 * documentation see the release notes.
 *
 *   @author Eric van Herwijnen
 *   @author Monica Pepe-Altarelli
 *   @date 09/01/2008
 */

// Forward declarations
class Tim;

class Adder : public Algorithm {
public:
  /// Constructor of this form must be provided
  Adder(const std::string& name, ISvcLocator* pSvcLocator); 

  /// Three mandatory member functions of any algorithm
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  StatusCode findDimServices();
private:  
  std::vector<std::string> m_nodeNames; 
  std::vector<std::string> m_svcFoundName;
  std::vector<std::string> m_histogramName; 
  std::vector<std::string> m_histogramUniqueName; 
  std::vector<std::string> m_histogramFoundName; 
  std::vector<std::string> m_algorithmName; 
  std::vector<std::string> m_algorithmFoundName; 
  std::vector<std::string> m_taskName; 
  std::vector<std::string> m_taskFoundName; 

  int m_refreshTime;
  std::string m_dimClientDns;
  std::string m_serverName;    
  int m_iCount;
  int m_iCount2d;
  int m_iCountp;
  //we only call the dimbrowser once, so store found service names in these arrays

  std::vector<std::vector <std::string> > m_hSvcName;
  std::vector<std::vector <std::string> >  m_hSvcName2d;
  std::vector<std::vector <std::string> > m_pSvcName;
  
  //the number of histograms found
  std::vector<int> m_nbOf1DHistos;
  std::vector<int> m_nbOf2DHistos;
  std::vector<int> m_nbOfPHistos; 
  int m_histosFound;
  
  //arrays with Dim buffers of each histogram service found
  std::vector<std::vector<DimInfoHistos*> > m_hInfo;
  std::vector<std::vector<DimInfoHistos*> > m_hInfo2D;
  std::vector<std::vector<DimInfoHistos*> > m_pInfo;
  
  DimBrowser dbr;  
  Tim* tim;
  std::vector<std::vector<DimInfoHistos*> > m_infoHistos;
  
  protected:
  /// Variable to contain the process name
  std::string   m_procName;
  
};



#endif    // Adder_Adder_H
