// $Id$
// ============================================================================
// Include files
// ============================================================================
#include <algorithm>
#include <functional>
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/IParticleArrayFilter.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/IHybridFactory.h"
#include "LoKi/Primitives.h"
#include "LoKi/Operators.h"
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
    /** @class ParticleArrayFilter HybridParticleArrayFilter.cpp
     *  Simple "hybrid-based" implementation of the interface
     *  IParticelArrayFilter
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-07-26
     */
    class ParticleArrayFilter
      : public GaudiTool
      , public virtual IParticleArrayFilter
    {
      // ======================================================================
      /// the friend factory for istantiation
      friend class ToolFactory<LoKi::Hybrid::ParticleArrayFilter> ;
      // ======================================================================
    public:
      // ==================================================================
      /** Filter and put the results into new array
       *  @see IParticleArrayFilter
       */
      virtual StatusCode filter
      ( const LHCb::Particle::ConstVector& input    ,
        LHCb::Particle::ConstVector&       filtered ) const
      {
        filtered.clear() ;
        // copy the particles,whcih satisfy the cut into output conterner
        std::copy_if                      ///< missing "std::copy_if"
          ( input.begin ()              , ///< begin of the input sequence
            input.end   ()              , ///< end of the input sequence
            std::back_inserter ( filtered ) ,   ///< destination
            std::cref(m_cut)                ) ; ///< criteria
        //
        return StatusCode::SUCCESS ;
      }
      // ==================================================================
      /** Filter and remove elements that do not pass filter from array
       *  @see IParticleArrayFilter
       */
      virtual StatusCode filter
      ( LHCb::Particle::ConstVector& particles ) const
      {
        // remove all particles which do *NOT* satisfy the criteria:
        particles.erase
          ( std::remove_if ( particles.begin () ,
                             particles.end   () ,
                             !m_cut             ) , particles.end () ) ;
        //
        return StatusCode::SUCCESS ;
      }
      // ======================================================================
    public:
      // ======================================================================
      /// intialize the tool
      virtual StatusCode initialize ()
      {
        // initialize the base
        StatusCode  sc = GaudiTool::initialize() ;
        if ( sc.isFailure() ) { return sc ; }                          // RETURN
        //
        return initVar() ;
      }
      /// finalize the tool
      virtual StatusCode finalize()
      {
        // reset the functor
        m_cut = LoKi::Constant<const LHCb::Particle*,bool>( false ) ;
        // finalize the base:
        return GaudiTool::finalize () ;
      }
      // ======================================================================
    protected:
      // ======================================================================
      /// initialization of the tool
      StatusCode initVar     () ;
      // ======================================================================
    public:
      // ======================================================================
      /// the preambulo
      std::string preambulo() const { return _preambulo ( m_preambulo ) ; }
      // ======================================================================
    public:
      // ======================================================================
      /// the update handler
      void propHandler ( Property& /* p */ )
      {
        //
        if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
        //
        StatusCode sc = initVar () ;
        Assert ( sc.isSuccess() , "Unable to set 'Code'"   , sc ) ;
      }
      // ======================================================================
    protected:
      // ==================================================================
      /// Standard constructor
      ParticleArrayFilter
      ( const std::string& type,
        const std::string& name,
        const IInterface* parent)
        : GaudiTool ( type , name , parent )
        , m_cut     ( LoKi::Constant<const LHCb::Particle*,bool>( false ) )
        , m_code    ( "NONE" )
        , m_factory ( "LoKi::Hybrid::Tool/HybridFactory:PUBLIC")
        , m_preambulo ()
      {
        declareInterface<IParticleArrayFilter>(this);
        //
        if      ( 0 == name.find ( "Hlt1" ) )
        { m_factory = "LoKi::Hybrid::Tool/Hlt1HybridFactory:PUBLIC" ; }
        else if ( 0 == name.find ( "Hlt2" ) )
        { m_factory = "LoKi::Hybrid::Tool/Hlt2HybridFactory:PUBLIC" ; }
        //
        declareProperty
          ( "Code"    ,
            m_code    ,
            "Python pseudocode for the filter criteria"  ) ->
          declareUpdateHandler
          ( &LoKi::Hybrid::ParticleArrayFilter::propHandler , this ) ;
        //
        declareProperty
          ( "Factory" ,
            m_factory ,
            "Type/Name for C++/Python Hybrid Factory" ) ->
          declareUpdateHandler
          ( &LoKi::Hybrid::ParticleArrayFilter::propHandler , this ) ;
        //
        declareProperty
          ( "Preambulo" ,
            m_preambulo ,
            "Preambulo to be used for Bender/Python script" ) ->
          declareUpdateHandler
          ( &LoKi::Hybrid::ParticleArrayFilter::propHandler , this ) ;
        //
      }
      // ======================================================================
      /// destructor : virtual and protected
      virtual ~ParticleArrayFilter( ){}
      // ======================================================================
    private:
      // ======================================================================
      /// selection criteria itself
      LoKi::Types::Cut  m_cut     ;                // selection criteria itself
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
// the intialization fo the tool
// ============================================================================
StatusCode LoKi::Hybrid::ParticleArrayFilter::initVar ()
{
  // (2) get the factory:
  IHybridFactory* factory = tool<IHybridFactory> ( m_factory , this ) ;
  if ( !factory )
  { return Error ( "Could not locate IHybridFactory" ) ; }      // RETURN
  // (3) use the factory to get the cuts
  StatusCode sc = factory->get (  m_code , m_cut , preambulo() ) ;
  if ( sc.isFailure() )
  { return Error ( "Error from IHybridFactory", sc   ) ; }     // RETURN
  //
  debug() << "CUT: '" << m_cut << "' "<< endmsg ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
/// Declaration of the Tool Factory
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi::Hybrid,ParticleArrayFilter)
// ============================================================================
// The END
// ============================================================================
