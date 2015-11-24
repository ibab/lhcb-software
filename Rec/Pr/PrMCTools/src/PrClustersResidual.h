#ifndef PRCLUSTERSRESIDUAL_H 
#define PRCLUSTERSRESIDUAL_H 1

// Include files 
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "PrKernel/PrHitManager.h"
#include "PrKernel/PrHit.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "Linker/LinkedTo.h"
#include "Linker/LinkedFrom.h"
#include "Event/Track.h"
#include "Event/MCTrackInfo.h"
#include "MCInterfaces/IIdealStateCreator.h"
#include "MCInterfaces/ILHCbIDsToMCParticles.h"
#include "MCInterfaces/ILHCbIDsToMCHits.h"
#include "Event/FTCluster.h"
#include "Event/FTLiteCluster.h"
#include "Event/MCHit.h"
//#include "Event/FTRawCluster.h"
#include "GaudiAlg/GaudiHistoTool.h"
#include "Event/MCParticle.h"                                  
#include "Event/Track.h"
#include "Linker/LinkerTool.h"
#include "PrKernel/IPrCounter.h"

/** @class PrClustersResidual PrClustersResidual.h
 *  Make nTuples of Cluster vs MCHits and Hit content on track + Occupancy study possible ( also in PrPlotFTHits )
 *  -MCHitsLocation : Where to find the MCHits ( /Event/MC/FT/FTHits by default )
 *  -HitManagerName : The FTHitManage ( PrFTHitManager ) 
 *  -DoClusterResidual : Will produce nTuple of cluster  info vs MCHit contributing to it info
 *  -DoTrackStudy : Will produce nTuple filled by MCParticle with arrays of MCHits , Clusters, PrHit associated to it. Possible to do study offline trough Root macros.
 *  -DecodeData : true or false, set to true if no Tracking algorithms ( forward or seeding are run ) . False by default.
 *  -OnlyHasT: process only MCParticle which are reconstructible in the scifi
 *  -RemoveClones : Hack to remove duplicates MCHits / Clusters / PrHit in same layers ( set always to True )
 *  -DumpAllHits : produce nTuple filled by hit ( huge nTuples produced, set to False by Default )
 *  @author renato quagliani
 *  @date   2015-01-29
 */
class PrClustersResidual : public GaudiTupleAlg {
public: 
  /// Standard constructor
  PrClustersResidual( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PrClustersResidual( ); ///< Destructor
  
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  
protected:
  
private:
  
  void HitEfficiency();
  
  /** @brief Create Ntuple to study clusters and PrHit generated.
   */
  void ClusterResidual();
  
  /**@brief Create Ntuple containing PrHits for track study plus all the MCHits.
   */
  void TrackStudy();
  
  /**@brief Create Ntuple containing all the PrHits in the event.
   */
  void DumpAllHits();
  


  void Occupancy();
  
  typedef std::vector< PrHit* > Hits;
  typedef FastClusterContainer<LHCb::FTLiteCluster,int> FTLiteClusters;
  
  /// range of object for Hits
  typedef Gaudi::Range_< Hits > HitRange;
  
  std::vector<const LHCb::Track*> getTrack(const LHCb::LHCbID id, const std::string location);
  LHCb::FTLiteCluster getLiteCluster(const LHCb::LHCbID id);
  
  std::string m_mcHitLocation;
  
  //  unsigned int m_zone;
  bool m_debugTracking;
  
  int m_nEvents;
  
  double m_NClusters ;
  double m_NMCHit ;
  double m_NMCHit_inClusters;
  bool m_onlyHasT;
  PrHitManager* m_ftHitManager;
  unsigned int m_zone;
  
  bool m_decodeData;
  bool m_removeClones;
  bool m_dumpAllHits;
  bool m_Occupancy;
  std::string m_hitManagerName;
  bool m_doClusterResidual;
  bool m_doTrackStudy;
  
  //from PrCounter
  // typedef LinkerTool<LHCb::Track, LHCb::MCParticle> MyAsct;  
  // typedef MyAsct::DirectType            Table;  
  // typedef MyAsct::DirectType::Range     Range;
  // typedef Table::iterator               iterator;                     
  // typedef MyAsct::InverseType           InvTable;
  // typedef InvTable::Range               InvRange;
  // typedef InvTable::iterator            InvIterator;
  // MyAsct*         m_link;
  // const InvTable* m_invTable;                                  
};
#endif // PRCLUSTERSRESIDUAL_H
