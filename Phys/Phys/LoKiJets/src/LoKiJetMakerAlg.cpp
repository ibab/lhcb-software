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
      , m_runJetID ( true  )
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
      declareProperty 
        ( "JetID"  , 
          m_runJetID , 
          "Append Jet ID information to the jet") ;  
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

    StatusCode appendJetIDInfo   ( LHCb::Particle * jet ) ;
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
    /// append jet ID info
    bool m_runJetID              ; // append jet ID info
    
    
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
  using namespace LoKi::Cuts ;

  if ( m_associate2Vertex ){
    
    LoKi::Types::Fun bestVertexKey = BPV(VX);

    Range part = select("part",PALL);
    
    const LHCb::RecVertex::Range pvs = this->primaryVertices () ;
    for ( LHCb::RecVertex::Range::const_iterator i_pv = pvs.begin() ; pvs.end() != i_pv ; i_pv++ )
    {
      IJetMaker::Input inputs;
      for (Range::const_iterator i_p = part.begin() ; part.end() != i_p ; i_p++ ){
        if (Q(*i_p) == 0 ){
          if ( ABSID(*i_p) != 310 && ABSID(*i_p) != 3122 ) inputs.push_back(*i_p) ;
          else if ( bestVertexKey(*i_p) == VX( *i_pv) ) inputs.push_back(*i_p) ;
          else continue ;
        }
        else{
          if ( LHCb::Track::Downstream == TRTYPE(*i_p) ) inputs.push_back(*i_p) ;
          else if ( bestVertexKey(*i_p) == VX( *i_pv) ) inputs.push_back(*i_p) ;
          else continue ;
        }
      }

      // input container of "particles"
      IJetMaker::Jets jets ;
      
      if ( 0 == m_maker ) 
      { m_maker = tool<IJetMaker> ( m_makerName ,m_makerName, this ) ; }
      // make the jets 
      StatusCode sc = m_maker->makeJets ( inputs.begin () , inputs.end   () , jets  ) ;
      
      if ( sc.isFailure() ) { return Error ( "Error from jet maker" , sc ) ; }
      
      // save all jets
      while ( !jets.empty() ) 
      {
        LHCb::Particle* jet = jets.back() ;
        if(m_runJetID) this->appendJetIDInfo(jet);
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
      if(m_runJetID) this->appendJetIDInfo(jet);
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

StatusCode LoKi::JetMaker::appendJetIDInfo( LHCb::Particle* jet )
{
  // Do whatever
  jet->addInfo ( 9001 , 0. );
  
  return SUCCESS;
}

// ===========================================================================
/// The factory
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LoKi,JetMaker)
// ============================================================================
// The END 
// ============================================================================
