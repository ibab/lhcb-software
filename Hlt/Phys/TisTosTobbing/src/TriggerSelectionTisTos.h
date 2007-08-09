// $Id: TriggerSelectionTisTos.h,v 1.1.1.1 2007-08-09 01:31:19 tskwarni Exp $
#ifndef TRIGGERSELECTIONTISTOS_H 
#define TRIGGERSELECTIONTISTOS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "TisTosTobbing/ITriggerSelectionTisTos.h"            // Interface

#include "CaloInterfaces/ITrack2Calo.h"
#include "HltBase/HltConfigurationHelper.h"

namespace LHCb {
  class HltSummary;
};
  
/** @class TriggerSelectionTisTos TriggerSelectionTisTos.h
 *  
 *
 *  @author Tomasz Skwarnicki
 *  @date   2007-08-06
 */
class TriggerSelectionTisTos : public GaudiTool, virtual public ITriggerSelectionTisTos {
public: 

  // hits are split into following categories for individual matching
  enum HitType {kVelo,kTT,kOTIT,kMuon,kEcal,kHcal,nHitTypes,kNotUsed=nHitTypes};

  typedef std::vector<LHCb::LHCbID> ClassifiedHits;


  /// Standard constructor
  TriggerSelectionTisTos( const std::string& type, 
                          const std::string& name,
                          const IInterface* parent);

  virtual ~TriggerSelectionTisTos( ); ///< Destructor


  virtual StatusCode         initialize();
  
  // ------------  various ways to define off-line input

  void setOfflineInput( ); // erase previous input 
 
  //    Detector hit input 
  void addToOfflineInput( const std::vector<LHCb::LHCbID> & hitlist, ClassifiedHits hitidlist[] );
  void addToOfflineInput( const std::vector<LHCb::LHCbID> & hitlist ){ addToOfflineInput( hitlist, m_offlineInput );  }
  


  //    Track input 
  void addToOfflineInput( const LHCb::Track & track,  ClassifiedHits hitidlist[] );  
  void addToOfflineInput( const LHCb::Track & track ){ addToOfflineInput( track, m_offlineInput );  }
  
  
  //    Proto-particle input
  void addToOfflineInput( const LHCb::ProtoParticle & protoParticle, ClassifiedHits hitidlist[]  ); 
  void addToOfflineInput( const LHCb::ProtoParticle & protoParticle ){ addToOfflineInput( protoParticle, m_offlineInput );  }
  
  //    Particle input; for composite particles loop over daughters will be executed
  void addToOfflineInput( const LHCb::Particle & particle, ClassifiedHits hitidlist[] ); 
  void addToOfflineInput( const LHCb::Particle & particle ){ addToOfflineInput( particle, m_offlineInput );  }  

  // -------------- outputs:
  //
  //    decision =  was trigger satisfied ? (this is independent of offline input passed to this tool!)
  //    tis      =  Trigger Independent of Signal  
  //                (trigger selection was satisified independently of the offline input passed to this tool)
  //    tos      =  Trigger On Signal  
  //                (the offline input passed to this tool was sufficient to satisfy this trigger selection)
  //   
  //    comments:
  //         if decision==false then tis=false and tos=false 
  //
  //         tis, tos are not mutually exclusive ( tis==true and tos=true is possible, TOE in TriggerSource terminology)
  //
  //         TOB (Trigger On Both ) =  decision && !tis && !tos  
  //           

  // ------------ single selection summary TisTos
  void selectionTisTos( std::string selectionName,  
                                bool & decision, bool & tis, bool & tos ); 


  // ------------ multiple selections summary TisTos
  void selectionTisTos( std::vector< std::string > selectionNames,
                                bool & decision, bool & tis, bool & tos ,
                                bool selectionOR = true);  // set to false if an AND between selections is required

  
  // ------------ auxiliary output:  list of LHCbIDs corresponding to present offline input
  std::vector<LHCb::LHCbID> offlineLHCbIDs(); 

  // ------------  additional functionality:  lists of tracks/vertices/particles from selection summary
  //               satisfying TOS, ordered according to TOS quality (best first)
  //               return empty vector in case of a mismatch between the output type and the selection summary
  std::vector<const LHCb::Track*>     matchedTOSTracks( std::string selectionName );
  std::vector<const LHCb::RecVertex*> matchedTOSVertices( std::string selectionName );
  std::vector<const LHCb::Particle*>  matchedTOSParticles( std::string selectionName );


  // --------------------- utilities ------------------------

  static int hitMatchType(const LHCb::LHCbID & id);

  static void matchIDs(const LHCb::Track & track, 
                const ClassifiedHits offidlist[],
                double overlap[] );
    
  static std::vector<const LHCb::Particle*> finalStateParticles(const LHCb::Particle* particle);

protected:

private:


  void trackListTISTOS(const std::vector<LHCb::Track*> & ontracks,
                       const ClassifiedHits offidlist[],
                       bool& TIS,bool& TOS) const;

  void vertexListTISTOS(const std::vector<LHCb::RecVertex*> & onvertices,
                        const ClassifiedHits offidlist[],
                        bool& TIS,bool& TOS) const;

  void particleListTISTOS(const SmartRefVector<LHCb::Particle> & onparticles,
			  const ClassifiedHits offidlist[],
			  bool& TIS,bool& TOS) const;

  void getHltSummary();

  //  -------------------------- data members --------------------

  double m_TISFrac[nHitTypes]; // maximum fraction of matching hits allowed for TIS
  double m_TOSFrac[nHitTypes]; // minimum fraction of matching hits required for TOS
 
  ClassifiedHits m_offlineInput[nHitTypes];

  ITrack2Calo*  m_track2calo;
  DeCalorimeter* m_hcalDeCal;
  DeCalorimeter* m_ecalDeCal;

  std::string m_MuonTracksLocation;
  LHCb::Tracks* m_muonTracks;
  
  std::string m_HltSummaryLocation;
  LHCb::HltSummary* m_summary;
  Hlt::Configuration* m_hltconf;

  int m_warning_count;  
  
};
#endif // TRIGGERSELECTIONTISTOS_H
