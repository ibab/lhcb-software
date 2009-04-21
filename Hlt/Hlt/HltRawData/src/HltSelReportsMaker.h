// $Id: HltSelReportsMaker.h,v 1.6 2009-04-21 21:40:58 snies Exp $
#ifndef HLTSELREPORTSMAKER_H 
#define HLTSELREPORTSMAKER_H 1

// Include files
// from Gaudi
#include "Event/HltObjectSummary.h"
#include "Event/Track.h"
#include "GaudiAlg/GaudiAlgorithm.h"

#include "Kernel/IANNSvc.h"
#include "HltBase/IHltDataSvc.h"

namespace LHCb {
  class CaloCluster;  
  class Particle;
  class RecVertex;
};


/** @class HltSelReportsMaker HltSelReportsMaker.h
 *  
 *  @author Tomasz Skwarnicki
 *  @date   2008-07-23
 *
 *  Algorithm to translate HltSummary  into HltSelResults and associated HltObjectSummaries
 *
 */
class HltSelReportsMaker : public GaudiAlgorithm {
public:

  enum OutputInfoLevel { kMinInfoLevel=0, ///< lhcbIDs only
                         kStandardInfoLevel=1, ///< lhcbIDs + standard numerical info(object specific)
                         kExtraInfoLevel=2, ///< lhcbIDs + extra numerical info (no standard info!)
                         kMaxInfoLevel=3 ///< lhcbIDs + standard + extra numerical info 
  };
  
  enum GlobalSelectionIDs { kHlt1GlobalID=1,
                            kHlt2GlobalID=2,
                            kL0GlobalID=3 
  };

  

  /// Standard constructor
  HltSelReportsMaker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltSelReportsMaker( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:

  /// for producing numerical info to be saved on the object
  LHCb::HltObjectSummary::Info infoToSave( const LHCb::HltObjectSummary* hos ) const;
  

  /// store Track in HltObjectSummary store
  const LHCb::HltObjectSummary* storeTrack(const LHCb::Track* object);
  /// store RecVertex in HltObjectSummary store
  const LHCb::HltObjectSummary* storeRecVertex(const LHCb::RecVertex* object);
  /// store Particle in HltObjectSummary store
  const LHCb::HltObjectSummary* storeParticle(const LHCb::Particle* object);
  /// store CaloCluster in HltObjectSummary store
  const LHCb::HltObjectSummary* storeCaloCluster(const LHCb::CaloCluster* object);
  

  /// for ranking selections
  typedef std::pair<int,std::string> RankedSelection;

  /// rank Track for selection rank
  int rankTrack(const LHCb::Track* object) const;
  /// rank RecVertex 
  int rankRecVertex(const LHCb::RecVertex* object) const;
  /// rank Particle 
  int rankParticle(const LHCb::Particle* object) const;
  /// rank CaloCluster 
  int rankCaloCluster(const LHCb::CaloCluster* object) const;
  
  /// rank LHCbIDs for selection rank
  int rankLHCbIDs( const std::vector<LHCb::LHCbID> & lhcbIDs ) const;
  
   //static bool rankSelLess( const RankedSelection & elem1, const RankedSelection & elem2);
   /// for sorting ranked selections
  class  rankSelLess {
   public:
    bool operator() (const RankedSelection & elem1, const RankedSelection & elem2 ) const;
   };


  /// for trimming output size and disabling output all together (if 0)
  unsigned int maximumNumberOfCandidatesToStore( const std::string & selectionName ) const;  

  /// set present output parameters
  void setPresentInfoLevel( const std::string & selectionName );
  

  // ----------------------- data members 

  /// location of input Hlt Dec Reports
  StringProperty m_inputHltDecReportsLocation;

  /// location of output Hlt Summary
  StringProperty m_outputHltSelReportsLocation;

  /// location of HLT muon-track-stubs 
  StringProperty m_HltMuonTracksLocation;
  /// pointer to container of the HLT muon-track-stubs
  LHCb::Tracks* m_HLTmuonTracks;

  /// HltObjectSummary container
  LHCb::HltObjectSummary::Container* m_objectSummaries;  

  /// HltANNSvc for making selection names to int selection ID
  IANNSvc* m_hltANNSvc;  
  IHltDataSvc* m_hltDataSvc;  

  std::vector<IANNSvc::minor_value_type> m_selectionNameToIntMap;

  // get trigger selection names 
  std::vector<stringKey> m_selectionIDs;




  /// for setting per selection properties
  typedef std::map<std::string,int> SelectionSetting;
  typedef SimpleProperty< std::map<std::string,int> > SelectionSettingProp;
  

  /// Present output mode (0=normal 1=debug)
  unsigned int m_debugMode;

  /// Present output info level (depends on selection context)
  unsigned int m_presentInfoLevel;
  unsigned int m_presentInfoLevelTrack;
  unsigned int m_presentInfoLevelRecVertex;
  unsigned int m_presentInfoLevelParticle;
  unsigned int m_presentInfoLevelCaloCluster;
  
  /// debug event period (global, can't be change per selection)  0=never 1=always e.g. 100=every 100th event
  UnsignedIntegerProperty m_debugPeriod;

  /// event counter (reset at every debug event)
  unsigned int m_event;
  
  /// default max number of candidates for Decision selections (can be overruled by per selection setting)
  UnsignedIntegerProperty m_maxCandidatesDecision;
  UnsignedIntegerProperty m_maxCandidatesDecisionDebug;

  /// default max number of candidates for non-Decision selections (can be overruled by per selection setting)
  UnsignedIntegerProperty m_maxCandidatesNonDecision;
  UnsignedIntegerProperty m_maxCandidatesNonDecisionDebug;
  
  /// per selection max number of candidates
  SelectionSettingProp m_maxCandidates;
  SelectionSettingProp m_maxCandidatesDebug;
  

  /// default output info level for Decision selections (can be overruled by per selection setting)
  UnsignedIntegerProperty m_infoLevelDecision;
  UnsignedIntegerProperty m_infoLevelDecisionDebug;

  /// default output info level for non-Decision selections (can be overruled by per selection setting)
  UnsignedIntegerProperty m_infoLevelNonDecision;
  UnsignedIntegerProperty m_infoLevelNonDecisionDebug;
  
  /// per selection info level
  SelectionSettingProp m_infoLevel;
  SelectionSettingProp m_infoLevelDebug;

  /// class specific enablers (particular feature must be enabled here for other settings to matter)
  UnsignedIntegerProperty m_infoLevelTrack;
  UnsignedIntegerProperty m_infoLevelTrackDebug;
  UnsignedIntegerProperty m_infoLevelRecVertex;
  UnsignedIntegerProperty m_infoLevelRecVertexDebug;
  UnsignedIntegerProperty m_infoLevelParticle;
  UnsignedIntegerProperty m_infoLevelParticleDebug;
  UnsignedIntegerProperty m_infoLevelCaloCluster;
  UnsignedIntegerProperty m_infoLevelCaloClusterDebug;

};




#endif // HLTSELREPORTSMAKER_H
