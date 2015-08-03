// $Id$
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <sstream>
// ============================================================================
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// MCInterfaces
// ============================================================================
#include "MCInterfaces/IMCParticleSelector.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/IMCHybridFactory.h"
#include "LoKi/Primitives.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Hybrid 
  {
    // ========================================================================
    /** @class MCParticleSelector 
     *  
     *  The first (test) attempt to develop a "hybrid"
     *  solution for LoKi+Hlt
     *  Simple tool (IMCParticleSelector) which is used in C++ analysis 
     *  environment, but the "cuts" are descrived through Python 
     * 
     *  This file is a part of LoKi project - 
     *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
     *
     *  The package has been designed with the kind help from
     *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
     *  contributions and advices from G.Raven, J.van Tilburg, 
     *  A.Golutvin, P.Koppenburg have been used in the design.
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2004-06-29
     */
    // ============================================================================
    class MCParticleSelector
      : public            GaudiTool
      , virtual public IMCParticleSelector
    {
      // ======================================================================
      // friend factory for instantiation 
      friend class ToolFactory<LoKi::Hybrid::MCParticleSelector> ;
      // ======================================================================
    public:
      // ======================================================================
      /// initialization of the tool 
      virtual StatusCode initialize () ;
      /// select
      virtual bool accept      ( const LHCb::MCParticle* p ) const      
      { return m_mccut.fun ( p ) ; }
      /// Test if filter is satisfied (functor interface)
      virtual bool operator()  ( const LHCb::MCParticle* p ) const 
      { return m_mccut.fun ( p ) ; }
      // ======================================================================
    protected:
      // ======================================================================
      /// Standard constructor
      MCParticleSelector 
      ( const std::string& type, 
        const std::string& name,
        const IInterface* parent)
        : GaudiTool ( type , name , parent )
        , m_mccut   ( LoKi::BasicFunctors<const LHCb::MCParticle*>::BooleanConstant( false ) ) 
        , m_code    ( "MCNONE" )
        , m_factory ("LoKi::Hybrid::MCTool/MCHybridFactory:PUBLIC") 
      {
        declareInterface<IMCParticleSelector>(this);
        declareProperty 
          ( "Code"    , m_code    , "Python pseudocode for the filter criteria"  ) ;
        declareProperty 
          ( "Factory" , m_factory , "Type/Name for C++/Python Hybrid MC-Factory" ) ;
      } ;
      /// destructor : virtual and protected
      virtual ~MCParticleSelector( ){}
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      MCParticleSelector() ;
      /// the copy constructor is disabled 
      MCParticleSelector           ( const MCParticleSelector& ) ;
      /// the assignement operator is disabled
      MCParticleSelector& operator=( const MCParticleSelector& ) ;
      // ======================================================================
    private:
      // ======================================================================
      // selection criteria itself 
      LoKi::Types::MCCut m_mccut ; ///< selection criteria itself
      // python pseudo-code
      std::string       m_code    ; ///< python pseudo-code
      // factory type/name
      std::string       m_factory ; ///< factory type/name
      // ======================================================================
    } ;
    // ========================================================================
  } // end of namespace LoKi::Hybrid
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// Declaration of the Tool Factory
// ============================================================================
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi::Hybrid,MCParticleSelector)
// ============================================================================
// initialization of the tool 
// ============================================================================
StatusCode LoKi::Hybrid::MCParticleSelector::initialize () 
{
  // (1) initialize the base 
  StatusCode  sc = GaudiTool::initialize() ;
  if ( sc.isFailure() ) { return sc ; }                          // RETURN 
  // (2) get the factory:
  IMCHybridFactory* factory = tool<IMCHybridFactory> ( m_factory , this ) ;  
  if ( 0 == factory ) 
  { return Error ( "Could not locate IMCHybridFactory" ) ; }     // RETURN 
  // (3) use the factory to get the cuts 
  sc = factory->get (  m_code , m_mccut ) ;
  if ( sc.isFailure() ) 
  { return Error ( "Error from IMCHybridFactory", sc   ) ; }     // RETURN 
  // 
  info() << "CUT: '" << m_mccut << "' "<< endmsg ;
  //
  return StatusCode::SUCCESS ;  
}
// ============================================================================
// The END 
// ============================================================================
