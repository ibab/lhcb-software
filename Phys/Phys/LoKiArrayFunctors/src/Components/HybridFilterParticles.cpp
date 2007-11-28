// $Id: HybridFilterParticles.cpp,v 1.2 2007-11-28 14:55:54 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToolFactory.h" 
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
namespace LoKi
{
  namespace Hybrid 
  {
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
      /// the friend factory for istantiation
      friend class ToolFactory<LoKi::Hybrid::FilterParticles> ;
    public:
      /// Test if filter is satisfied on ensemble of particles
      virtual bool isSatisfied
      ( const LHCb::Particle::ConstVector& a ) const { return m_cut ( a ) ; }
      /// Test if filter is satisfied on ensemble of particles
      virtual bool operator()
      ( const LHCb::Particle::ConstVector& a ) const { return m_cut ( a ) ; }
    public:
      // ==================================================================
      /// intialize the tool 
      virtual StatusCode initialize () ;      
      // ==================================================================
    protected:
      // ==================================================================
      /// Standard constructor
      FilterParticles
      ( const std::string& type, 
        const std::string& name,
        const IInterface* parent)
        : GaudiTool ( type , name , parent )
        , m_cut     ( LoKi::Constant<LoKi::ATypes::Combination,bool>( false ) ) 
        , m_code    ( "ANONE" )
        , m_factory ( "LoKi::Hybrid::Tool/HybridFactory:PUBLIC") 
      {
        declareInterface<IFilterParticles>(this);
        declareProperty 
          ( "Code"    , m_code    , "Python pseudocode for the filter criteria"  ) ;
        declareProperty 
          ( "Factory" , m_factory , "Type/Name for C++/Python Hybrid Factory" ) ;
      } ;
      // ==================================================================      
      /// destructor : virtual and protected
      virtual ~FilterParticles () {}
      // ==================================================================
    private:
      // selection criteria itself 
      LoKi::Types::ACut m_cut ; ///< selection criteria itself
      // python pseudo-code
      std::string       m_code    ; ///< python pseudo-code
      // factory type/name
      std::string       m_factory ; ///< factory type/name
    } ;
    // ==================================================================
  } // end of namespace LoKi::Hybrid
} // end of namespace LoKi 
// ============================================================================
/// Declaration of the Tool Factory
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi::Hybrid,FilterParticles);
// ============================================================================
// the intializationfo the tool
// ============================================================================
StatusCode LoKi::Hybrid::FilterParticles::initialize () 
{
  // (1) initialize the base 
  StatusCode  sc = GaudiTool::initialize() ;
  if ( sc.isFailure() ) { return sc ; }                          // RETURN 
  // (2) get the factory:
  IHybridFactory* factory = tool<IHybridFactory> ( m_factory , this ) ;  
  if ( 0 == factory ) 
  { return Error ( "Could not locate IHybridFactory" ) ; }      // RETURN 
  // (3) use the factory to get the cuts 
  sc = factory->get (  m_code , m_cut ) ;
  if ( sc.isFailure() ) 
  { return Error ( "Error from IHybridFactory", sc   ) ; }     // RETURN 
  // 
  info() << "CUT: '" << m_cut << "' "<< endreq ;
  //
  return StatusCode::SUCCESS ;  
}
// ============================================================================
// The END 
// ============================================================================


