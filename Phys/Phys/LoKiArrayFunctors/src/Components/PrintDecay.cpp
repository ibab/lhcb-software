// $Id:$ 
// ============================================================================
// Include files 
// ============================================================================
// STD&ST:
// ============================================================================
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/IService.h"
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// DaVinciInterfaces
// ============================================================================
#include "Kernel/IPrintDecay.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/DecayChain.h"
#include "LoKi/Objects.h"
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  class PrintDecay :  public extends1<GaudiTool,IPrintDecay>
  {
    // ========================================================================
    /// friend factpory (needed for instantiation)
    friend class ToolFactory<LoKi::PrintDecay> ;
    // ========================================================================
  public:
    // ========================================================================
    /** Print the decay tree for a given particle
     *
     *  @code
     * 
     *  IPrintDecay* tool = ... ;
     *  const LHCb::Particle* B = ... ;
     * 
     *  tool -> printDecay ( B ) ; 
     *
     *  @endcode 
     *
     *  @param mother the pointer to the particle 
     *  @param maxDepth the maximal depth level
     */
    virtual void printTree
    ( const LHCb::Particle* mother   , 
      int                   maxDepth ) ;
    // ========================================================================
  public:
    // ========================================================================

    // ========================================================================
  public:
    // ========================================================================
    /// initialize it!
    StatusCode initialize () ; // initialize it!
    /// the only method...
    StatusCode execute    () ; // the only method...
    // ========================================================================
  protected:
    // ========================================================================
    /// constructor 
    PrintDecay 
    ( const std::string& type   , 
      const std::string& name   , 
      const IInterface*  parent ) ;
    /// destructor 
    virtual ~PrintDecay() {}
    // ========================================================================
  private:
    // ========================================================================
    /// default constructor is disabled 
    PrintDecay () ;
    /// copy constructor is disabled 
    PrintDecay           ( const PrintDecay& ) ;  // no copy 
    /// assignement operator is disabled 
    PrintDecay& operator=( const PrintDecay& ) ;  // no assignement 
    // ========================================================================             
  private:
    // ========================================================================             
    /// print mode, @see LoKi::DecayChainBase::Mode
    unsigned       m_mode   ; // LoKi::DecayChainBase::Mode
    /// print vertex ? 
    bool           m_vertex ; // print vertex ? 
    // ========================================================================             
  };
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
// initialize it!
// ============================================================================
StatusCode LoKi::PrintDecay::initialize ()  // initialize it!
{
  StatusCode sc = GaudiTool::initialize() ;
  if ( sc.isFailure() ) { return sc ; }
  /// ensure LoKi  service is running 
  svc<IService>("LoKiSvc", true ) ;
  //
  if ( LoKi::DecayChainBase::LV_MPTETA < m_mode ) 
  { m_mode = LoKi::DecayChainBase::LV_STD ; }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// the only method...
// ============================================================================
void LoKi::PrintDecay::printTree
( const LHCb::Particle* mother   , 
  int                   maxDepth ) 
{
  //
  const size_t depth = 0 <= maxDepth ? (size_t) maxDepth : 10000 ;
  //
  const LoKi::DecayChainBase::Mode mode = 
    m_mode <= LoKi::DecayChainBase::LV_MPTETA ? 
    (LoKi::DecayChainBase::Mode) m_mode :  LoKi::DecayChainBase::LV_STD ;
  //
  // create printer 
  LoKi::DecayChain printer ( depth , m_vertex   , mode ) ;
  //
  // use it! 
  printer.print ( mother                  , 
                  info  ()                ,
                  endreq                  , 
                  LoKi::Objects::_VALID_  , 
                  LoKi::Objects::_NONE_   ) ;
  //
}
// ============================================================================
// constructor 
// ============================================================================
LoKi::PrintDecay::PrintDecay 
( const std::string& name   , 
  const std::string& type   , 
  const IInterface*  parent ) 
  : base_class ( name , type , parent ) 
  , m_mode   ( LoKi::DecayChainBase::LV_STD ) 
  , m_vertex ( true ) 
{
  declareProperty 
    ( "PrintVertex" , m_vertex , "Print vertex information?" ) ; 
  declareProperty 
    ( "Mode"        , m_mode   , "Print mode, see LoKi::DecayChainBase::Mode" ) ; 
}
// ============================================================================
/// Declaration of the Algorithm Factory
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi,PrintDecay)
// ============================================================================
// The END 
// ============================================================================
