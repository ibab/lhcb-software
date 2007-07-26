// $Id: HybridMCParticleArrayFilter.cpp,v 1.1 2007-07-26 14:28:55 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <algorithm>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToolFactory.h" 
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/GaudiTool.h" 
// ============================================================================
// DaVinciMCKernel
// ============================================================================
#include "Kernel/IMCParticleArrayFilter.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/IMCHybridFactory.h"
#include "LoKi/select.h"
#include "LoKi/Primitives.h"
#include "LoKi/Operators.h"
// ============================================================================
namespace LoKi
{
  namespace Hybrid 
  {
    /** @class MCParticleArrayFilter HybridMCParticleArrayFilter.cpp
     *  Simple "hybrid-based" implementation of the interface 
     *  IMCParticelArrayFilter 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-07-26
     */
    class MCParticleArrayFilter
      : public GaudiTool 
      , public virtual IMCParticleArrayFilter 
    {
      /// the friend factory for istantiation
      friend class ToolFactory<LoKi::Hybrid::MCParticleArrayFilter> ;
    private:
      // ==================================================================
      /** Filter and put the results into new array
       *  @see IMCParticleArrayFilter
       */
      virtual StatusCode filter
      ( const LHCb::MCParticle::ConstVector& input    , 
        LHCb::MCParticle::ConstVector&       filtered ) const 
      {
        filtered.clear() ;
        // copy the particles,whcih satisfy the cut into output conterner
        LoKi::select                      ///< missing "std::copy_if" 
          ( input.begin ()              , ///< begin of the input sequence 
            input.end   ()              , ///< end of the input sequence      
            std::back_inserter ( filtered ) ,   ///< destination 
            m_mccut                         ) ; ///< criteria  
        //
        return StatusCode::SUCCESS ;
      }
      // ==================================================================
      /** Filter and remove elements that do not pass filter from array
       *  @see IMCParticleArrayFilter
       */
      virtual StatusCode filter
      ( LHCb::MCParticle::ConstVector& particles ) const 
      {
        // remove all particles which do *NOT* satisfy the criteria:
        particles.erase 
          ( std::remove_if ( particles.begin () , 
                             particles.end   () , 
                             !m_mccut           ) , particles.end () ) ;
        //
        return StatusCode::SUCCESS ;
      }
      // ==================================================================
    public:
      // ==================================================================
      /// intialize the tool 
      virtual StatusCode initialize () ;      
      // ==================================================================
    protected:
      // ==================================================================
      /// Standard constructor
      MCParticleArrayFilter  
      ( const std::string& type, 
        const std::string& name,
        const IInterface* parent)
        : GaudiTool ( type , name , parent )
        , m_mccut   ( LoKi::BooleanConstant<const LHCb::MCParticle*>( false ) ) 
        , m_code    ( "MCNONE" )
        , m_factory ("LoKi::Hybrid::MCTool/MCHybridFactory:PUBLIC") 
      {
        declareInterface<IMCParticleArrayFilter>(this);
        declareProperty 
          ( "Code"    , m_code    , "Python pseudocode for the filter criteria"  ) ;
        declareProperty 
          ( "Factory" , m_factory , "Type/Name for C++/Python Hybrid MC-Factory" ) ;
      } ;
      // ==================================================================      
      /// destructor : virtual and protected
      virtual ~MCParticleArrayFilter( ){}
      // ==================================================================
    private:
      // selection criteria itself 
      LoKi::Types::MCCut m_mccut ; ///< selection criteria itself
      // python pseudo-code
      std::string       m_code    ; ///< python pseudo-code
      // factory type/name
      std::string       m_factory ; ///< factory type/name
    } ;
  } // end of namespace Hybrid 
} // end of namespace LoKi 
// ============================================================================
/// Declaration of the Tool Factory
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi::Hybrid,MCParticleArrayFilter );
// ============================================================================
StatusCode LoKi::Hybrid::MCParticleArrayFilter::initialize () 
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
  info() << "CUT: '" << m_mccut << "' "<< endreq ;
  //
  return StatusCode::SUCCESS ;  
}
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
