// $Id: LoKiJetMakerAlg.cpp,v 1.6 2010-01-11 08:37:42 cocov Exp $
// ============================================================================
// Include files 
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
// ============================================================================
// LoKiAlgo
// ============================================================================
#include "LoKi/Algo.h"
#include "LoKi/ParticleCuts.h"
#include "LoKi/VertexCuts.h"
#include "LoKi/ParticleContextCuts.h"
// ============================================================================
// DaVinci Kernel 
// ============================================================================
#include "Kernel/IParticleCombiner.h"
#include "Kernel/IJetMaker.h"
#include "Kernel/Particle2Vertex.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @class JetMaker
   *  
   *  This file is a part of LoKi project - 
   *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
   *
   *  The package has been designed with the kind help from
   *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
   *  contributions and advices from G.Raven, J.van Tilburg, 
   *  A.Golutvin, P.Koppenburg have been used in the design.
   *
   *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
   *  @date   2005-03-21
   */
  class JetMaker : public LoKi::Algo  
  {
    // ========================================================================
    /// the friend factory for instantiation 
    friend class AlgFactory<LoKi::JetMaker> ;
    // ========================================================================
  protected:  
    // ========================================================================    
    /** Standard constructor
     *  @param name instance name 
     *  @param pSvc pointer to Service Locator 
     */
    JetMaker
    ( const std::string& name ,
      ISvcLocator*       pSvc ) 
      : LoKi::Algo ( name , pSvc )
      // 
      , m_makerName ( "LoKi::FastJetMaker"   )
      , m_maker     ( 0   )
      , m_associate2Vertex ( false  )
    { 
      // 
      declareProperty 
        ( "JetMaker"  , 
          m_makerName , 
          "Type type/name of jet-maker tool (IJetMaker interface)") ;  
      declareProperty 
        ( "Associate2Vertex"  , 
          m_associate2Vertex , 
          "Jet reconstruction per vertex") ;  
      //
    }
    /// destructor
    virtual ~JetMaker( ){}
    // ========================================================================    
  public:
    // ========================================================================    
    /** standard execution of the algorithm 
     *  @see LoKi::Algo 
     *  @return status code 
     */
    virtual StatusCode analyse   () ;
    // ========================================================================    
  private:
    // ========================================================================    
    // the default constructor is disabled 
    JetMaker () ;
    // the copy constructor is disabled 
    JetMaker ( const JetMaker& )  ;
    // the assignement operator is disabled 
    JetMaker& operator=( const  JetMaker& )  ;
    // ========================================================================    
  private:  
    // ========================================================================    
    /// maker name
    std::string      m_makerName ; // jet maker name  
    /// maker
    const IJetMaker* m_maker     ; // jet maker to be used 
    /// associate two vertex?
    bool m_associate2Vertex      ; // make jet per vertex
    
    // ========================================================================    
  };
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
/** @file 
 *  Implementation file for class  LoKi::JetMaker
 *  @date  2005-03-21 
 *  @author Vanya BELYAEV  belyaev@lapp.in2p3.fr
 */
// ============================================================================
/*  standard execution of the algorithm 
 *  @see LoKi::Algo 
 *  @return status code 
 */
// ===========================================================================
StatusCode LoKi::JetMaker::analyse   () 
{
  using namespace LoKi        ;
  using namespace LoKi::Types ;

  if ( m_associate2Vertex ){
    
    LoKi::Types::Fun bestVertexKey = LoKi::Cuts::BPV(LoKi::Cuts::VX);

    const LHCb::RecVertex::Range pvs = this->primaryVertices () ;
    for ( LHCb::RecVertex::Range::const_iterator i_pv = pvs.begin() ; pvs.end() != i_pv ; i_pv++ )
    {
      // Select all neutrals + all downstream + only long and upstream associated to this particular vertex
      Range part = 
        select ( "part" , LoKi::Cuts::Q == 0 || ( LHCb::Track::Downstream == LoKi::Cuts::TRTYPE || ( bestVertexKey == LoKi::Cuts::VX( *i_pv) ) ) ) ;
      
      // input container of "particles"
      IJetMaker::Jets jets ;
      
      if ( 0 == m_maker ) 
      { m_maker = tool<IJetMaker> ( m_makerName ,m_makerName, this ) ; }
      // make the jets 
      StatusCode sc = m_maker->makeJets ( part.begin () , part.end   () , jets  ) ;
      
      if ( sc.isFailure() ) { return Error ( "Error from jet maker" , sc ) ; }
      
      // save all jets
      while ( !jets.empty() ) 
      {
        LHCb::Particle* jet = jets.back() ;
        this->relate ( jet , *i_pv );
        save ( "jets" , jet ).ignore() ;
        jets.pop_back() ;
        delete jet ;
      }
    }
  }
  else{

    Range all = select ( "all" , LoKi::Cuts::PALL ) ;
    // input container of "particles"
    IJetMaker::Jets jets ;
    
    if ( 0 == m_maker ) 
    { m_maker = tool<IJetMaker> ( m_makerName ,m_makerName, this ) ; }
    
    
    // make the jets 
    StatusCode sc = m_maker->makeJets ( all.begin () , all.end   () , jets  ) ;
    
    if ( sc.isFailure() ) { return Error ( "Error from jet maker" , sc ) ; }
    
    
    // save all jets
    while ( !jets.empty() ) 
    {
      LHCb::Particle* jet = jets.back() ;
      save ( "jets" , jet ).ignore() ;
      jets.pop_back() ;
      delete jet ;
    }
  }
  
  if ( statPrint() || msgLevel ( MSG::DEBUG ) ) 
  { counter ( "#jets" ) += selected ("jets").size() ; }
  setFilterPassed ( true ) ;
  
  return StatusCode::SUCCESS ;
}
// ===========================================================================
/// The factory
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LoKi,JetMaker)
// ============================================================================
// The END 
// ============================================================================
