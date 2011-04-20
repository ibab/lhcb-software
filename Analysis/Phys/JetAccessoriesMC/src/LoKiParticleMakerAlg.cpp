// $Id: LoKiHepMCParticleMakerAlg.cpp,v 1.4 2008/06/26 13:21:57 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
// ============================================================================
// LoKiAlgo
// ============================================================================
#include "LoKi/AlgoMC.h"
#include "LoKi/ParticleCuts.h"
// ============================================================================
// DaVinci Kernel 
// ============================================================================
#include "Kernel/IParticleMaker.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
#include "Relations/IRelation2D.h"
#include "Relations/Relation2D.h"

// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @class ParticleMaker
   *  
   *  This file is a part of LoKi project - 
   *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
   *
   *  The package has been designed with the kind help from
   *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
   *  contributions and advices from G.Raven, J.van Tilburg, 
   *  A.Golutvin, P.Koppenburg have been used in the design.
   *
   *  @author Victor COCO cocov@lapp.in2p3.fr
   *  @date   2009-10-28
   */
  class ParticleMaker : public LoKi::AlgoMC
  {
    // ========================================================================
    /// the friend factory for instantiation 
    friend class AlgFactory<LoKi::ParticleMaker> ;
    // ========================================================================
  protected:  
    // ========================================================================    
    /** Standard constructor
     *  @param name instance name 
     *  @param pSvc pointer to Service Locator 
     */
    ParticleMaker
    ( const std::string& name ,
      ISvcLocator*       pSvc ) 
      : LoKi::AlgoMC ( name , pSvc )
      // 
      , m_makerName ( "LoKi::HepMCParticleMaker"   )
      , m_maker     ( 0   )
    { 
      // 
      declareProperty 
        ( "ParticleMaker"  , 
          m_makerName , 
          "Type type/name of jet-maker tool (IParticleMaker interface)") ;  
      //
    }
    /// destructor
    virtual ~ParticleMaker( ){}
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
    ParticleMaker () ;
    // the copy constructor is disabled 
    ParticleMaker ( const ParticleMaker& )  ;
    // the assignement operator is disabled 
    ParticleMaker& operator=( const  ParticleMaker& )  ;
    // ========================================================================    
  private:  
    // ========================================================================    
    /// maker name
    std::string      m_makerName ; // jet maker name  
    /// maker
    IParticleMaker* m_maker     ; // jet maker to be used 
    // ========================================================================    
  };
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
/** @file 
 *  Implementation file for class  LoKi::ParticleMaker
 *  @date  2005-03-21 
 *  @author Vanya BELYAEV  belyaev@lapp.in2p3.fr
 */
// ============================================================================
/*  standard execution of the algorithm 
 *  @see LoKi::Algo 
 *  @return status code 
 */
// ===========================================================================
StatusCode LoKi::ParticleMaker::analyse   () 
{
  using namespace LoKi        ;
  using namespace LoKi::Types ;
  
  // create the maker
  if ( 0 == m_maker ) 
  { m_maker = tool<IParticleMaker> ( m_makerName ,m_makerName, this ) ; }

  // make the particles
  LHCb::Particle::ConstVector particles;
  StatusCode sc = m_maker->makeParticles( particles );


  // save all jets
  while ( !particles.empty() ) 
  {
    const LHCb::Particle* c_part((particles.back())) ;
    LHCb::Particle* part(c_part->clone()) ;
    save ( "particles" , part ).ignore() ;
    particles.pop_back() ;
    delete part ;
    delete c_part ;
  }

  //for (LHCb::Particle::ConstVector::const_iterator ip = particles.begin() ; particles.end() != ip ; ip++ ){
  //  const LHCb::Particle* p = *ip
  //  desktop()->keep(*ip);
  //}
  

  if ( sc.isFailure() ) { return Error ( "Error from particle maker" , sc ) ; }

  // sc = desktop()->saveDesktop  	();
  
  //sc = desktop()->saveTrees(particles);
  // sc = desktop()->saveDesktop  	();
  //if ( sc.isFailure() ){ return Error( "Not able to save desktop" );}

  if ( statPrint() || msgLevel ( MSG::DEBUG ) ) 
  { counter ( "#particles" ) += selected ("particles").size() ; }
  
  setFilterPassed ( true ) ;
  
  return StatusCode::SUCCESS ;
}
// ===========================================================================
/// The factory
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LoKi,ParticleMaker)
// ============================================================================
// The END 
// ============================================================================
