// $Id: RZMuonMatch.h,v 1.1.1.1 2007-05-22 10:20:32 asatta Exp $
#ifndef RZMUONMATCH_H 
#define RZMUONMATCH_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltFunctions.h"

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
  StatusCode tryMatch(LHCb::Track* track);
protected:

private:
  float m_x_cut;
  float m_sec_tol;
  float z_magnet_center;
  
};
#endif // RZMUONMATCH_H
