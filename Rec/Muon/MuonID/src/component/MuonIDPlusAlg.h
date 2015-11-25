#ifndef COMPONENT_MUONIDPLUSALG_H 
#define COMPONENT_MUONIDPLUSALG_H 1


#include "GaudiAlg/GaudiAlgorithm.h"
class IMuonIDTool;

/** @class MuonIDPlusAlg MuonIDPlusAlg.h component/MuonIDPlusAlg.h
 *  
 *  novel MuonID algorithm based on the MuonIDPlusTool
 *
 *  @author Giacomo Graziani
 *  @date   2015-11-18
 */
class MuonIDPlusAlg : public GaudiAlgorithm {
public: 
  /// Standard constructor
  MuonIDPlusAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MuonIDPlusAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization


private:
  IMuonIDTool* m_muonIDtool;
  std::string m_BestTrackLocation;
  std::string m_MuonPIDsPath;
  std::string m_MuonTracksPath;
};
#endif // COMPONENT_MUONIDPLUSALG_H
