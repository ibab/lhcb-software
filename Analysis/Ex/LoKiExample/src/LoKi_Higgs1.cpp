// $Id: LoKi_Higgs1.cpp,v 1.3 2005-03-05 14:06:45 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ ; version $Revision: 1.3 $
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
  namespace MCParticles
  {
    /** @struct PseudoRapidity
     *  evaluator of the pseudirapidity 
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr 
     *  @date   2005-03-04
     */
    struct PseudoRapidity : public LoKi::Function<const MCParticle*>
    {      
      /// clone method (mandatory!)
      virtual PseudoRapidity* clone() const 
      { return new PseudoRapidity(*this) ; };      
      /// the only one essential method 
      result_type operator() ( argument p ) const 
      {
        if ( 0 == p ) 
        {
          Warning( "MCParticle* points to NULL!" ) ;
          return -1000 ;
        }
        return p->momentum().pseudoRapidity() ;
      };
    };
    /** @struct Phi
     *  trivial evaluator of Phi
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr 
     *  @date   2005-03-04
     */
    struct Phi : public LoKi::Function<const MCParticle*>
    {      
      /// clone method (mandatory!)
      virtual Phi* clone() const { return new Phi(*this) ; };      
      /// the only one essential method 
      result_type operator() ( argument p ) const 
      {
        if ( 0 == p ) 
        {
          Warning( "MCParticle* points to NULL!" ) ;
          return -1000 ;
        }
        return p->momentum().phi() ;
      };
    };
    
  }; // end of namespace MCParticles 
  
  namespace Particles
  {
    /** @struct PseudoRapidity
     *  evaluator of the pseudirapidity 
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr 
     *  @date   2005-03-04
     */
    struct PseudoRapidity : public LoKi::Function<const Particle*>
    {      
      /// clone method (mandatory!)
      virtual PseudoRapidity* clone() const 
      { return new PseudoRapidity(*this) ; };      
      /// the only one essential method 
      result_type operator() ( argument p ) const 
      {
        if ( 0 == p ) 
        {
          Warning( "Particle* points to NULL!" ) ;
          return -1000 ;
        }
        return p->momentum().pseudoRapidity() ;
      };
    };
    /** @struct Phi
     *  trivial evaluator of Phi
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr 
     *  @date   2005-03-04
     */
    struct Phi : public LoKi::Function<const Particle*>
    {      
      /// clone method (mandatory!)
      virtual Phi* clone() const { return new Phi(*this) ; };      
      /// the only one essential method 
      result_type operator() ( argument p ) const 
      {
        if ( 0 == p ) 
        {
          Warning( "Particle* points to NULL!" ) ;
          return -1000 ;
        }
        return p->momentum().phi() ;
      };
    };
    
  }; // end of namespace Particles 
  
}; //end of namespace LoKi
  

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
  
  LoKi::MCParticles::PseudoRapidity MCETA ;
  LoKi::MCParticles::Phi            MCPHI ;
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

  return StatusCode::SUCCESS ;
};

// ============================================================================
// The END 
// ============================================================================
