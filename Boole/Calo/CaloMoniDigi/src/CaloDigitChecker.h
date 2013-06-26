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
