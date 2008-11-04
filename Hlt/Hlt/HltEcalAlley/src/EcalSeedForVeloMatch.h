// $Id: EcalSeedForVeloMatch.h,v 1.1 2008-11-04 08:30:28 witekma Exp $
#ifndef ECALSEEDFORVELOMATCH_H 
#define ECALSEEDFORVELOMATCH_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "HltBase/ICaloSeedTool.h"            // Interface

// forward declaration
namespace LHCb {
  class CaloCellID;
}

/** @class EcalSeedForVeloMatch EcalSeedForVeloMatch.h
 *  
 *  Tool to prepare L0HadronCand to a track using information
 *  in the ECal if possible
 *
 *  @author Johannes Albrecht
 *  @date   2007-07-17
 *
 *  2007-05-31 Johannes Albrecht: Hadron Seed preparation completely rewritten and optimized, 
 *                                uses 2x2 HCal cluster and 2x2 ECal cluster   
 *
 *  2007-01-15 Johannes Albrecht some adaptions
 *
 *  2006 Herve Terrier

*/
class EcalSeedForVeloMatch : public GaudiTool, virtual public ICaloSeedTool {
public: 
  /// Standard constructor
  EcalSeedForVeloMatch( const std::string& type, 
                  const std::string& name,
                  const IInterface* parent);

  virtual ~EcalSeedForVeloMatch( ); ///< Destructor

  /// Initialize method
  virtual StatusCode initialize();

  virtual StatusCode makeTrack( const LHCb::L0CaloCandidate& hadL0Cand,
                                LHCb::Track& seedTrack );
  

protected:

private:

  double m_eres[2]; /// de/e = sqrt( m_eres[0]^2 + m_eres[1]^2 / e )  
  double m_eCorrect;  /// simple energy correction factor

};
#endif // ECALSEEDFORVELOMATCH_H
