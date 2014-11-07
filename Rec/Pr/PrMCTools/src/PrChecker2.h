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
#include <tuple> 
//#include <unordered_map>

class IHistoTool ;


/** @class PrChecker2 PrChecker2.h
 *  Check the quality of the pattern recognition, by comparing to MC information
 *  Produces efficiency, ghost rate and clone rate numbers.
 *  Parameters:
 *   - Eta25Cut: Only consider particles with 2 < eta < 5? (default: false)
 *   - TriggerNumbers: Give numbers for p > 3GeV, pT > 500 MeV? (default: false)
 *   - UseElectrons: Take electrons into account in numbers? (default: false)
 * 
 *  @author Olivier Callot, Thomas Nikodem
 *  @date   2014-02-13
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

  std::string m_veloTracks;
  std::string m_forwardTracks;
  std::string m_matchTracks;
  std::string m_seedTracks;
  std::string m_downTracks;
  std::string m_upTracks;
  
  IPrCounter* m_velo;
  IPrCounter* m_forward;
  IPrCounter* m_match;
  IPrCounter* m_upTrack;
  IPrCounter* m_tTrack;
  IPrCounter* m_downTrack;
  IPrCounter* m_best;
  IPrCounter* m_bestLong;
  IPrCounter* m_bestDownstream;

  IPrTTCounter* m_ttForward;
  IPrTTCounter* m_ttMatch;
  IPrTTCounter* m_ttDownst;

  //IPrCounter* m_utForward;
  //IPrCounter* m_utMatch;
  //IPrCounter* m_utbestLong;
  //IPrCounter* m_utDownst;

  int  m_writetestHistos;  
  int  m_writeVeloHistos;      
  int  m_writeForwardHistos;   
  int  m_writeMatchHistos;     
  int  m_writeDownHistos;      
  int  m_writeUpHistos;        
  int  m_writeTTrackHistos;    
  int  m_writeBestHistos;
  int  m_writeBestLongHistos;
  int  m_writeBestDownstreamHistos;
  int  m_writeUTHistos;
  int  m_writeTTForwardHistos;
  int  m_writeTTMatchHistos;
  int  m_writeTTDownstHistos;
   

  bool m_eta25cut;             
  bool m_triggerNumbers;
  bool m_writeXYEffHistos;
  bool m_particlekeyforward;
  bool m_useElectrons;
 
 
  

  //convert strings to normal cuts ==> called m_otherCuts (without LoKi Hybrid factory)
  class isTrack {
  
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
      isNotElectron = 19
    };

    std::map< std::string, recAs> lookuptable = {{"isLong",isLong},
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
						 {"isNotElectron",isNotElectron}};

        
  public:
    isTrack(const std::string kind) {
      m_kind  = lookuptable[kind];//2ndarg=map[1st argument]
     
     };
    
    bool operator()(LHCb::MCParticle* mcp, MCTrackInfo mcInfo) const { 
      if(m_kind == isLong) return mcInfo.hasVeloAndT( mcp );
      if(m_kind == isNotLong) return !mcInfo.hasVeloAndT( mcp );
      if(m_kind == isDown) return mcInfo.hasT( mcp ) &&  mcInfo.hasTT( mcp );
      if(m_kind == isNotDown) return !mcInfo.hasT( mcp ) &&  !mcInfo.hasTT( mcp );
      if(m_kind == isUp) return mcInfo.hasVelo( mcp ) &&  mcInfo.hasTT( mcp );
      if(m_kind == isNotUp) return !mcInfo.hasVelo( mcp ) &&  !mcInfo.hasTT( mcp );
      if(m_kind == isVelo) return mcInfo.hasVelo( mcp );
      if(m_kind == isNotVelo) return !mcInfo.hasVelo( mcp );
      if(m_kind == isSeed) return mcInfo.hasT( mcp );
      if(m_kind == isNotSeed) return !mcInfo.hasT( mcp );
      if(m_kind == isTT) return mcInfo.hasTT( mcp );
      if(m_kind == isNotTT) return !mcInfo.hasTT( mcp );
      if(m_kind == isElectron) return abs( mcp->particleID().pid() ) == 11;
      if(m_kind == isNotElectron) return abs( mcp->particleID().pid() ) != 11;
      //if(m_kind == eta25) return true;
      
      if ( 0 != mcp->originVertex() ) {
	const LHCb::MCParticle* mother =  mcp->originVertex()->mother();
	if ( 0 != mother ) {
	  if ( 0 != mother->originVertex() ) {
	    double rOrigin = mother->originVertex()->position().rho();
	    if ( fabs( rOrigin ) < 5. ) {
	      int pid = abs( mother->particleID().pid() );
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
	while( 0 != mother ) {
	  if ( mother->particleID().hasBottom() && 
	       ( mother->particleID().isMeson() ||  mother->particleID().isBaryon() ) ) if(m_kind == fromB) return true;
	  
	  if ( mother->particleID().hasCharm() &&
	       ( mother->particleID().isMeson() ||  mother->particleID().isBaryon() ) ) if(m_kind == fromD) return true;
	  
	  mother = mother->originVertex()->mother();
	}
      }
      return false;
    }
    
  private:
    recAs m_kind;
  };



  class addOtherCuts{
  
  public:
    void addCut(isTrack cat){ m_cuts.push_back(cat); }
    
    bool operator()(LHCb::MCParticle* mcp, MCTrackInfo mcInfo){
      
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
  std::map <std::string,std::string> m_map_forward;
  std::map <std::string,std::string> m_map_velo;
  std::map <std::string,std::string> m_map_up;
  std::map <std::string,std::string> m_map_ttrack;
  std::map <std::string,std::string> m_map_down;
  std::map <std::string,std::string> m_map_ttforward;
  std::map <std::string,std::string> m_map_ttdown;
  std::map <std::string,std::string> m_map_new1;
  std::map <std::string,std::string> m_map_new2;

  //default cuts for each container, if not other ones are specified in run python file, those are taken
  std::map <std::string,std::string> fillMyMap( std::string name ){
    
    std::map <std::string,std::string> map_velo = { {"velo" ,"isVelo & isNotElectron"},
						    {"long","isLong & isNotElectron"},{"long>5GeV" ,"isLong & over5 & isNotElectron " }, {"long_strange" , "isLong & strange & isNotElectron" },{ "long_strange>5GeV","isLong & strange & over5 & isNotElectron" },{"long_fromB" ,"isLong & fromB & isNotElectron" },{"long_fromB>5GeV" , "isLong & fromB & over5 & isNotElectron" }};
    std::map <std::string,std::string> map_forward = {{"long","isLong & isNotElectron"}, {"long>5GeV","isLong & over5 & isNotElectron " }, {"long_strange","isLong & strange & isNotElectron" },{"long_strange>5GeV","isLong & strange & over5 & isNotElectron" },{"long_fromB","isLong & fromB & isNotElectron" },{"long_fromB>5GeV","isLong & fromB & over5 & isNotElectron" }};
    std::map <std::string,std::string> map_up = { {"velo" ,"isVelo & isNotElectron"},{"velo+UT" ,"isVelo & isTT & isNotElectron"},{"velo+UT>5GeV" , "isVelo & isTT & over5 & isNotElectron"},{"velo+notLong" ,"isNotLong & isVelo & isNotElectron "},{"velo+UT+notLong" ,"isNotLong & isVelo & isTT & isNotElectron"},{"velo+UT+notLong>5GeV" ,"isNotLong & isVelo & isTT & isNotElectron & over5 "},{"long" ,"isLong & isNotElectron"},{"long>5GeV" ,"isLong & over5 & isNotElectron "},{"long_fromB" ,"isLong & fromB & isNotElectron"},{"long_fromB>5GeV" , "isLong & fromB & over5 & isNotElectron"}};
    std::map <std::string,std::string> map_ttrack = {{"hasT" ,"isSeed & isNotElectron"},{"long" ,"isLong & isNotElectron"},{"long>5GeV" ,"isLong & over5 & isNotElectron "},{ "long_fromB" ,"isLong & fromB & isNotElectron"},{"long_fromB>5GeV" , "isLong & fromB & over5 & isNotElectron"},{"UT+T_strange" , " strange & isDown & isNotElectron"},{"UT+T_strange>5GeV" , " strange & isDown & over5 & isNotElectron"},{"noVelo+UT+T_strange" , " strange & isDown & isNotVelo & isNotElectron"},{"noVelo+UT+T_strange>5GeV" , " strange & isDown & over5 & isNotVelo & isNotElectron"},{"UT+T_SfromDB" , " strange & isDown & ( fromB | fromD ) & isNotElectron"},{"UT+T_SfromDB>5GeV" , " strange & isDown & over5 & ( fromB | fromD ) & isNotElectron"},{"noVelo+UT+T_SfromDB>5GeV" , " strange & isDown & isNotVelo & over5 & ( fromB | fromD ) & isNotElectron"}};
    std::map <std::string,std::string> map_down = {{"UT+T" ,"isDown & isNotElectron"},{"UT+T>5GeV" ,"isDown & over5 & isNotElectron"},{"UT+T_strange>5GeV" , " strange & isDown & over5 & isNotElectron"},{"UT+T_strange>5GeV" , " strange & isDown & over5 & isNotElectron"},{"noVelo+UT+T_strange" , " strange & isDown & isNotVelo & isNotElectron"},{"noVelo+UT+T_strange>5GeV" , " strange & isDown & over5 & isNotVelo & isNotElectron"},{"UT+T_fromB" , "isDown & fromB & isNotElectron"},{"UT+T_fromB>5GeV" , "isDown & fromB & over5 & isNotElectron"},{"noVelo+UT+T_fromB" , "isDown & fromB & isNotVelo & isNotElectron"},{"noVelo+UT+T_fromB>5GeV" , "isDown & fromB & over5 & isNotVelo & isNotElectron"},{"UT+T_SfromDB" , " strange & isDown & ( fromB | fromD ) & isNotElectron"},{"UT+T_SfromDB>5GeV" , " strange & isDown & over5 & ( fromB | fromD ) & isNotElectron"},{"noVelo+UT+T_SfromDB" , " strange & isDown & isNotVelo & ( fromB | fromD ) & isNotElectron"},{"noVelo+UT+T_SfromDB>5GeV" , " strange & isDown & isNotVelo & over5 & ( fromB | fromD ) & isNotElectron"}};
    std::map <std::string,std::string> map_ttforward = {{ "long" ,"isLong & isNotElectron"},{"long>5GeV" ,"isLong & over5 & isNotElectron "}};
    std::map <std::string,std::string> map_ttdown = {{"has seed" ,"isSeed & isNotElectron"},{"has seed +noVelo, T+TT" ,"isSeed & isNotVelo & isDown & isNotElectron"},{"down+strange" , " strange & isDown"},{"down+strange+>5GeV" , " strange & isDown & over5 "}, {"pi<-Ks<-B" , "fromKsFromB "},{"pi<-Ks<-B+> 5 GeV" , "fromKsFromB & over5 "}};
    std::map <std::string,std::string> empty_map = {{},{}};

    if( name == "Forward" ) return map_forward;
    if( name == "Velo" ) return map_velo;
    if( name == "Upstream" ) return map_up;
    if( name == "Ttrack" ) return map_ttrack;
    if( name == "Downstream" ) return map_down;
    if( name == "TTForward" ) return map_ttforward;
    if( name == "TTDownstream" ) return map_ttdown;
   
    return empty_map;

  }
   

  //make vector of second elements of maps --> needed as input for m_textcuts
  std::vector<std::string> getMyCut( std::map <std::string,std::string> myThingy ){
    std::vector<std::string> dummy;
    for( auto it : myThingy ){
      dummy.push_back(it.second);
    }
    return dummy;
  }

  
  //== Vector of the counters
  std::vector<IPrCounter*> m_allCounters;
  std::vector<IPrTTCounter*> m_allTTCounters;
  
  const IHistoTool* m_histoTool;
  LoKi::IMCHybridFactory* m_factory;
  //maps for cuts
  std::map< std::string, std::vector <std::string> >  m_textCuts;
  std::map < std::string, std::vector < LoKi::Types::MCCut> > m_LoKiCuts;
  std::map < std::string, std::vector <addOtherCuts> > m_otherCuts;




};

#endif // PRCHECKER_H
