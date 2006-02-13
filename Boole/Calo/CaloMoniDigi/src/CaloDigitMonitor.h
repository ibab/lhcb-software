// $Id: CaloDigitMonitor.h,v 1.3 2006-02-13 14:39:27 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2006/01/18 18:15:21  odescham
// v2r0 : adapt to the new Event Model + cleaning
//
// Revision 1.1.1.1  2004/10/25 08:57:08  ibelyaev
// // The new package: code is inported from Calo/CaloMonitor
//
// Revision 1.2  2002/11/13 20:49:11  ibelyaev
//  small update of monitoring algorithms
//
// ============================================================================
#ifndef CALODIGITMONITOR_H 
#define CALODIGITMONITOR_H 1
// Include files

// from STL 
#include <string>
#include <vector>
// from Gaudi 
#include "GaudiAlg/GaudiHistoAlg.h"


/** @class CaloDigitMonitor CaloDigitMonitor.h
 *  Monitor the results of the Calo Digitisation. Histograms
 *
 *  @author Olivier Callot
 *  @date   25/05/2001
 */

class CaloDigitMonitor : public GaudiHistoAlg {
public:
  /// Standard constructor & descructor
  CaloDigitMonitor( const std::string& name, ISvcLocator* pSvcLocator) ;
  virtual ~CaloDigitMonitor( ); 

  StatusCode initialize();   ///< Initialisation
  StatusCode execute   ();   ///< Execution
  StatusCode finalize  ();   ///< Finalization
protected:

private:

  std::string              m_nameOfDetector      ; // Detector short name
  std::string              m_nameOfHits          ; // Hits container
  std::string              m_nameOfMCDigits      ; // MCDigit container
  std::string              m_nameOfDigits        ; // Digit container

  double                   m_maxMultiplicity     ; // histo limits
  double                   m_maxEnergy           ;
  double                   m_scaleHit            ;

};
#endif // CALODIGITMONITOR_H
