// $Id: L0MuonAndTConf.h,v 1.2 2007-07-12 17:45:06 asatta Exp $
#ifndef L0MUONANDTCONF_H 
#define L0MUONANDTCONF_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltFunctions.h"
#include "Event/L0MuonCandidate.h"
#include "MuonDet/IMuonPosTool.h"

/** @class L0MuonAndTConf L0MuonAndTConf.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2007-02-21
 */
class L0MuonAndTConf : public HltAlgorithm {
public: 
  /// Standard constructor
  L0MuonAndTConf( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~L0MuonAndTConf( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  
protected:

private:
  IMuonPosTool*      m_iPosTool;
  float m_maxdist;
//  bool m_skipFilter;
  double  calcDLL(LHCb::Track* track, LHCb::MuonTileID & tileM4, LHCb::MuonTileID & tileM5);

};
#endif // L0MUONANDTCONF_H
