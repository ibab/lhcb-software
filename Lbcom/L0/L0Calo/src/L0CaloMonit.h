// $Id: L0CaloMonit.h,v 1.2 2002-01-14 10:00:18 cattanem Exp $
#ifndef L0CALOMONIT_H 
#define L0CALOMONIT_H 1
// Include files

// from STL
#include <string>
#include <vector>

// from Gaudi 
#include "GaudiKernel/Algorithm.h"
#include "AIDA/IHistogram1D.h"

/** @class L0CaloMonit L0CaloMonit.h
 *  Monitoring algorithm for the L0 Calorimeter trigger
 *  Scans the L0CaloCandidate and fills histograms.
 *
 *  @author Olivier Callot
 *  @date   31/05/2001
 */
class L0CaloMonit: public Algorithm {
public:
  /// Standard constructor
  L0CaloMonit(const std::string& name, ISvcLocator* pSvcLocator );

  /// Standard destructor
  virtual ~L0CaloMonit( ); 

  StatusCode initialize();    ///< Algorithm initialization
  StatusCode execute   ();    ///< Algorithm execution
  StatusCode finalize  ();    ///< Algorithm finalization
  
protected:

private:
  IHistogram1D*            m_histElectron      ;
  IHistogram1D*            m_histPhoton        ;
  IHistogram1D*            m_histHadron        ;
  IHistogram1D*            m_histHadron2       ;
  IHistogram1D*            m_histPi0Local      ;
  IHistogram1D*            m_histPi0Global     ;
  IHistogram1D*            m_histSumEt         ;
};
#endif // L0CALOMONIT_H
