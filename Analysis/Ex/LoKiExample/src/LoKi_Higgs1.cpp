// $Id: LoKi_Higgs1.cpp,v 1.4 2005-03-24 11:18:28 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ ; version $Revision: 1.4 $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/LoKi.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/L0DUReport.h"
#include "Event/L1Score.h"
#include "Event/TrgDecision.h"
// ============================================================================

namespace LoKi 
{
  namespace Particles 
  {
    /** @struct NumberOfDaughters 
     *  The trivial evaluator of number of daughter particles 
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-03-23 
     */
    struct NumberOfDaughters : public LoKi::Function<const Particle*> 
    {
    public:
      /// MANDATORY: clone method ("virtual constructor")
      virtual NumberOfDaughters* clone() const 
      { return new NumberOfDaughters( *this ) ; }
      /// the only one essential method 
      virtual result_type operator() ( argument p ) const 
      {
        if ( 0 == p ) { return 0 ; }
        const Vertex* v = p->endVertex() ;
        if ( 0 == v ) { return 0 ; }
        return v->products().size() ;
      };
    };

  }; // end of namespace Particles 
  
  namespace Cuts 
  {
    
    /** @var NDAU
     *  The trivial evaluator of number of daughter particles 
     *
     *  @code
     *
     *   const Particle* jet = ... ; 
     *   
     *   int nConstituents = (int) NDAUG( jet ) ;
     *
     *  @endcode 
     *
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-03-23 
     */
    const LoKi::Particles::NumberOfDaughters   NDAUG ;
    
  }; // end of namespace Cuts 

}; // end of namespace LoKi 


LOKI_ALGORITHM( LoKi_Higgs1 )
{
  using namespace LoKi          ;
  using namespace LoKi::Fits    ;
  using namespace LoKi::Cuts    ;
  using namespace LoKi::MCTypes ;

  // get the event header 
  const EventHeader* evtHdr = 
    get<EventHeader>( EventHeaderLocation::Default ) ;
  
  // trigger decisions 
  const L0DUReport*  L0  = get<L0DUReport>  ( L0DUReportLocation  ::Default ) ;
  const L1Score*     L1  = get<L1Score>     ( L1ScoreLocation     ::Default ) ;
  const TrgDecision* TRG = get<TrgDecision> ( TrgDecisionLocation ::Default ) ;

  MCMatch mc = mctruth() ;
  
  // # select all stable MC particles, produced near collision point
  MCCut   mccut = abs( MCVXFUN( MCVZ ) ) < (  40 *  cm )  ;
  MCRange mcall = mcselect ( "mcall" , mccut    && ( MCPZ > 0 ) && 
                             ( ( "e+"    == MCABSID ) ||
                               ( "mu+"   == MCABSID ) ||
                               ( "K+"    == MCABSID ) ||
                               ( "p+"    == MCABSID ) ||
                               ( "pi+"   == MCABSID ) ||
                               ( "gamma" == MCABSID ) ) ) ;
  
  // # book N-Tuple for MC 
  Tuple tup1 = nTuple( "MC-tuple" ) ;
  
  // W or Z ? 
  MCCut MCPRIM = MCMOTHCUT( !MCVALID , true ) ;
  
  tup1 << Tuples::Column ( "" , evtHdr         ) ;
  tup1 << Tuples::Column ( "" , L0             ) ;
  tup1 << Tuples::Column ( "" , L1             ) ;
  tup1 << Tuples::Column ( "" , TRG            ) ;
  
  tup1 -> farray ( "pid"  , MCID       ,
                   "pt"   , MCPT / GeV ,
                   "e"    , MCE  / GeV ,
                   mcall  .begin ()    , 
                   mcall  .end   ()    , 
                   "nMC"  , 5000       ) ;
  tup1 -> farray ( "eta"  , MCETA      ,
                   "phi"  , MCPHI      ,
                   "prim" , MCPRIM     ,
                   mcall  .begin ()    , 
                   mcall  .end   ()    , 
                   "nMC"  , 5000       ) ;
  
  tup1 -> write  () ;
  
  // # get only MC leptons 
  MCRange mcleps = mcselect ( "mclep" , 
                              mcall   ,  
                              ( "e+" == MCABSID ) || ( "mu+"   == MCABSID ) ) ;
  
  Cut mcCutLep = PNONE ;
  MCCut mcMu   = "mu+" == MCABSID  ;
  for ( MCRange::iterator imcl = mcleps.begin() ; mcleps.end() != imcl ; ++imcl ) 
  {
    const MCParticle* mcl = *imcl ;
    if ( 0 == mcl      ) { continue ; }
    const MCParticle* moth = mcl->mother() ;
    if ( 0 != moth     ) { continue ; }
    if ( !mcMu ( mcl ) ) { continue ; }
    // update mc cut
    mcCutLep = mcCutLep || MCTRUTH ( mc , mcl ) ;
  }
  
  // # select *ALL* recontructed particles 
  Range all = select ( "all" , PALL ) ;
  
  LoKi::Particles::PseudoRapidity ETA ;
  LoKi::Particles::Phi            PHI ;
  
  // # retrieve tuple for reconstructed energy flow 
  Tuple tup2 = nTuple( "RC-tuple" ) ;
  
  tup2 << Tuples::Column ( "" , evtHdr         ) ;
  tup2 << Tuples::Column ( "" , L0             ) ;
  tup2 << Tuples::Column ( "" , L1             ) ;
  tup2 << Tuples::Column ( "" , TRG            ) ;
  
  tup2 -> farray ( "pid" , ID        , 
                   "pt"  , PT / GeV  , 
                   "e"   , E  / GeV  ,
                   all   .begin ()   ,
                   all   .end   ()   ,
                   "nRC" , 500       ) ;
  
  tup2 -> farray ( "phi" , PHI       , 
                   "eta" , ETA       , 
                   "mcm" , mcCutLep  ,
                   "pidm", PIDmu     ,
                   all   .begin ()   ,
                   all   .end   ()   ,
                   "nRC" , 500       ) ;
  
  tup2 -> write() ;
  
  // # jets 
  Range jets = select( "jet" , "CELLjet" == ABSID && 5 <= NDAUG ) ;

  always () 
    << "# of jets " << jets.size() << endreq ;
  
  // # retrieve the tuple for reconstructed jets 
  Tuple tup3 = nTuple( "JET-tuple" ) ;
  
  for ( Loop JJ = loop ( "jet jet" , "CELLjet" , FitNone ) ; JJ ; ++JJ ) 
  {
    // tup3 << Tuples::Column ( "" , evtHdr         ) ;
    // tup3 << Tuples::Column ( "" , L0             ) ;
    // tup3 << Tuples::Column ( "" , L1             ) ;
    // tup3 << Tuples::Column ( "" , TRG            ) ;
    
    const Particle* jet1 = JJ(1) ;
    const Particle* jet2 = JJ(2) ;
    
    if ( 0 == jet1 || 0 == jet2 ) { continue ; }
    
    tup3 -> column ( "pt1"  , PT  ( jet1 )   / GeV ) ;
    tup3 -> column ( "pt2"  , PT  ( jet2 )   / GeV ) ;
    tup3 -> column ( "eta1" , ETA ( jet1 )         ) ;
    tup3 -> column ( "eta2" , ETA ( jet2 )         ) ;
    tup3 -> column ( "phi1" , PHI ( jet1 )         ) ;
    tup3 -> column ( "phi2" , PHI ( jet2 )         ) ;
    tup3 -> column ( "e1"   , E   ( jet1 )   / GeV ) ;
    tup3 -> column ( "e2"   , E   ( jet2 )   / GeV ) ;
    tup3 -> column ( "mjj"  , JJ->mass(1,2)  / GeV ) ;

    tup3 -> write() ;
    
  };
  
  
  return StatusCode::SUCCESS ;
};

// ============================================================================
// The END 
// ============================================================================
