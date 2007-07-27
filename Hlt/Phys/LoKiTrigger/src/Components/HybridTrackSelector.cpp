// $Id: HybridTrackSelector.cpp,v 1.1.1.1 2007-07-27 15:56:42 ibelyaev Exp $
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
// TrackInerfaces 
// ============================================================================
#include "TrackInterfaces/ITrackSelector.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/ITrHybridFactory.h"
#include "LoKi/Primitives.h"
#include "LoKi/Operators.h"
// ============================================================================
namespace LoKi
{
  namespace Hybrid 
  {
    /** @class TrackSelector  HybridTrackSelector.cpp
     *  Simple "hybrid-based" implementation of the interface 
     *  ITrackSelector 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-07-26
     */
    class TrackSelector
      : public GaudiTool 
      , public virtual ITrackSelector
    {
      /// the friend factory for istantiation
      friend class ToolFactory<LoKi::Hybrid::TrackSelector> ;
    public:
      // ==================================================================
      /// the method @see ITrackSelector
      virtual bool accept(const LHCb::Track& aTrack) const 
      { return m_cut ( aTrack ) ; }
      // ==================================================================
    public:
      // ==================================================================
      /// intialize the tool 
      virtual StatusCode initialize () ;      
      // ==================================================================
    protected:
      // ==================================================================
      /// Standard constructor
      TrackSelector  
      ( const std::string& type, 
        const std::string& name,
        const IInterface* parent)
        : GaudiTool ( type , name , parent )
        , m_cut     ( LoKi::BooleanConstant<LHCb::Track>( false ) ) 
        , m_code    ( "TrNONE" )
        , m_factory ( "LoKi::Hybrid::TrTool/TrHybridFactory:PUBLIC") 
      {
        declareInterface<ITrackSelector>(this);
        declareProperty 
          ( "Code"    , m_code    , "Python pseudocode for the filter criteria"  ) ;
        declareProperty 
          ( "Factory" , m_factory , "Type/Name for C++/Python Hybrid Factory" ) ;
      } ;
      // ==================================================================      
      /// destructor : virtual and protected
      virtual ~TrackSelector () {}
      // ==================================================================
    private:
      // selection criteria itself 
      LoKi::Types::TrCut m_cut ; ///< selection criteria itself
      // python pseudo-code
      std::string        m_code    ; ///< python pseudo-code
      // factory type/name
      std::string        m_factory ; ///< factory type/name
    } ;  
  } // end of namespace Hybrid 
} // end of namespace LoKi 
// ============================================================================
/// Declaration of the Tool Factory
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi::Hybrid,TrackSelector);
// ============================================================================
// the intializationfo the tool
// ============================================================================
StatusCode LoKi::Hybrid::TrackSelector::initialize () 
{
  // (1) initialize the base 
  StatusCode  sc = GaudiTool::initialize() ;
  if ( sc.isFailure() ) { return sc ; }                          // RETURN 
  // (2) get the factory:
  ITrHybridFactory* factory = tool<ITrHybridFactory> ( m_factory , this ) ;  
  if ( 0 == factory ) 
  { return Error ( "Could not locate ITrHybridFactory" ) ; }      // RETURN 
  // (3) use the factory to get the cuts 
  sc = factory->get (  m_code , m_cut ) ;
  if ( sc.isFailure() ) 
  { return Error ( "Error from ITrHybridFactory", sc   ) ; }     // RETURN 
  // 
  info() << "CUT: '" << m_cut << "' "<< endreq ;
  //
  return StatusCode::SUCCESS ;  
}
// ============================================================================
// The END 
// ============================================================================
