// $Id: $

#ifndef PRCHECKER2_H
#define PRCHECKER2_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"

// local
//#include "PrCounter.h"
#include "PrKernel/IPrCounter.h"
#include "PrKernel/IPrTTCounter.h"
#include "LoKi/IMCHybridFactory.h"
#include "LoKi/Primitives.h"
#include "LoKi/MCParticles.h"
#include "MCInterfaces/IMCReconstructible.h"



class IHistoTool ;


/** @class PrChecker2 PrChecker2.h
 *  Check the quality of the pattern recognition, by comparing to MC information
 *  Produces efficiency, ghost rate and clone rate numbers.
 *  Parameters:
 *   - Eta25Cut: Only consider particles with 2 < eta < 5? (default: false)
 *   - TriggerNumbers: Give numbers for p > 3GeV, pT > 500 MeV? (default: false)
 *     if selected long_fromB_P>3GeV_Pt>0.5GeV cut is added to each track container
 *   - VetoElectrons: Take electrons into account in numbers? (default: true)
 *   - TrackExtrapolation: Do track extrapolation into middle of T and TTstations for efficiency maps (default: false)
 *   - Upgrade: Gets hitLocation from link to MCParticle from "Pr/LHCbID" instead of "Pat/LHCbID" (default: true)
 * 
 * 
 * additional track location container can be added for PrCounter2 and PrTTCounter via:
 *
 * @code
 *   from Configurables import PrChecker2
 *   #to fix the LokiGendecorator problem
 *   from Configurables import LoKi__Hybrid__MCTool
 *   myFactory = LoKi__Hybrid__MCTool("MCHybridFactory")
 *   myFactory.Modules = [ "LoKiMC.decorators" ]
 *   PrChecker2("PrChecker2").addTool( myFactory )
 *
 *   PrChecker2("PrChecker2").NewTracks= "Rec/Track/Velo" 
 *   PrChecker2("PrChecker2").WriteNewHistos = 2
 *   PrChecker2("PrChecker2").SelectIdNewContainer = 1
 *   PrChecker2("PrChecker2").MyNewCuts= {"fromBorD" : "BOrDMother", "B daughters": "fromB", "D daughters": "fromD" }
 *  @endcode
 *
 *  TrackSelector to change definition of reconstructed tracks, change MaxChi2Cut for example:
 *
 *  @code
 *   from Configurables import TrackSelector
 *   PrChecker2("PrChecker2").addTool(PrCounter2, name="Velo")
 *   PrChecker2("PrChecker2").Velo.addTool(PrCounter2, name="Selector")
 *   PrChecker2("PrChecker2").Velo.Selector = "TrackSelector"
 *   PrChecker2("PrChecker2").Velo.addTool(TrackSelector("TrackSelector"))
 *   PrChecker2("PrChecker2").Velo.TrackSelector.MaxChi2Cut = 5.0 
 *  @endcode
 *
 *  Uses IHistoTool: for each container histograms are plotted if the following code segment is used. 
 *  Default values are -1 (no histograms), can also be set to 2 for additional histograms (expectedHits, docaz, PVz, EtaP, EtaPhi, efficiency maps @z=9000mm XYZ9000 and @z=2485mm XYZ2485) 
 * 
 *  @code
 *  PrChecker2("PrChecker2").Write(container)Histos = 1
 *  @endcode
 *
 *  Configurable selection cuts for each container, can be changed via:
 *
 *  @code
 *  PrChecker2("PrChecker2").MyForwardCuts = {"long" : "isLong", "B daughter": "fromB" }
 *  @endcode
 *
 *  As a default selection cuts of old PrChecker are used. The following cuts are predefined: 
 *  - is(Not)Long, is(Not)Velo, is(Not)Down, is(Not)Up, is(Not)TT, is(Not)Seed, 
 *  - fromB, fromD, BOrDMother, fromKsFromB, strange 
 *  - is(Not)Electron, eta25, over5, trigger
 *  
 *  and LoKi syntax (LoKi::MCParticles) can be used for kinematical cuts, e.g. (MCPT> 2300), here the '()' are essential.
 * 
 * 
 *  NB: If you care about the implementation: The cut-strings are converted into two types of functors: 
 *  - LoKi-type functors (hence all LoKi::MCParticles cuts work) 
 *  - and custom-defined ones, mostly for type of reconstructibility and daughter-criteria (like 'isNotLong')
 *  where in the end all functors are evaluated on each MCParticle for each track container to define the reconstructibility.
 *  If a MCParticle is actually reconstructed is checked in the PrCounter2.
 *  A large part of the code just deals with the conversion of the strings into functors.
 *
 *
 *  @author Olivier Callot, Thomas Nikodem, Svende Braun, Michel De Cian
 *  @date   2015-01-17
 */

class EffCounter {
public:
  EffCounter() {
    nEvt    = 0;
    total   = 0;
    ghost   = 0;
    nLong   = 0;
    okLong  = 0;
    nLong5 = 0;
    okLong5 = 0;
  };
  ~EffCounter() {};
  int nEvt;
  int total;
  int ghost;
  int nLong;
  int okLong;
  int nLong5;
  int okLong5;
};



class PrChecker2 : public GaudiHistoAlg {
public:
  
  /// Standard constructor
  PrChecker2( const std::string& name, ISvcLocator* pSvcLocator );
  
  virtual ~PrChecker2( ); ///< Destructor
  
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  
protected:
  
  
private:

  // typedefs to make everything a bit more readable
  typedef std::vector<std::string>            strings;
  typedef std::map<std::string, std::string>  stringmap;
  typedef std::pair<std::string, std::string> stringpair;
  

  std::string m_veloTracks;
  std::string m_forwardTracks;
  std::string m_matchTracks;
  std::string m_seedTracks;
  std::string m_downTracks;
  std::string m_upTracks;
  std::string m_bestTracks;
  std::string m_newTracks;///< additional configurable track container for PrCounter2
  std::string m_ttnewTracks;///< additional configurable track container for PrTTCounter
  
  IPrCounter* m_velo;
  IPrCounter* m_forward;
  IPrCounter* m_match;
  IPrCounter* m_upTrack;
  IPrCounter* m_tTrack;
  IPrCounter* m_downTrack;
  IPrCounter* m_best;
  IPrCounter* m_bestLong;
  IPrCounter* m_bestDownstream;
  IPrCounter* m_new;
  
  IPrTTCounter* m_ttForward;
  IPrTTCounter* m_ttMatch;
  IPrTTCounter* m_ttDownst;
  IPrTTCounter* m_ttnew;
  
  //IPrCounter* m_utForward;
  //IPrCounter* m_utMatch;
  //IPrCounter* m_utbestLong;
  //IPrCounter* m_utDownst;
  
  int  m_writeVeloHistos;      
  int  m_writeForwardHistos;   
  int  m_writeMatchHistos;     
  int  m_writeDownHistos;      
  int  m_writeUpHistos;        
  int  m_writeTTrackHistos;    
  int  m_writeBestHistos;
  int  m_writeBestLongHistos;
  int  m_writeBestDownstreamHistos;
  int  m_writeNewHistos; 
  int  m_writeUTHistos;
  int  m_writeTTForwardHistos;
  int  m_writeTTMatchHistos;
  int  m_writeTTDownstHistos;
  int  m_writeTTNewHistos;
  
  int m_selectIdNew;
  int m_selectIdNewTT;
  
  bool m_eta25cut;             
  bool m_triggerNumbers;
  bool m_vetoElectrons;
  bool m_trackextrapolation;
  
  bool m_upgrade;
  
  enum recAs {
    isLong = 1,
    isNotLong = 2,
    isDown = 3,
    isNotDown = 4,
    isUp = 5,
    isNotUp = 6,
    isVelo = 7,
    isNotVelo = 8,
    isTT = 9,
    isNotTT = 10,
    isSeed = 11,
    isNotSeed = 12,
    strange = 13,
    fromB = 15,
    fromD = 16,
    fromKsFromB = 17,
    isElectron = 18,
    isNotElectron = 19,
    BOrDMother = 20,
  };
  
  std::map< std::string, recAs> m_lookuptable = {{"isLong",isLong},
                                                 {"isNotLong",isNotLong},
                                                 {"isDown",isDown}, 
                                                 {"isNotDown",isNotDown},
                                                 {"isUp",isUp}, 
                                                 {"isNotUp",isNotUp},
                                                 {"isVelo",isVelo},
                                                 {"isNotVelo",isNotVelo}, 
                                                 {"isTT",isTT},
                                                 {"isNoTT",isNotTT},
                                                 {"isSeed",isSeed}, 
                                                 {"isNotSeed",isNotSeed}, 
                                                 {"strange",strange},
                                                 {"fromB",fromB},
                                                 {"fromD",fromD},
                                                 {"fromKsFromB",fromKsFromB},
                                                 {"isElectron",isElectron},
                                                 {"isNotElectron",isNotElectron},
                                                 {"BOrDMother",BOrDMother}};


  // convert strings to normal cuts ==> called m_otherCuts (without LoKi Hybrid factory)
  /** @class isTrack PrChecker2.h
  *  Predefined selection cuts: it converts strings to normal cuts, used by addOtherCuts
  */
  class isTrack {   
        
  public:
    isTrack(const int kind) { 
      m_kind = kind;
    };
    /// Functor that checks if the MCParticle fulfills certain criteria, e.g. reco'ble as long track, B daughter, ...
    bool operator()(LHCb::MCParticle* mcp, MCTrackInfo* mcInfo) const {
      bool motherB       = false;
      bool motherD       = false;
      if(m_kind == isLong) return mcInfo->hasVeloAndT( mcp );
      if(m_kind == isNotLong) return !mcInfo->hasVeloAndT( mcp );
      if(m_kind == isDown) return mcInfo->hasT( mcp ) &&  mcInfo->hasTT( mcp );
      if(m_kind == isNotDown) return !(mcInfo->hasT( mcp ) &&  mcInfo->hasTT( mcp ));
      if(m_kind == isUp) return mcInfo->hasVelo( mcp ) &&  mcInfo->hasTT( mcp );
      if(m_kind == isNotUp) return !(mcInfo->hasVelo( mcp ) &&  mcInfo->hasTT( mcp ));
      if(m_kind == isVelo) return mcInfo->hasVelo( mcp );
      if(m_kind == isNotVelo) return !mcInfo->hasVelo( mcp );
      if(m_kind == isSeed) return mcInfo->hasT( mcp );
      if(m_kind == isNotSeed) return !mcInfo->hasT( mcp );
      if(m_kind == isTT) return mcInfo->hasTT( mcp );
      if(m_kind == isNotTT) return !mcInfo->hasTT( mcp );
      if(m_kind == isElectron) return abs( mcp->particleID().pid() ) == 11;
      if(m_kind == isNotElectron) return abs( mcp->particleID().pid() ) != 11;
      
      if ( 0 != mcp->originVertex() ) {
        const LHCb::MCParticle* mother =  mcp->originVertex()->mother();
        if ( 0 != mother ) {
          if ( 0 != mother->originVertex() ) {
            double rOrigin = mother->originVertex()->position().rho();
            if ( fabs( rOrigin ) < 5. ) {
              int pid = abs( mother->particleID().pid() );
              // -- MCParticle is coming from a strang particle
              if ( 130 == pid ||    // K0L
                   310 == pid ||    // K0S
                   3122 == pid ||   // Lambda
                   3222 == pid ||   // Sigma+
                   3212 == pid ||   // Sigma0
                   3112 == pid ||   // Sigma-
                   3322 == pid ||   // Xsi0
                   3312 == pid ||   // Xsi-
                   3334 == pid      // Omega-
                   ) {
                if(m_kind == strange) return true;
                
              }
              // -- It's a Kshort from a b Hadron
              if ( 0 != mother->originVertex()->mother() ) {
                if  ( 310 == pid &&
                      2 == mcp->originVertex()->products().size()  &&
                      mother->originVertex()->mother()->particleID().hasBottom() &&
                      ( mother->originVertex()->mother()->particleID().isMeson() ||  
                        mother->originVertex()->mother()->particleID().isBaryon() )) {
                  if(m_kind == fromKsFromB) return true;
                }
              }
            }
          }
        }
        // -- It's a daughter of a B or D hadron
        while( 0 != mother ) {
          if ( mother->particleID().hasBottom() && 
               ( mother->particleID().isMeson() ||  mother->particleID().isBaryon() ) )   motherB = true;
          
          if ( mother->particleID().hasCharm() &&
               ( mother->particleID().isMeson() ||  mother->particleID().isBaryon() ) )  motherD = true;
          
          mother = mother->originVertex()->mother();
          
        }
        if( m_kind == fromD && motherD == true)  return m_kind == fromD;
        if( m_kind == fromB && motherB == true)  return m_kind == fromB;
        
        if(m_kind == BOrDMother && (motherD || motherB)  )  return m_kind == BOrDMother;
      }
      return false;
    }
    
  private:
    int m_kind;
  };


  /** @class addOtherCuts PrChecker2.h
   *  Class that adds selection cuts defined in isTrack to cuts
   */
  class addOtherCuts{
    
  public:
    void addCut(isTrack cat){ m_cuts.push_back(cat); }
    
    /// Functor that evaluates all 'isTrack' cuts
    bool operator()(LHCb::MCParticle* mcp, MCTrackInfo* mcInfo){
      
      bool decision = true;
      
      for(unsigned int i = 0; i < m_cuts.size(); ++i){
        decision = decision && m_cuts[i](mcp, mcInfo);
      }
      
      return decision;
    }
    
  private:
    std::vector<isTrack> m_cuts;
    
  };
  
  //maps for each track container with {cut name,selection cut}
  stringmap m_map_forward;
  stringmap m_map_velo;
  stringmap m_map_up;
  stringmap m_map_ttrack;
  stringmap m_map_down;
  stringmap m_map_ttforward;
  stringmap m_map_ttdown;
  stringmap m_map_new;
  stringmap m_map_ttnew;
  
  //default cuts for each container, if not other ones are specified in run python file, those are taken
  /** @brief Map filled  with default cuts for each container, first string is name of the cut, second one is cut (predefined from class isTrack or kinematical Loki Cuts)
   */
  stringmap DefaultCutMap( std::string name ){
    
    stringmap map_velo      = { {"01_velo" ,"isVelo "},//use numbers for right order of cuts
                                {"02_long","isLong "},
                                {"03_long>5GeV" ,"isLong & over5  " }, 
                                {"04_long_strange" , "isLong & strange " },
                                {"05_long_strange>5GeV","isLong & strange & over5 " },
                                {"06_long_fromB" ,"isLong & fromB " },
                                {"07_long_fromB>5GeV" , "isLong & fromB & over5 " } };
    stringmap map_forward   = { {"01_long","isLong "}, 
                                {"02_long>5GeV","isLong & over5 " }, 
                                {"03_long_strange","isLong & strange " },
                                {"04_long_strange>5GeV","isLong & strange & over5 " },
                                {"05_long_fromB","isLong & fromB " },
                                {"06_long_fromB>5GeV","isLong & fromB & over5 " } };
    stringmap map_up        = { {"01_velo" ,"isVelo "},
                                {"02_velo+UT" ,"isVelo & isTT "},
                                {"03_velo+UT>5GeV" , "isVelo & isTT & over5 "},
                                {"04_velo+notLong" ,"isNotLong & isVelo  "},
                                {"05_velo+UT+notLong" ,"isNotLong & isVelo & isTT "},
                                {"06_velo+UT+notLong>5GeV" ,"isNotLong & isVelo & isTT & over5 "},
                                {"07_long" ,"isLong "},
                                {"08_long>5GeV" ,"isLong & over5  "},
                                {"09_long_fromB" ,"isLong & fromB "},
                                {"10_long_fromB>5GeV" , "isLong & fromB & over5 "} };
    stringmap map_ttrack    = { {"01_hasT" ,"isSeed "},
                                {"02_long" ,"isLong "},
                                {"03_long>5GeV" ,"isLong & over5 "},
                                { "04_long_fromB" ,"isLong & fromB "},
                                {"05_long_fromB>5GeV" , "isLong & fromB & over5 "},
                                {"06_UT+T_strange" , " strange & isDown "},
                                {"07_UT+T_strange>5GeV" , " strange & isDown & over5"},
                                {"08_noVelo+UT+T_strange" , " strange & isDown & isNotVelo"},
                                {"09_noVelo+UT+T_strange>5GeV" , " strange & isDown & over5 & isNotVelo "},
                                {"10_UT+T_SfromDB" , " strange & isDown & ( fromB | fromD ) "},
                                {"11_UT+T_SfromDB>5GeV" , " strange & isDown & over5 & ( fromB | fromD )"},
                                {"12_noVelo+UT+T_SfromDB>5GeV" , " strange & isDown & isNotVelo & over5 & ( fromB | fromD ) "} };
    stringmap map_down      = { {"01_UT+T" ,"isDown "},
                                {"02_UT+T>5GeV" ,"isDown & over5"},
                                {"03_UT+T_strange" , " strange & isDown"},
                                {"04_UT+T_strange>5GeV" , " strange & isDown & over5 "},
                                {"05_noVelo+UT+T_strange" , " strange & isDown & isNotVelo"},
                                {"06_noVelo+UT+T_strange>5GeV" , " strange & isDown & over5 & isNotVelo "},
                                {"07_UT+T_fromB" , "isDown & fromB "},
                                {"08_UT+T_fromB>5GeV" , "isDown & fromB & over5 "},
                                {"09_noVelo+UT+T_fromB" , "isDown & fromB & isNotVelo"},
                                {"10_noVelo+UT+T_fromB>5GeV" , "isDown & fromB & over5 & isNotVelo"},
                                {"11_UT+T_SfromDB" , " strange & isDown & ( fromB | fromD ) "},
                                {"12_UT+T_SfromDB>5GeV" , " strange & isDown & over5 & ( fromB | fromD ) "},
                                {"13_noVelo+UT+T_SfromDB" , " strange & isDown & isNotVelo & ( fromB | fromD )"},
                                {"14_noVelo+UT+T_SfromDB>5GeV" , " strange & isDown & isNotVelo & over5 & ( fromB | fromD ) "} };
    
    stringmap map_new       = { };
    stringmap map_ttforward = { { "01_long" ,"isLong "},
                                {"02_long>5GeV" ,"isLong & over5  "} };
    stringmap map_ttdown    = { {"01_has seed" ,"isSeed "},
                                {"02_has seed +noVelo, T+TT" ,"isSeed & isNotVelo & isDown "},
                                {"03_down+strange" , " strange & isDown"},
                                {"04_down+strange+>5GeV" , " strange & isDown & over5 "}, 
                                {"05_pi<-Ks<-B" , "fromKsFromB "},
                                {"06_pi<-Ks<-B+> 5 GeV" , "fromKsFromB & over5 "} };
    stringmap map_ttnew     = { };
    stringmap empty_map     = { {},{} };

    if( name == "Forward"      ) return map_forward;
    if( name == "Velo"         ) return map_velo;
    if( name == "Upstream"     ) return map_up;
    if( name == "Ttrack"       ) return map_ttrack;
    if( name == "Downstream"   ) return map_down;
    if( name == "New"          ) return map_new;
    if( name == "TTForward"    ) return map_ttforward;
    if( name == "TTDownstream" ) return map_ttdown;
    if( name == "TTNew"        ) return map_ttnew;
   
    return empty_map;

  }
   
  /** @brief makes vector of second elements of DefaultCutMap --> needed as input for m_Cuts */
  strings getMyCut( stringmap myCutMap ){
    strings dummy;
    for( auto it : myCutMap ){
      dummy.push_back(it.second);
    }
    return dummy;
  }

  


  //== Vector of the counters
  std::vector<IPrCounter*>   m_allCounters;///<Vector of PrCounter
  std::vector<IPrTTCounter*> m_allTTCounters;///<Vector of PrTTCounter
  
  const IHistoTool*       m_histoTool;
  LoKi::IMCHybridFactory* m_factory;///<needed to convert normal cuts into Loki cuts

  // maps for cuts
  std::map< std::string, strings >                            m_Cuts;///<map of track container name and corresponding cuts
  
  std::map < std::string, std::vector < LoKi::Types::MCCut> > m_LoKiCuts;///<converted map of Loki cuts, first component is name of track container
  std::map < std::string, std::vector <addOtherCuts> >        m_otherCuts;///<map of other cuts as predefined in isTrack, first component is name of track container 
  
  std::vector<LoKi::Types::MCCut> m_veloMCCuts; 
  std::vector<addOtherCuts>       m_veloMCCuts2; 
  std::vector<LoKi::Types::MCCut> m_forwardMCCuts;
  std::vector<addOtherCuts>       m_forwardMCCuts2; 
  std::vector<LoKi::Types::MCCut> m_upstreamMCCuts; 
  std::vector<addOtherCuts>       m_upstreamMCCuts2;
  std::vector<LoKi::Types::MCCut> m_ttrackMCCuts;
  std::vector<addOtherCuts>       m_ttrackMCCuts2;
  std::vector<LoKi::Types::MCCut> m_downstreamMCCuts;
  std::vector<addOtherCuts>       m_downstreamMCCuts2;
  std::vector<LoKi::Types::MCCut> m_newMCCuts;
  std::vector<addOtherCuts>       m_newMCCuts2;
  std::vector<LoKi::Types::MCCut> m_ttforwardMCCuts;
  std::vector<addOtherCuts>       m_ttforwardMCCuts2;
  std::vector<LoKi::Types::MCCut> m_ttdownstreamMCCuts;
  std::vector<addOtherCuts>       m_ttdownstreamMCCuts2;
  std::vector<LoKi::Types::MCCut> m_ttnewMCCuts;
  std::vector<addOtherCuts>       m_ttnewMCCuts2;


};

#endif // PRCHECKER_H
