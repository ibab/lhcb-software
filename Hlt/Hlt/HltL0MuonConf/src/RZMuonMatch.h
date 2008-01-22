// $Id: RZMuonMatch.h,v 1.2 2008-01-22 09:58:06 hernando Exp $
#ifndef RZMUONMATCH_H 
#define RZMUONMATCH_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "HltBase/HltAlgorithm.h"
#include "HltBase/IMatchTVeloTracks.h"

/** @class RZMuonMatch RZMuonMatch.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2007-02-21
 */
class RZMuonMatch : public HltAlgorithm {
public: 
  /// Standard constructor
  RZMuonMatch( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RZMuonMatch( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  IMatchTVeloTracks* m_matchToolPointer;
  
  
};
#endif // RZMUONMATCH_H
