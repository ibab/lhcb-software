// $Id: CaloDigitChecker.h,v 1.1 2007-10-30 20:24:38 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2006/02/13 14:39:27  odescham
// migration to GaudiHistoAlg completed
//
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
#ifndef CALODIGITCHECKER_H 
#define CALODIGITCHECKER_H 1
// Include files

// from STL 
#include <string>
#include <vector>
// from Gaudi 
#include "GaudiAlg/GaudiHistoAlg.h"


/** @class CaloDigitChecker CaloDigitChecker.h
 *  Monitor the results of the Calo Digitisation. Histograms
 *
 *  @author Olivier Callot
 *  @date   25/05/2001
 */

class CaloDigitChecker : public GaudiHistoAlg {
public:
  /// Standard constructor & descructor
  CaloDigitChecker( const std::string& name, ISvcLocator* pSvcLocator) ;
  virtual ~CaloDigitChecker( ); 

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
#endif // CALODIGITCHECKER_H
