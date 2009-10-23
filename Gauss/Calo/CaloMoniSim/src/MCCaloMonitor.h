// $Id: MCCaloMonitor.h,v 1.5 2009-10-23 10:45:22 jonrob Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2009/03/26 21:55:03  robbep
// changes for spill over generation
//
// Revision 1.3  2006/02/13 12:03:00  odescham
// v2r1 migration to GaudiHistoAlg completed - structure pathes adapted to new enveloppes
//
// Revision 1.2  2005/12/16 17:53:59  odescham
// v2r0 - LHCb v20 migration
//
// Revision 1.1.1.1  2004/10/25 08:53:25  ibelyaev
// // New Package: code imported from Calo/CaloMonitor
//
// ============================================================================
#ifndef MCCALOMONITOR_H
#define MCCALOMONITOR_H 1

// Include files
// from STL
#include <cmath>
#include <string>
#include <vector>
// from Calo
#include "CaloDet/DeCalorimeter.h"

/** @class MCCaloMonitor MCCaloMonitor.h 
 *
 * This class declares all the parameters from MCCaloMonitor.cpp, that 
 * obtain MCCaloDigit hits and monitorize them in histograms 
 *
 *  @author Carlos Gonzalez
 *  @date   24-11-2003
 */

// histograms

class MCCaloMonitor : public GaudiHistoAlg {
public:
  /// Standard constructor
  MCCaloMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MCCaloMonitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

    
private:
  // histograms:
  
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
  std::string m_mcParticles ;
  
  const std::string m_textNorm;

  int m_Bin;
  
  DeCalorimeter * m_detector;

protected:
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // MCCALOMONITOR_H
// ============================================================================


