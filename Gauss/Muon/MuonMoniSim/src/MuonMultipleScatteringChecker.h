// $Id: MuonMultipleScatteringChecker.h,v 1.1 2009-04-29 14:23:21 svecchi Exp $
#ifndef MUONMULTIPLESCATTERINGCHECKER_H 
#define MUONMULTIPLESCATTERINGCHECKER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
// for Muons
#include "Event/MCHit.h"   


/** @class MuonMultipleScatteringChecker MuonMultipleScatteringChecker.h
 *  
 *
 *  @author Stefania Vecchi
 *  @date   2009-03-12
 */
class MuonMultipleScatteringChecker : public GaudiTupleAlg {
public: 
  /// Standard constructor
  MuonMultipleScatteringChecker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MuonMultipleScatteringChecker( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  bool m_fillNtuple;

protected:

private:
  std::string m_mcHeader ; ///< Location of MC Header
  std::string m_muonHits ; ///< Location of muon hits

};
#endif // MUONMULTIPLESCATTERINGCHECKER_H
