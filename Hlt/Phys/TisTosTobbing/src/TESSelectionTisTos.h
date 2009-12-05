// $Id: TESSelectionTisTos.h,v 1.1 2009-12-05 00:51:31 tskwarni Exp $
#ifndef TESSELECTIONTISTOS_H 
#define TESSELECTIONTISTOS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/ITriggerSelectionTisTos.h"            // Interface

#include "CaloInterfaces/ITrack2Calo.h"

#include "Event/HltObjectSummary.h"
#include "Event/Particle.h"
#include "Event/RecVertex.h"
#include "Event/Track.h"

#include "GaudiKernel/IIncidentListener.h"

/** @class TESSelectionTisTos TESSelectionTisTos.h
 *  
 *  @author Tomasz Skwarnicki
 *  @date   2009-12-03
 *
 *  This is a special version of TisTosTobbing of TES Selections with candidates saved on TES
 *  This code is general enough to TisTos any Selection of Particles, Tracks, RecVertices saved on TES 
 *   with respect to user defined "Signal"   
 *
 *  Hit based implementation of Tis,Tos'ing TES Selection(s).
 *  @sa  ITriggerSelectionTisTos docs for more explanation.
 *  This interface also defines inlined shortcuts to set Offline Input and get an output in one call. 
 */
class TESSelectionTisTos :  public GaudiTool, 
                               virtual public IIncidentListener, 
                               virtual public ITriggerSelectionTisTos {
public: 

  /// hits are split into @c HitType categories for matching in each category 
  enum HitType {kVelo,kTT,kOTIT,kMuon,kEcal,kHcal,nHitTypes,kNotUsed=nHitTypes};

  /// ClassifiedHits arrays of dimension @c nHitTypes are used as argument in internal methods 
  typedef std::vector<LHCb::LHCbID> ClassifiedHits;


  /// Standard constructor
  TESSelectionTisTos( const std::string& type, 
                            const std::string& name,
                            const IInterface* parent);

  virtual ~TESSelectionTisTos( ); ///< Destructor


  virtual StatusCode         initialize();
  virtual void handle(const Incident&);
 
  // ------------  various ways to define off-line input -------------------------

  /// erase previous input 
  void setOfflineInput( ); 
 
  ///    Detector hit input 
  void addToOfflineInput( const std::vector<LHCb::LHCbID> & hitlist, ClassifiedHits hitidlist[] );
  ///    Detector hit input seen via interface
  void addToOfflineInput( const std::vector<LHCb::LHCbID> & hitlist ){ addToOfflineInput( hitlist, m_offlineInput );  }

  ///    Calo hit input 
  void addToOfflineInput( const std::vector<LHCb::CaloCellID> & hitlist, ClassifiedHits hitidlist[] );

  ///    Track input 
  void addToOfflineInput( const LHCb::Track & track,  ClassifiedHits hitidlist[] );  
  ///    Track input seen via interface
  void addToOfflineInput( const LHCb::Track & track ){ addToOfflineInput( track, m_offlineInput );  }
  
  ///    Proto-particle input
  void addToOfflineInput( const LHCb::ProtoParticle & protoParticle, ClassifiedHits hitidlist[]  ); 
  ///    Proto-particle input seen via interface
  void addToOfflineInput( const LHCb::ProtoParticle & protoParticle ){ addToOfflineInput( protoParticle, m_offlineInput );  }
  
  ///    Particle input; for composite particles loop over daughters will be executed
  void addToOfflineInput( const LHCb::Particle & particle, ClassifiedHits hitidlist[] ); 
  ///    Particle input seen via interface
  void addToOfflineInput( const LHCb::Particle & particle ){ addToOfflineInput( particle, m_offlineInput );  }  


  // ------------  TIS,TOS output functions  ------------------------

  /// single TES Selection Summary TisTos  (define Offline Input before calling)
  void selectionTisTos( const std::string & selectionName,  
                        bool & decision, bool & tis, bool & tos ); 


  /// multiple TES Selection Summaries TisTos (set selectionOR=false for an AND) (define Offline Input before calling)
  void selectionTisTos( const std::vector< std::string > & selectionNames,
                        bool & decision, bool & tis, bool & tos ,
                        bool selectionOR = kSelectionDefaultLogic );  

  
  // ------------ auxiliary outputs ---------------------------------

  /// list of LHCbIDs corresponding to present Offline Input (only hits used in matching are returned)
  std::vector<LHCb::LHCbID> offlineLHCbIDs(); 

  /// list of HltObjectSummaries from Selection Summary satisfying TOS,TIS requirements (define Offline Input before calling)
  std::vector<const LHCb::HltObjectSummary*> hltSelectionObjectSummaries( const std::string & selectionName,
                                                                          unsigned int tisRequirement      = kAnything,
                                                                          unsigned int tosRequirement      = kAnything );


  /// templated method to convert object summaries to objects themselves
  template <class T>
  std::vector<const T*> hltObjects(const std::vector<const LHCb::HltObjectSummary*> & hosVec )
  {
    std::vector<const T*> hoVec;
    for( std::vector<const LHCb::HltObjectSummary*>::const_iterator phos=hosVec.begin();
         phos!=hosVec.end();++phos){
      const LHCb::HltObjectSummary & hos = **phos;
      if( !hos.summarizedObject() )break; // means pointers are not available    
      if( hos.summarizedObjectCLID() != T::classID()  )break; // means selection has different objects than requested
      hoVec.push_back( dynamic_cast<const T*>( hos.summarizedObject() ) );
    }
    return hoVec;
  }

  /// list of tracks from Selection Summary (none if mismatch) satisfying TOS (define Offline Input before calling)
  std::vector<const LHCb::Track*>     matchedTOSTracks( const std::string & selectionName )  
  {
    return hltObjects<LHCb::Track>( hltSelectionObjectSummaries(selectionName,kAnything,kTrueRequired) );
  }

  /// list of vertices from Selection Summary (none if mismatch) satisfying TOS (define Offline Input before calling)
  std::vector<const LHCb::RecVertex*> matchedTOSVertices( const std::string & selectionName )
  {
    return hltObjects<LHCb::RecVertex>( hltSelectionObjectSummaries(selectionName,kAnything,kTrueRequired) );
  }

  /// list of particles from Selection Summary (none if mismatch) satisfying TOS (define Offline Input before calling)
  std::vector<const LHCb::Particle*>  matchedTOSParticles( const std::string & selectionName ) 
  {
    return hltObjects<LHCb::Particle>( hltSelectionObjectSummaries(selectionName,kAnything,kTrueRequired) );
  }


  // --------------------- utilities ------------------------

  /// classify hit into @c HitType
  static int hitMatchType(const LHCb::LHCbID & id);

  /** @par matchIDs
   *  Dimension of @c offidlist (input) and @c overlap (output) arrays is @c nHitTypes
   *  @par
   *  Returned overlap values are in the range [0.,1.] if hits of that type are present on the input Track, 
   *  otherwise the returned value is 2.0
   */ 

  /// calculate hit overlap between the (trigger) @c Track and set of @c ClasssifiedHits (Offline Input) for each @c HitType
  static void matchIDs(const LHCb::Track & track, 
                       const ClassifiedHits offidlist[],
                       double overlap[] );
    
  /// return TIS,TOS for a vector of (trigger) Tracks with respect to a set of @c ClasssifiedHits (here from Offline Input)
  void trackListTISTOS(const std::vector<LHCb::Track*> & ontracks,
                       const ClassifiedHits offidlist[],
                       bool& TIS,bool& TOS) const;

  /// return TIS,TOS for a vector of (rec) Vertices with respect to a set of @c ClasssifiedHits (here from Offline Input)
  void vertexListTISTOS(const std::vector<LHCb::RecVertex*> & onvertices,
                        const ClassifiedHits offidlist[],
                        bool& TIS,bool& TOS) const;

  /// return TIS,TOS for a vector of (particle) Vertices with respect to a set of @c ClasssifiedHits (here from Offline Input)
  void pvertexListTISTOS(const std::vector<LHCb::Vertex*> & onvertices,
                        const ClassifiedHits offidlist[],
                        bool& TIS,bool& TOS);

  /// return TIS,TOS for a vector of (trigger) Particles with respect to a set of @c ClasssifiedHits (here from Offline Input)
  void particleListTISTOS(const std::vector<LHCb::Particle*> & onparticles,
                          const ClassifiedHits offidlist[],
                          bool& TIS,bool& TOS);

  /// for given particle returns descendent daughters (of any generation) which have no daughters themselves 
  static std::vector<const LHCb::Particle*> finalStateParticles(const LHCb::Particle* particle);

protected:

  /// get Hlt Summary and configuration
  void getHltSummary();
  
  bool m_newEvent;

  /// Prefix in TES (prefix for TES locations if the selection-name is not absolute path itself)
  StringProperty m_PrefixInTES;

private:

  // internal Cache of results used as long as the Offline Input remains the same
 
  void clearCache()
  {
    m_cached_SelectionNames.clear();
    m_cached_decision.clear();
    m_cached_tis.clear();
    m_cached_tos.clear();
  }
  
  void storeInCache(const std::string & selectionName, bool decision, bool tis, bool tos)
  {
    m_cached_SelectionNames.push_back(selectionName);
    m_cached_decision.push_back(decision);
    m_cached_tis.push_back(tis);
    m_cached_tos.push_back(tos);
  }
  
  bool findInCache(const std::string & selectionName, bool & decision, bool & tis, bool & tos)
  {
    std::vector< std::string >::iterator found =
      std::find( m_cached_SelectionNames.begin(),m_cached_SelectionNames.end(), selectionName);
    if( found == m_cached_SelectionNames.end() )return false;
    int index ( found - m_cached_SelectionNames.begin() );
    decision = m_cached_decision[index];
    tis =      m_cached_tis[index];
    tos =      m_cached_tos[index];
    return true;
  }  

  //  -------------------------- data members --------------------

  /// maximum fraction of matching hits allowed for TIS
  double m_TISFrac[nHitTypes]; 
  /// minimum fraction of matching hits required for TOS
  double m_TOSFrac[nHitTypes]; 
 
  /// to allow disabling collecting offline hit ids via Ecal hypotheses
  bool m_noCaloHypo;

  /// Offline Input stored as classified hits
  ClassifiedHits m_offlineInput[nHitTypes];

  /// Tool for projecting track into Hcal and Ecal 
  ITrack2Calo*  m_track2calo;
  /// Hcal detector geometry
  DeCalorimeter* m_hcalDeCal;
  /// Ecal detector geometry
  DeCalorimeter* m_ecalDeCal;

  /// Location of Muon Tracks holding muon hits for protoparticles
  StringProperty m_MuonTracksLocation;
  StringProperty m_HltMuonTracksLocation;
  /// Extracted muon track container 
  LHCb::Tracks* m_muonTracks;
  LHCb::Tracks* m_HLTmuonTracks;
  /// flag to disable muon hits if muon tracks cannot be found
  bool m_muonsOff;

  // Cache of results for the same Offline Input
  std::vector< std::string > m_cached_SelectionNames;
  std::vector< bool >        m_cached_decision;
  std::vector< bool >        m_cached_tis;
  std::vector< bool >        m_cached_tos;
  
  /// HltObjectSummary container
  LHCb::HltObjectSummary::Container* m_objectSummaries;  

};
#endif // TESSELECTIONTISTOS_H
