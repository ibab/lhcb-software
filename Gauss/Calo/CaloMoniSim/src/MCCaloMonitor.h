// $Id: MCCaloMonitor.h,v 1.1.1.1 2004-10-25 08:53:25 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef MCCALOMONITOR_H
#define MCCALOMONITOR_H 1

// Include files
// from STL
#include <cmath>
#include <string>
#include <vector>
// from Gaudi
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/INTuple.h"
// from Calo
#include "CaloDet/DeCalorimeter.h"
#include "AIDA/IAnalysisFactory.h"
#include "AIDA/IHistogramFactory.h"

/** @class MCCaloMonitor MCCaloMonitor.h 
 *
 * This class declares all the parameters from MCCaloMonitor.cpp, that 
 * obtain MCCaloDigit hits and monitorize them in histograms 
 *
 *  @author Carlos Gonzalez
 *  @date   24-11-2003
 */

// histograms
class IHistogram1D  ;  
class IHistogram2D  ;  
class IHistogramSvc ; 

class MCCaloMonitor : public Algorithm {
public:
  /// Standard constructor
  MCCaloMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MCCaloMonitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

    
private:
  // histogramas:
  
  std::string m_nameOfMCHits           ;
  std::string m_hName1;
  std::string m_hName1a;
  std::string m_hName1b;
  std::string m_hName1c;
  std::string m_hName2;
  std::string m_hName2a;
  std::string m_hName2b;
  std::string m_hName2c;
  std::string m_hName3;
  std::string m_hName3a;
  std::string m_hName3b;
  std::string m_hName3c;
  std::string m_hName4;
  std::string m_hName40;
  std::string m_hName41;
  std::string m_hName42;
  std::string m_hName43;
  std::string m_hName44;
  std::string m_hName45;

  std::string m_hNameNorm1;
  std::string m_hNameNorm2;
  
  




  std::string m_GeometryRoot ;
  bool m_DivMonitor;
  double m_MaxE;
  double m_MinE;
  double m_Threshold;
  int m_MaxT;
  double MaxT;
  int m_MinT;
  double MinT;
  std::string m_hDir;
  int m_nEvents;
  std::string m_Detector;
  std::string m_DetectorName;
  
  const std::string m_textNorm;

  int m_Bin;
  
  /// histogram service
  
  IHistogramFactory*      hf                  ;
  
  IHistogramSvc*  m_histoSvc                  ;
  IHistogram1D*   m_hSHitEnergy           ;
  IHistogram1D*   m_hSHitEnergyInn        ;
  IHistogram1D*   m_hSHitEnergyOut        ;
  IHistogram1D*   m_hSHitEnergyMid        ;
  IHistogram1D*   m_hEnergyEnergy         ;
  IHistogram1D*   m_hEnergyEnergyInn      ;
  IHistogram1D*   m_hEnergyEnergyOut      ;
  IHistogram1D*   m_hEnergyEnergyMid      ;  
  
  
  IHistogram1D*   m_hDistribution          ;
  IHistogram1D*   m_hDistributionBC        ;
  IHistogram1D*   m_hDistributionBC0       ;
  IHistogram1D*   m_hDistributionBC1       ;
  IHistogram1D*   m_hDistributionBC2       ;
  IHistogram1D*   m_hDistributionBC3       ;
  IHistogram1D*   m_hDistributionBC4       ;

  IHistogram1D*   m_hEnergyTime           ;
  IHistogram1D*   m_hEnergyTimeInn        ;
  IHistogram1D*   m_hEnergyTimeMid        ;
  IHistogram1D*   m_hEnergyTimeOut        ;

protected:
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // MCCALOMONITOR_H
// ============================================================================


