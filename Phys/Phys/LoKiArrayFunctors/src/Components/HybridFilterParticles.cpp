// $Id$
// ============================================================================
// Include files 
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiTool.h" 
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/IFilterParticles.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/IHybridFactory.h"
#include "LoKi/select.h"
#include "LoKi/Primitives.h"
// ============================================================================
// Local 
// ============================================================================
#include "Preambulo.h"
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Hybrid 
  {
    // ========================================================================
    /** @class FilterParticles HybridFilterParticles.cpp
     *  Simple "hybrid-based" implementation of the interface 
     *  IFilterParticles
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-07-26
     */
    class FilterParticles
      : public GaudiTool 
      , public virtual IFilterParticles
    {
      // ======================================================================
      /// the friend factory for istantiation
      friend class ToolFactory<LoKi::Hybrid::FilterParticles> ;
      // ======================================================================
    public:
      // ======================================================================
      /// Test if filter is satisfied on ensemble of particles
      virtual bool isSatisfied
      ( const LHCb::Particle::ConstVector& a ) const { return m_cut ( a ) ; }
      // ======================================================================
    public:
      // ======================================================================
      /// intialize the tool 
      virtual StatusCode initialize () ;      
      // finalize the tool (reset functor)
      virtual StatusCode finalize () 
      {
        // reset functor 
        m_cut = LoKi::Constant<LoKi::ATypes::Combination,bool>( false ) ;
        // finalize the base 
        return GaudiTool::finalize () ;
      }
      // ======================================================================
    protected:
      // ======================================================================
      /// Standard constructor
      FilterParticles
      ( const std::string& type, 
        const std::string& name,
        const IInterface* parent)
        : GaudiTool ( type , name , parent )
        , m_cut     ( LoKi::Constant<LoKi::ATypes::Combination,bool>( false ) ) 
        , m_code    ( "ANONE" )
        , m_factory ( "LoKi::Hybrid::Tool/HybridFactory:PUBLIC") 
        , m_preambulo () 
      {
        declareInterface<IFilterParticles>(this);
        //
        if      ( 0 == name.find ( "Hlt1" ) ) 
        { m_factory = "LoKi::Hybrid::Tool/Hlt1HybridFactory:PUBLIC" ; }
        else if ( 0 == name.find ( "Hlt2" ) ) 
        { m_factory = "LoKi::Hybrid::Tool/Hlt2HybridFactory:PUBLIC" ; }
        //
        declareProperty 
          ( "Code"    , 
            m_code    , 
            "Python pseudocode for the filter criteria"  ) 
          -> declareUpdateHandler 
          ( &LoKi::Hybrid::FilterParticles::propHandler , this ) ;
        //
        declareProperty 
          ( "Factory" , 
            m_factory , 
            "Type/Name for C++/Python Hybrid Factory" ) 
          -> declareUpdateHandler 
          ( &LoKi::Hybrid::FilterParticles::propHandler , this ) ;
        // the preambulo
        declareProperty 
          ( "Preambulo" , 
            m_preambulo , 
            "The preambulo to be used for Bender/Python script" ) 
          -> declareUpdateHandler 
          ( &LoKi::Hybrid::FilterParticles::propHandler , this ) ;
      } 
      // ======================================================================
      /// destructor : virtual and protected
      virtual ~FilterParticles () {}      // destructor : virtual and protected
      // ======================================================================
    public:
      // ======================================================================
      /// the preambulo 
      std::string preambulo() const { return _preambulo ( m_preambulo ) ; }
      /// the update handler
      void propHandler ( Property& /* p */ )  
      {
        //
        if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
        StatusCode sc = initCut  () ;
        Assert ( sc.isSuccess() , "Unable to set 'Code'"   , sc ) ;
      }
      // ======================================================================
    protected:
      // ======================================================================
      /// init the cut
      StatusCode initCut() ;      
      // ======================================================================
    private:
      // ======================================================================      
      /// selection criteria itself 
      LoKi::Types::ACut m_cut     ;                // selection criteria itself
      /// python pseudo-code
      std::string       m_code    ;                //        python pseudo-code
      /// factory type/name
      std::string       m_factory ;                //         factory type/name 
      /// preambulo 
      std::vector<std::string> m_preambulo ;       //                 preambulo
      // ======================================================================
    } ;
    // ========================================================================
  } //                                            end of namespace LoKi::Hybrid
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
// the intialization of the tool
// ============================================================================
StatusCode LoKi::Hybrid::FilterParticles::initialize () 
{
  // (1) initialize the base 
  StatusCode  sc = GaudiTool::initialize() ;
  if ( sc.isFailure() ) { return sc ; }                          // RETURN 
  //
  return initCut () ;
}
// ============================================================================
// the intialization of the cut
// ===========================================================================
StatusCode LoKi::Hybrid::FilterParticles::initCut () 
{
  // (1) get the factory:
  IHybridFactory* factory = tool<IHybridFactory> ( m_factory , this ) ;  
  if ( 0 == factory ) 
  { return Error ( "Could not locate IHybridFactory" ) ; }      // RETURN 
  // (2) use the factory to get the cuts 
  StatusCode sc = factory->get (  m_code , m_cut , preambulo() ) ;
  if ( sc.isFailure() ) 
  { return Error ( "Error from IHybridFactory", sc   ) ; }     // RETURN 
  //
  release ( factory ) ;
  //
  debug() << "CUT: '" << m_cut << "' "<< endreq ;
  //
  return StatusCode::SUCCESS ;  
}
// ============================================================================
/// Declaration of the Tool Factory
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi::Hybrid,FilterParticles)
// ============================================================================
// The END 
// ============================================================================


