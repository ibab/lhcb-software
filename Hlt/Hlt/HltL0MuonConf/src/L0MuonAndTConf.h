// $Id: L0MuonAndTConf.h,v 1.1.1.1 2007-05-22 10:20:31 asatta Exp $
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
  double  calcDLL(LHCb::Track* track);
  
protected:

private:
  IMuonPosTool*      m_iPosTool;
  float m_maxdist;
  bool m_skipFilter;
};
#endif // L0MUONANDTCONF_H
