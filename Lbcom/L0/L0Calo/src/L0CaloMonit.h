// $Id: L0CaloMonit.h,v 1.3 2005-01-12 09:19:38 ocallot Exp $
#ifndef L0CALOMONIT_H 
#define L0CALOMONIT_H 1
// Include files

// from Gaudi 
#include "GaudiAlg/GaudiHistoAlg.h"
#include "AIDA/IHistogram1D.h"

/** @class L0CaloMonit L0CaloMonit.h
 *  Monitoring algorithm for the L0 Calorimeter trigger
 *  Scans the L0CaloCandidate and fills histograms.
 *
 *  @author Olivier Callot
 *  @date   31/05/2001
 */
class L0CaloMonit: public GaudiHistoAlg {
public:
  /// Standard constructor
  L0CaloMonit(const std::string& name, ISvcLocator* pSvcLocator );

  /// Standard destructor
  virtual ~L0CaloMonit( ); 

  StatusCode initialize();    ///< Algorithm initialization
  StatusCode execute   ();    ///< Algorithm execution
  
protected:

private:
  IHistogram1D*            m_histElectron      ;
  IHistogram1D*            m_histPhoton        ;
  IHistogram1D*            m_histHadron        ;
  IHistogram1D*            m_histPi0Local      ;
  IHistogram1D*            m_histPi0Global     ;
  IHistogram1D*            m_histSumEt         ;
  IHistogram1D*            m_histSpdMult       ;
};
#endif // L0CALOMONIT_H
