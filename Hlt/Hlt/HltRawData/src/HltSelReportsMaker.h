// $Id: HltSelReportsMaker.h,v 1.13 2010-07-17 06:11:31 tskwarni Exp $
#ifndef HLTSELREPORTSMAKER_H
#define HLTSELREPORTSMAKER_H 1

// Include files
#include <string>
// from Gaudi
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/VectorMap.h"
#include "Event/HltObjectSummary.h"
#include "Event/Track.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Event/RelatedInfoMap.h"


#include "Kernel/IANNSvc.h"
#include "HltBase/IHltRegister.h"
#include "HltBase/IHltData.h"
#include "HltBase/IHltInspector.h"
#include "HltDAQ/IReportConvert.h"
#include "HltDAQ/ReportConvertTool.h"

namespace LHCb {
  class CaloCluster;
  class Particle;
  class RecVertex;
  class HltSelReports;
  class HltDecReports;
}

/** @class HltSelReportsMaker HltSelReportsMaker.h
 *
 *  @author Tomasz Skwarnicki
 *  @date   2008-07-23
 *
 *  Algorithm to translate HltSummary  into HltSelResults and associated HltObjectSummaries
 *
 */
class HltSelReportsMaker : public GaudiAlgorithm
                         , virtual public IIncidentListener {
public:

  enum OutputInfoLevel { kMinInfoLevel=0, ///< lhcbIDs only
                         kStandardInfoLevel=1, ///< lhcbIDs + standard numerical info(object specific)
                         kExtraInfoLevel=2, ///< lhcbIDs + extra numerical info (no standard info!)
                         kMaxInfoLevel=3 ///< lhcbIDs + standard + extra numerical info
  };

  enum GlobalSelectionIDs { kHlt1GlobalID=1,
                            kHlt2GlobalID=2
  };


  /// Standard constructor
  HltSelReportsMaker( const std::string& name, ISvcLocator* pSvcLocator,
                      const Gaudi::StringKey idKey);

  virtual ~HltSelReportsMaker( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
   
  void handle(const Incident&);

protected:

  virtual StatusCode postExecute(LHCb::HltSelReports* outputSummary,
                                 const LHCb::HltDecReports* decReports) = 0;

  LHCb::HltObjectSummary::Container& objectSummaries() const { return *m_objectSummaries; }

  IANNSvc* hltANNSvc() const { return m_hltANNSvc; }

  Gaudi::StringKey idKey() const { return m_idKey; }

  IReportConvert* reportConverter() const { return m_conv; }

private:

  /// for producing numerical info to be saved on the object
  LHCb::HltObjectSummary::Info infoToSave( const LHCb::HltObjectSummary& hos ) const;

  /// for splitting strings
  std::vector<std::string> &split(const std::string &, char, std::vector<std::string> &);
  std::vector<std::string> split(const std::string &, char);

  /// for converting objects in to summaries
  IReportConvert* m_conv;

  /// store Track in HltObjectSummary store
  const LHCb::HltObjectSummary* store_(const LHCb::Track& object);
  /// store RecVertex in HltObjectSummary store
  const LHCb::HltObjectSummary* store_(const LHCb::RecVertex& object);
  /// store Vertex in HltObjectSummary store
  const LHCb::HltObjectSummary* store_(const LHCb::Vertex& object);
  /// store RichPID in HltObjectSummary store
  const LHCb::HltObjectSummary* store_(const LHCb::RichPID& object);
  /// store MuonPID in HltObjectSummary store
  const LHCb::HltObjectSummary* store_(const LHCb::MuonPID& object);
  /// store Particle in HltObjectSummary store
  const LHCb::HltObjectSummary* store_(const LHCb::Particle& object);
  /// store ProtoParticle in HltObjectSummary store
  const LHCb::HltObjectSummary* store_(const LHCb::ProtoParticle& object);
  /// store CaloCluster in HltObjectSummary store
  const LHCb::HltObjectSummary* store_(const LHCb::CaloCluster& object);
  /// store RelatedInfoMap in HltObjectSummary store
  const LHCb::HltObjectSummary* store_(std::pair<int,const LHCb::RelatedInfoMap&> object);

  template <typename T> const LHCb::HltObjectSummary* store(const ContainedObject* obj) {
      auto t = dynamic_cast<const T*>(obj);
      if (!t) return nullptr;
      auto i = std::find_if( std::begin(*m_objectSummaries), std::end(*m_objectSummaries),
                             [&t]( const LHCb::HltObjectSummary* hos ) {
            return hos->summarizedObjectCLID() == t->clID() && hos->summarizedObject() == t ;
      });
      return ( (i!=std::end(*m_objectSummaries)) && (m_Turbo == false) ) ? *i : store_(*t);
  }



  /// rank Track for selection rank
  int rank_(const LHCb::Track& object) const;
  /// rank RecVertex
  int rank_(const LHCb::RecVertex& object) const;
  /// rank Vertex
  int rank_(const LHCb::Vertex& object) const;
  /// rank ProtoParticle
  int rank_(const LHCb::ProtoParticle& object) const;
  /// rank RichPID
  int rank_(const LHCb::RichPID& object) const;
  /// rank MuonPID
  int rank_(const LHCb::MuonPID& object) const;
  /// rank Particle
  int rank_(const LHCb::Particle& object) const;
  /// rank CaloCluster
  int rank_(const LHCb::CaloCluster& object) const;
  /// rank RecSummary
  int rank_(const LHCb::RecSummary& object) const;

  template <typename T> bool rank(const ContainedObject* obj, int& rnk) {
      const T* c = dynamic_cast<const T*>(obj);
      bool ret = (c!=0);
      if (ret) rnk = rank_(*c);
      return ret;
  }

  /// rank LHCbIDs for selection rank
  int rankLHCbIDs( const std::vector<LHCb::LHCbID> & lhcbIDs ) const;


  /// for trimming output size and disabling output all together (if 0)
  unsigned int maximumNumberOfCandidatesToStore( const std::string & selectionName ) const;

  /// set present output parameters
  void setPresentInfoLevel( const std::string & selectionName );


  // ----------------------- data members

  /// location of input Hlt Dec Reports
  StringProperty m_inputHltDecReportsLocation;

  /// location of output Hlt Summary
  StringProperty m_outputHltSelReportsLocation;

  /// HltObjectSummary container
  LHCb::HltObjectSummary::Container* m_objectSummaries;
   
  /// HltANNSvc for making selection names to int selection ID
  IANNSvc* m_hltANNSvc;
  Hlt::IData* m_hltSvc;
  Hlt::IRegister* m_regSvc;
  Hlt::IInspector* m_inspectionSvc;

  // RelatedInfo
  std::map<std::string,std::vector<std::string>> m_RelInfoLocationsMap;
  std::vector<std::pair<std::string,int> > m_RelInfoToSave;

  // from info id to its name
  GaudiUtils::VectorMap< int, std::string > m_infoIntToName;

  // get trigger selection names //  -- amalgamate into vector<struct>
  struct selectionInfo {
      const Hlt::Selection* selection;
      Gaudi::StringKey id;
      int intId;
      int maxCand;
      int maxCandDebug;
  };
  std::vector< selectionInfo> m_selectionInfo;

  /// for ranking selections
  typedef std::pair<int, selectionInfo > RankedSelection;
   //static bool rankSelLess( const RankedSelection & elem1, const RankedSelection & elem2);
   /// for sorting ranked selections
  class  rankSelLess {
   public:
    bool operator() (const RankedSelection & elem1, const RankedSelection & elem2 ) const;
   };


  /// for setting per selection properties
  typedef std::map<std::string,int> SelectionSetting;
  typedef SimpleProperty< std::map<std::string,int> > SelectionSettingProp;

  /// cache last PV selection name
  std::string m_lastPVSelectionName;
  int m_intPVSelID;

  /// Where is the RecSummary location
  string m_RecSummaryLoc;
  /// Do we want extended reports for Turbo stream
  bool m_Turbo;

  /// Present output mode (false=normal true=debug)
  bool m_debugMode;
  std::string m_debugIncidentName;

  /// Present output info level (depends on selection context)
  unsigned int m_presentInfoLevel;
  unsigned int m_presentInfoLevelTrack;
  unsigned int m_presentInfoLevelRecVertex;
  unsigned int m_presentInfoLevelParticle;
  unsigned int m_presentInfoLevelCaloCluster;
  unsigned int m_presentInfoLevelSelection;

  /// Which selectionIDs should be stored
  const  Gaudi::StringKey m_idKey;

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
  UnsignedIntegerProperty m_infoLevelSelection;
  UnsignedIntegerProperty m_infoLevelSelectionDebug;

  /// whether selections killed by postscale are allowed to save candidates
  bool m_SuppressPostscale;
  bool m_SuppressPostscaleDebug;

  int bookmark;
};




#endif // HLTSELREPORTSMAKER_H
