// $Id: LoKi_Higgs1.cpp,v 1.5 2005-03-28 12:45:08 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ ; version $Revision: 1.5 $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/LoKi.h"
#include "LoKi/LoKiGen.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/L0DUReport.h"
#include "Event/L1Score.h"
#include "Event/TrgDecision.h"
#include "Event/HepMCEvent.h"
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
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const 
      {
        if ( 0 == p ) { return 0 ; }
        const Vertex* v = p->endVertex() ;
        if ( 0 == v ) { return 0 ; }
        return v->products().size() ;
      };
    };
    
    /** @class MomentumDistance 
     *  Trivial evaluator of euclidian distance 
     *  of 4-momentums useful e.g. for trivial 
     *  "kinematical matching". 
     *  One can find 
     *   the particle with "closest" 4-momentum 
     *   to the given one:
     *
     *  @code 
     *
     *  SEQUENCE            particles = ... ;
     *  LoKi::LorentzVector vct       = ... ; 
     *
     *  // find "best match" 
     *  SEQUENCE::const_iterator imin = 
     *    LoKi::select_min( particles.begin () , 
     *                      particles.end   () , 
     *                      MOMDIST ( vct )    ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Cuts::MOMDIST
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-03-23 
     */
    class MomentumDistance : 
      public LoKi::Function<const Particle*> 
    {
    public:
      MomentumDistance( const LoKi::LorentzVector& vct ) 
        : LoKi::Function<const Particle*>() 
        , m_vct ( vct ) 
      {};
      /// virtual destructor 
      virtual ~MomentumDistance() {};
      /// MANDATORY: clone function ("virtual constructor")
      virtual MomentumDistance* clone() const 
      { return new MomentumDistance ( *this ) ;}
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const
      {
        if ( 0 == p ) 
        { 
          Error("Invalid Particle, return (100TeV)**2") ; 
          return 100 * TeV * 100 * TeV ; 
        };
        return m_vct.delta2Euclidean( p->momentum() ) ;
      };
    private:
      /// default constructor is private
      MomentumDistance() ;
    private:
      LoKi::LorentzVector m_vct ;
    };

    /** @class TransverseMomentumRel
     *  Trivial evaluator of the transverse momenrum, 
     *  relatve to the given direction 
     *
     *  @code 
     *
     *  const Particle*   particle = ... ;
     *  LoKi::ThreeVector vct  = ... ; 
     *
     *  Fun pt = PTDIR( vct ) ;
     *  const double pt = pt ( particle ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Cuts::PTDIR
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-03-23 
     */
    class TransverseMomentumRel :
      public LoKi::Function<const Particle*> 
    {
    public:
      TransverseMomentumRel 
      ( const LoKi::ThreeVector& vct ) 
        : LoKi::Function<const Particle*>() 
        , m_vct ( vct ) 
      {};
      /// virtual destructor 
      virtual ~TransverseMomentumRel() {};
      /// MANDATORY: clone function ("virtual constructor")
      virtual TransverseMomentumRel* clone() const 
      { return new TransverseMomentumRel( *this ) ;}
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const
      {
        if ( 0 == p ) 
        { 
          Error("Invalid Particle, return (100TeV)**2") ; 
          return 100 * TeV * 100 * TeV ; 
        };
        return p->momentum().perp( m_vct ) ;
      };
    private:
      /// default constructor is private
      TransverseMomentumRel() ;
    private:
      LoKi::ThreeVector m_vct ;
    };
    
  }; // end of namespace Particles 

  namespace MCParticles 
  {
    /** @class MomentumDistance 
     *  Trivial evaluator of euclidian distance 
     *  of 4-momentums useful e.g. for trivial 
     *  "kinematical matching". 
     *  One can find 
     *   the particle with "closest" 4-momentum 
     *   to the given one:
     *
     *  @code 
     *
     *  SEQUENCE            mcparticles = ... ;
     *  LoKi::LorentzVector vct       = ... ; 
     *
     *  // find "best match" 
     *  SEQUENCE::const_iterator imin = 
     *    LoKi::select_min( mcparticles.begin () , 
     *                      mcparticles.end   () , 
     *                      MCMOMDIST ( vct )    ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Cuts::MCMOMDIST
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-03-23 
     */
    class MomentumDistance : 
      public LoKi::Function<const MCParticle*> 
    {
    public:
      MomentumDistance( const LoKi::LorentzVector& vct ) 
        : LoKi::Function<const MCParticle*>() 
        , m_vct ( vct ) 
      {};
      /// virtual destructor 
      virtual ~MomentumDistance() {};
      /// MANDATORY: clone function ("virtual constructor")
      virtual MomentumDistance* clone() const 
      { return new MomentumDistance ( *this ) ;}
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const
      {
        if ( 0 == p ) 
        { 
          Error("Invalid MCParticle, return (100TeV)**2") ; 
          return 100 * TeV * 100 * TeV ; 
        };
        return m_vct.delta2Euclidean( p->momentum() ) ;
      };
    private:
      /// default constructor is private
      MomentumDistance() ;
    private:
      LoKi::LorentzVector m_vct ;
    };
    
    /** @class TransverseMomentumRel
     *  Trivial evaluator of the transverse momenrum, 
     *  relatve to the given direction 
     *
     *  @code 
     *
     *  const MCParticle*  mcparticle = ... ;
     *  LoKi::ThreeVector  vct  = ... ; 
     *
     *  Fun pt = MCPTDIR( vct ) ;
     *  const double pt = pt ( mcparticle ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Cuts::MCPTDIR
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-03-23 
     */
    class TransverseMomentumRel :
      public LoKi::Function<const MCParticle*> 
    {
    public:
      TransverseMomentumRel 
      ( const LoKi::ThreeVector& vct ) 
        : LoKi::Function<const MCParticle*>() 
        , m_vct ( vct ) 
      {};
      /// virtual destructor 
      virtual ~TransverseMomentumRel() {};
      /// MANDATORY: clone function ("virtual constructor")
      virtual TransverseMomentumRel* clone() const 
      { return new TransverseMomentumRel( *this ) ;}
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const
      {
        if ( 0 == p ) 
        { 
          Error("Invalid Particle, return (100TeV)**2") ; 
          return 100 * TeV * 100 * TeV ; 
        };
        return p->momentum().perp( m_vct ) ;
      };
    private:
      /// default constructor is private
      TransverseMomentumRel() ;
    private:
      LoKi::ThreeVector m_vct ;
    };
    
  }; // end of namespace MCParticles 
  
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
    const LoKi::Particles::NumberOfDaughters     NDAUG    ;
    
    /** the evaluator of euclidian distance with 
     *  respect to the given 4-momentum, useful 
     *  e.g. for trivial "kinematical matching". 
     *  One can find 
     *  the particle with "closest" 4-momentum 
     *  to the given one:
     *
     *  @code 
     *
     *  SEQUENCE            particles = ... ;
     *  LoKi::LorentzVector vct       = ... ; 
     *
     *  // find "best match" 
     *  SEQUENCE::const_iterator imin = 
     *    LoKi::select_min( particles.begin () , 
     *                      particles.end   () , 
     *                      MOMDIST ( vct )    ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Particles::MomentumDistance 
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-03-23 
     */
    typedef LoKi::Particles::MomentumDistance      MOMDIST   ;

    /** Trivial evaluator of the transverse momentum, 
     *  relatve to the given direction 
     *
     *  @code 
     *
     *  const Particle*   particle = ... ;
     *  LoKi::ThreeVector vct  = ... ; 
     *
     *  Fun pt = PTDIR( vct ) ;
     *  const double pt = pt ( particle ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Particles::TransverseMomentumRel
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-03-23 
     */
    typedef LoKi::Particles::TransverseMomentumRel PTDIR     ;
    
    /** the evaluator of euclidian distance with 
     *  respect to the given 4-momentum, useful 
     *  e.g. for trivial "kinematical matching". 
     *  One can find 
     *  the particle with "closest" 4-momentum 
     *  to the given one:
     *
     *  @code 
     *
     *  SEQUENCE            mcparticles = ... ;
     *  LoKi::LorentzVector vct         = ... ; 
     *
     *  // find "best match" 
     *  SEQUENCE::const_iterator imin = 
     *    LoKi::select_min( mcparticles.begin () , 
     *                      mcparticles.end   () , 
     *                      MCMOMDIST ( vct )    ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::MCParticles::MomentumDistance 
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-03-23 
     */
    typedef LoKi::MCParticles::MomentumDistance  MCMOMDIST ;

    /** Trivial evaluator of the transverse momenrum, 
     *  relatve to the given direction 
     *
     *  @code 
     *
     *  const MCParticle* mcparticle = ... ;
     *  LoKi::ThereVector vct        = ... ; 
     *
     *  Fun pt = MCPTDIR( vct ) ;
     *  const double pt = pt ( mcparticle ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::MCParticles::TransverseMomentumRel
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-03-23 
     */
    typedef LoKi::MCParticles::TransverseMomentumRel MCPTDIR     ;

    
  }; // end of namespace Cuts

  
}; // end of namespace LoKi 



namespace LoKi
{
  namespace Cuts
  {
    typedef LoKi::MCTypes::MCCut MCCut ;
    typedef LoKi::MCTypes::MCFun MCFun ;
  };
};


LOKI_ALGORITHM( LoKi_Higgs1 )
{
  using namespace LoKi          ;
  using namespace LoKi::Fits    ;
  using namespace LoKi::Cuts    ;

  // get the event header 
  const EventHeader* evtHdr = 
    get<EventHeader>( EventHeaderLocation::Default ) ;
  
  if ( evtHdr->evtNum() > 306 && 
       evtHdr->evtNum() < 312   ) 
  {
    Warning("skip event!");
    return StatusCode::SUCCESS ;
  };
  
    
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
  
  // event header 
  tup1 << Tuples::Column ( "" , evtHdr         ) ;
  // L0  information
  tup1 << Tuples::Column ( "" , L0             ) ;
  // L1  information
  tup1 << Tuples::Column ( "" , L1             ) ;
  // TRG information
  tup1 << Tuples::Column ( "" , TRG            ) ;
  
  tup1 -> farray ( "pid"  , MCID       ,  // particle ID 
                   "pt"   , MCPT / GeV ,  // transverse momentum 
                   "e"    , MCE  / GeV ,  // energy 
                   mcall  .begin ()    , 
                   mcall  .end   ()    , 
                   "nMC"  , 5000       ) ;
  tup1 -> farray ( "eta"  , MCETA      ,  // pseudorapidity 
                   "phi"  , MCPHI      ,  // phi 
                   "prim" , MCPRIM     ,  // mother is invalid ? 
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
  
  // get Generator information
  const HepMCEvents* events = 
    get<HepMCEvents>( HepMCEventLocation::Default ) ;
  
  typedef std::vector<const HepMC::GenParticle*> GenParticles ;
  
  // select b(and antib) quarks from decay of higgs 
  GenParticles bquarks ;
  LoKi::Extract::genParticles
    ( events                         , 
      std::back_inserter( bquarks )  , 
      ( "b" == GABSID ) && 
      1 == GNINTREE( "H_10" == GABSID , HepMC::parents ) ) ;
  
  // always() << " number of b-quarks "  << bquarks.size() << endreq ;
  
  if ( 2 != bquarks.size() ) 
  {
    Warning("illegal number of b-quarks!");
    return StatusCode::SUCCESS ;
  };
  
  // # select *ALL* recontructed particles 
  Range all = select ( "all" , PALL ) ;
  
  // # retrieve tuple for reconstructed energy flow 
  Tuple tup2 = nTuple( "RC-tuple" ) ;
  
  // event header information
  tup2 << Tuples::Column ( "" , evtHdr         ) ;
  // L0  information
  tup2 << Tuples::Column ( "" , L0             ) ;
  // L2  information
  tup2 << Tuples::Column ( "" , L1             ) ;
  // TRG information
  tup2 << Tuples::Column ( "" , TRG            ) ;
  
  tup2 -> farray ( "pid" , ID        ,  // particle ID 
                   "pt"  , PT / GeV  ,  // transverse momentum 
                   "e"   , E  / GeV  ,  // energy  
                   all   .begin ()   ,
                   all   .end   ()   ,
                   "nRC" , 500       ) ;
  
  tup2 -> farray ( "phi" , PHI       ,  // phi 
                   "eta" , ETA       ,  // pseudoRapidity
                   "mcm" , mcCutLep  ,  // flag for MC muon from W  
                   "pidm", PIDmu     ,  // DLL(mu-pi)
                   all   .begin ()   ,
                   all   .end   ()   ,
                   "nRC" , 500       ) ;
  
  tup2 -> write() ;

  VRange primaries = vselect( "PVs" , Vertex::Primary == VTYPE ) ;
  Fun mips = MIPCHI2( primaries , geo() ) ;

  // basic particle 
  Cut  c0 = HASORIGIN               ;    
  // basic charged particle 
  Cut  cc = HASORIGIN && ( 0 != Q ) ;    
  // basic charged particle with chi2_mips >  4 
  Cut  c4 = HASORIGIN && ( 0 != Q ) && ( mips >  4 ) ; 
  // basic charged particle with chi2_mips >  9 
  Cut  c9 = HASORIGIN && ( 0 != Q ) && ( mips >  9 ) ;
  // basic charged particle with chi2_mips > 16 
  Cut c16 = HASORIGIN && ( 0 != Q ) && ( mips > 16 ) ;
  // basic charged particle with chi2_mips > 25 
  Cut c25 = HASORIGIN && ( 0 != Q ) && ( mips > 25 ) ;
  // basic charged particle with chi2_mips > 49 
  Cut c49 = HASORIGIN && ( 0 != Q ) && ( mips > 49 ) ;
  
  // number of basic particles in the jet 
  Fun  n0 = NINTREE (  c0 ) ; 
  // number of basic charged particles in the jet 
  Fun  nc = NINTREE (  cc ) ; 
  // number of basic charged particles with chi2_mips >  4 in the jet
  Fun  n4 = NINTREE (  c4 ) ;
  // number of basic charged particles with chi2_mips >  9 in the jet
  Fun  n9 = NINTREE (  c9 ) ;
  // number of basic charged particles with chi2_mips > 16 in the jet
  Fun n16 = NINTREE ( c16 ) ;
  // number of basic charged particles with chi2_mips > 25 in the jet
  Fun n25 = NINTREE ( c25 ) ; 
  // number of basic charged particles with chi2_mips > 49 in the jet
  Fun n49 = NINTREE ( c49 ) ;

  // sum energy of all basic charged particle in the jet 
  Fun  ec = TREESUM ( E  ,  cc ) / GeV ;
  // sum energy of all basic charged particle with chi2_mips >  4 in the jet 
  Fun  e4 = TREESUM ( E  ,  c4 ) / GeV ;
  // sum energy of all basic charged particle with chi2_mips >  9 in the jet 
  Fun  e9 = TREESUM ( E  ,  c9 ) / GeV ;
  // sum energy of all basic charged particle with chi2_mips > 16 in the jet 
  Fun e16 = TREESUM ( E  , c16 ) / GeV ;
  // sum energy of all basic charged particle with chi2_mips > 25 in the jet 
  Fun e25 = TREESUM ( E  , c25 ) / GeV ;
  // sum energy of all basic charged particle with chi2_mips > 49 in the jet 
  Fun e49 = TREESUM ( E  , c49 ) / GeV ;

  // # jets 
  Range jets = select( "jet" , "CELLjet" == ABSID && 5 <= NDAUG ) ;
  
  Tuple tup4 = nTuple( "JET1-tuple" ) ;  
  for ( Range::iterator ij = jets.begin() ; jets.end() != ij ; ++ij ) 
  {
    const Particle* jet = *ij ;
    if ( 0 == jet ) { continue ; }
    
    // find "closest" b/anti-b quark 
    GenParticles::const_iterator ij = 
      LoKi::select_min( bquarks.begin () , 
                        bquarks.end   () , 
                        GMOMDIST ( jet->momentum() / GeV ) ) ;
    
    if ( bquarks.end() == ij ) { continue ; }
    
    const HepMC::GenParticle* b = *ij ;
    
    // transverse momentum of the reconstructed jet 
    tup4->column ( "pt"   , PT( jet )           / GeV ) ;

    // 4-momentum of the reconstructed jet 
    tup4->column ( "j"    , jet   -> momentum() / GeV ) ;

    // 4-momentum of the closest b/anti-b quark
    tup4->column ( "b"    , b     -> momentum()       ) ;

    // pseudorapidity of reconstructed jet
    tup4->column ( "jeta" , ETA  ( jet ) ) ;
    // phi of reconstructed jet 
    tup4->column ( "jphi" , PHI  ( jet ) ) ;
    
    // pseudorapidity of closest b/anti-b quark
    tup4->column ( "beta" , GETA ( b   ) ) ;
    // phi of closest b/anti-b quark
    tup4->column ( "bphi" , GPHI ( b   ) ) ;
    
    // number of basic particles in the jet 
    tup4->column (  "n0"  ,  n0  ( jet ) ) ;
    // number of basic charged particles in the jet
    tup4->column (  "nc"  ,  nc  ( jet ) ) ;
    // number of basic charged particles with chi2_mips >  4 in the jet
    tup4->column (  "n4"  ,  n4  ( jet ) ) ;
    // number of basic charged particles with chi2_mips >  9 in the jet
    tup4->column (  "n9"  ,  n9  ( jet ) ) ;
    // number of basic charged particles with chi2_mips > 16 in the jet
    tup4->column ( "n16"  , n16  ( jet ) ) ;
    // number of basic charged particles with chi2_mips > 25 in the jet
    tup4->column ( "n25"  , n25  ( jet ) ) ;
    // number of basic charged particles with chi2_mips > 49 in the jet
    tup4->column ( "n49"  , n49  ( jet ) ) ;
    
    // sum energy of all basic charged particle in the jet 
    tup4->column (  "ec"  ,  ec  ( jet ) ) ;
    // sum energy of all basic charged particle with chi2_mips >  4 in the jet 
    tup4->column (  "e4"  ,  e4  ( jet ) ) ;
    // sum energy of all basic charged particle with chi2_mips >  9 in the jet 
    tup4->column (  "e9"  ,  e9  ( jet ) ) ;
    // sum energy of all basic charged particle with chi2_mips > 16 in the jet 
    tup4->column ( "e16"  , e16  ( jet ) ) ;
    // sum energy of all basic charged particle with chi2_mips > 25 in the jet 
    tup4->column ( "e25"  , e25  ( jet ) ) ; 
    // sum energy of all basic charged particle with chi2_mips > 49 in the jet 
    tup4->column ( "e49"  , e49  ( jet ) ) ;
    
    tup4->write() ;
    
  };
  
  // # retrieve the tuple for reconstructed jets 
  Tuple tup3 = nTuple( "JET-tuple" ) ;
  
  for ( Loop JJ = loop ( "jet jet" , "CELLjet" , FitNone ) ; JJ ; ++JJ ) 
  {
    
    const Particle* jet1 = JJ(1) ;
    const Particle* jet2 = JJ(2) ;
    
    if ( 0 == jet1 || 0 == jet2 ) { continue ; }
    
    // transverse momentum of the first reconstructed jet 
    tup3 -> column ( "pt1"  , PT  ( jet1 )   / GeV ) ;
    // transverse momentum of the second reconstructed jet 
    tup3 -> column ( "pt2"  , PT  ( jet2 )   / GeV ) ;
    
    // pseudorapidity  of the first reconstructed jet
    tup3 -> column ( "j1eta" , ETA ( jet1 )         ) ;
    // pseudorapidity  of the second reconstructed jet
    tup3 -> column ( "j2eta" , ETA ( jet2 )         ) ;

    // phi of the first reconstructed jet
    tup3 -> column ( "j1phi" , PHI ( jet1 )         ) ;
    // phi of the second reconstructed jet
    tup3 -> column ( "j2phi" , PHI ( jet2 )         ) ;

    // energy of the first reconstructed jet
    tup3 -> column ( "e1"   , E   ( jet1 )   / GeV ) ;
    // energy of the second reconstructed jet
    tup3 -> column ( "e2"   , E   ( jet2 )   / GeV ) ;
    
    // di-jet invarinat mass 
    tup3 -> column ( "mjj"  , JJ->mass(1,2)  / GeV ) ;
    
    // number of basic particles in the jet 
    tup3 -> column ( "j1n0"  , n0  ( jet1 ) ) ;
    // number of basic charged particles in the jet
    tup3 -> column ( "j1nc"  , nc  ( jet1 ) ) ;
    // number of basic charged particles with chi2_mips >  4 in the jet
    tup3 -> column ( "j1n4"  , n4  ( jet1 ) ) ;
    // number of basic charged particles with chi2_mips >  9 in the jet
    tup3 -> column ( "j1n9"  , n9  ( jet1 ) ) ;
    // number of basic charged particles with chi2_mips > 26 in the jet
    tup3 -> column ( "j1n16" , n16 ( jet1 ) ) ;
    // number of basic charged particles with chi2_mips > 25 in the jet
    tup3 -> column ( "j1n25" , n25 ( jet1 ) ) ;
    // number of basic charged particles with chi2_mips > 49 in the jet
    tup3 -> column ( "j1n49" , n49 ( jet1 ) ) ;
    
    // number of basic particles in the jet 
    tup3 -> column ( "j2n0"  , n0  ( jet2 ) ) ;
    // number of basic charged particles in the jet
    tup3 -> column ( "j2nc"  , nc  ( jet2 ) ) ;
    // number of basic charged particles with chi2_mips >  4 in the jet
    tup3 -> column ( "j2n4"  , n4  ( jet2 ) ) ;
    // number of basic charged particles with chi2_mips >  9 in the jet
    tup3 -> column ( "j2n9"  , n9  ( jet2 ) ) ;
    // number of basic charged particles with chi2_mips > 16 in the jet
    tup3 -> column ( "j2n16" , n16 ( jet2 ) ) ;
    // number of basic charged particles with chi2_mips > 25 in the jet
    tup3 -> column ( "j2n25" , n25 ( jet2 ) ) ;
    // number of basic charged particles with chi2_mips > 49 in the jet
    tup3 -> column ( "j2n49" , n49 ( jet2 ) ) ;

    // sum energy of all basic charged particle in the jet 
    tup3 -> column (  "j1ec"  ,  ec  ( jet1 ) ) ;
    // sum energy of all basic charged particle with chi2_mips >  4 in the jet 
    tup3 -> column (  "j1e4"  ,  e4  ( jet1 ) ) ;
    // sum energy of all basic charged particle with chi2_mips >  9 in the jet 
    tup3 -> column (  "j1e9"  ,  e9  ( jet1 ) ) ;
    // sum energy of all basic charged particle with chi2_mips > 16 in the jet 
    tup3 -> column ( "j1e16"  , e16  ( jet1 ) ) ;
    // sum energy of all basic charged particle with chi2_mips > 25 in the jet 
    tup3 -> column ( "j1e25"  , e25  ( jet1 ) ) ; 
    // sum energy of all basic charged particle with chi2_mips > 49 in the jet 
    tup3 -> column ( "j1e49"  , e49  ( jet1 ) ) ;

    // sum energy of all basic charged particle in the jet 
    tup3 -> column (  "j2ec"  ,  ec  ( jet2 ) ) ;
    // sum energy of all basic charged particle with chi2_mips >  4 in the jet 
    tup3 -> column (  "j2e4"  ,  e4  ( jet2 ) ) ;
    // sum energy of all basic charged particle with chi2_mips >  9 in the jet 
    tup3 -> column (  "j2e9"  ,  e9  ( jet2 ) ) ;
    // sum energy of all basic charged particle with chi2_mips > 16 in the jet 
    tup3 -> column ( "j2e16"  , e16  ( jet2 ) ) ;
    // sum energy of all basic charged particle with chi2_mips > 25 in the jet 
    tup3 -> column ( "j2e25"  , e25  ( jet2 ) ) ; 
    // sum energy of all basic charged particle with chi2_mips > 49 in the jet 
    tup3 -> column ( "j2e49"  , e49  ( jet2 ) ) ;
    
    // find the closest b/anti-b quark to the first jet
    GenParticles::const_iterator i1 = 
      LoKi::select_min( bquarks.begin () , 
                        bquarks.end   () , 
                        GMOMDIST ( jet1->momentum() / GeV ) ) ;
    if ( bquarks.end() == i1 ) { continue ; }
    const HepMC::GenParticle* b1 = *i1 ; 
    // find the closest b/anti-b quark to the second jet
    GenParticles::const_iterator i2 = 
      LoKi::select_min( bquarks.begin () , 
                        bquarks.end   () , 
                        GMOMDIST ( jet2->momentum() / GeV ) ) ;
    if ( bquarks.end() == i2 ) { continue ; }
    const HepMC::GenParticle* b2 = *i2 ;
    
    // the 4-momentum of the first jet 
    tup3 -> column ( "j1"   ,  jet1->momentum() / GeV  )  ;
    // the 4-momentum of the first jet 
    tup3 -> column ( "j2"   ,  jet2->momentum() / GeV  )  ;
    
    // the 4-momentum of the first b/anti-b quark 
    tup3 -> column ( "b1"   ,  b1->momentum()   )  ;
    // the 4-momentum of the second b/anti-b quark 
    tup3 -> column ( "b2"   ,  b2->momentum()   )  ;
    
    // pseudorapidity of the first b/anti-b quark
    tup3 -> column ( "b1eta" , GETA ( b1 ) ) ;
    // phi of the first b/anti-b quark  
    tup3 -> column ( "b1phi" , GPHI ( b1 ) ) ;
    
    // pseudorapidity of the second b/anti-b quark 
    tup3 -> column ( "b2eta" , GETA ( b2 ) ) ;
    // phi of the second b/anti-b quark
    tup3 -> column ( "b2phi" , GPHI ( b2 ) ) ;
    
    tup3 -> write() ;
    
  };

  return StatusCode::SUCCESS ;
};

// ============================================================================
// The END 
// ============================================================================
