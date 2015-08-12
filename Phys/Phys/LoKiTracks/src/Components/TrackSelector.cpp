// $Id$
// ============================================================================
// $URL$
// ============================================================================
// Include files 
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
#include "LoKi/ITrackFunctorFactory.h"
#include "LoKi/Primitives.h"
#include "LoKi/Operators.h"
// ============================================================================
// Boost 
// ============================================================================
#include "boost/algorithm/string/join.hpp"
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Hybrid 
  {
    // ========================================================================
    /** @class TrackSelector     TrackSelector.cpp
     *
     *  Simple "hybrid-based" implementation of the interface 
     *  ITrackSelector 
     *  
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-07-26
     */
    class TrackSelector 
    // : public extends1<GaudiTool,ITrackSelector>
      : public virtual GaudiTool
      , public virtual ITrackSelector
    {
      // ======================================================================
      /// the friend factory for istantiation
      friend class ToolFactory<LoKi::Hybrid::TrackSelector> ;
      // ======================================================================
    public:
      // ======================================================================
      /// the method @see ITrackSelector
      virtual bool accept(const LHCb::Track& aTrack) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// intialize the tool 
      virtual StatusCode initialize () ;      
      // ======================================================================
    protected:
      // ======================================================================
      /// Standard constructor
      TrackSelector  
      ( const std::string& type, 
        const std::string& name,
        const IInterface* parent) ;
      // ======================================================================
      /// destructor : virtual and protected
      virtual ~TrackSelector () {}
      // ======================================================================
    protected:
      // ======================================================================
      // update handler for properties 
      void update ( Property& p ) ;            // update handler for properties 
      /// decode the cuts 
      StatusCode decode () const ;
      /// get the preambulo 
      std::string preambulo() const ;
      // ======================================================================
    private:
      // ======================================================================
      /// selection criteria itself 
      mutable LoKi::Types::TrCut m_cut             ; //      selection criteria 
      /// python pseudo-code
      std::string                m_code            ; //      python pseudo-code
      /// factory type/name
      std::string                m_factory         ; //       factory type/name
      /// preambulo 
      std::vector<std::string>   m_preambulo       ; //           the preambulo 
      /// update required ? 
      mutable bool               m_update_required ; //       update required ?
      // ======================================================================
    } ; 
    // ========================================================================
  } //                                            end of namespace LoKi::Hybrid 
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
// Standard constructor
LoKi::Hybrid::TrackSelector::TrackSelector  
( const std::string& type, 
  const std::string& name,
  const IInterface* parent)
// : base_class  ( type , name , parent )
  : GaudiTool   ( type , name , parent )
  , m_cut       ( LoKi::BasicFunctors<const LHCb::Track*>::BooleanConstant ( false ) ) 
  , m_code      ( "TrNONE" )
  , m_factory   ( "LoKi::Hybrid::TrackFunctorFactory/TrackFunctorFactory:PUBLIC") 
  , m_preambulo () 
  , m_update_required ( true ) 
{
  declareInterface<ITrackSelector> ( this ) ;
  // 
  if      ( 0 == name.find ( "Hlt1" ) ) 
  { m_factory = "LoKi::Hybrid::TrackFunctorFactory/Hlt1TrackFunctorFactory:PUBLIC" ; }
  else if ( 0 == name.find ( "Hlt2" ) ) 
  { m_factory = "LoKi::Hybrid::TrackFunctorFactory/Hlt2TrackFunctorFactory:PUBLIC" ; }
  //
  declareProperty 
    ( "Code"    , 
      m_code    , 
      "Python pseudocode for the filter criteria"           ) 
    -> declareUpdateHandler ( &LoKi::Hybrid::TrackSelector::update , this ) ;
  //
  declareProperty 
    ( "Factory" , 
      m_factory , 
      "Type/Name for C++/Python LoKi/Bender Hybrid Factory" ) 
    -> declareUpdateHandler ( &LoKi::Hybrid::TrackSelector::update , this ) ;
  //
  declareProperty 
    ( "Preambulo" , 
      m_preambulo , 
      "Additional Python lines to be used for processing"   ) 
    -> declareUpdateHandler ( &LoKi::Hybrid::TrackSelector::update , this ) ;
} 
// ============================================================================
// decode the cuts 
// ============================================================================
StatusCode LoKi::Hybrid::TrackSelector::decode () const  
{
  //
  m_update_required = true  ;
  // (1) get the factory:
  LoKi::ITrackFunctorFactory* factory = 
    tool<LoKi::ITrackFunctorFactory> ( m_factory , this ) ;
  if ( 0 == factory ) 
  { return Error ( "Could not locate LoKi::ITrackFunctorFactory" ) ; } // RETURN 
  // (2) use the factory to get the cuts 
  StatusCode sc = factory->get (  m_code , m_cut  , preambulo() ) ;
  if ( sc.isFailure() ) 
  { return Error ( "Error from LoKi::ITrackFunctorFactory", sc   ) ; } // RETURN 
  // 
  info() << "CUT: '" << m_cut << "' "<< endreq ;
  //
  m_update_required = false ;
  // (3) release factory, not needed anymore
  release ( factory ) ;
  //
  return StatusCode::SUCCESS ;  
}
// ============================================================================
// the intialization of the tool
// ============================================================================
StatusCode LoKi::Hybrid::TrackSelector::initialize () 
{
  // (1) initialize the base 
  StatusCode  sc = GaudiTool::initialize() ;
  if ( sc.isFailure() ) { return sc ; }                                // RETURN 
  // (2) decode the cuts 
  sc = decode() ;
  if ( sc.isFailure() ) { return Error("Unable to decode the cuts" , sc ) ; }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// update handler for properties 
// ============================================================================
void LoKi::Hybrid::TrackSelector::update ( Property& p ) // update the conditions map 
{
  //
  m_update_required = true ;
  //
  // no action if not yet initialized 
  if ( Gaudi::StateMachine::INITIALIZED > FSMState()   ) { return ; } 
  // 
  debug () << " Property is updated: " << p << endmsg ;
  //
  m_update_required = true ;
  //
}
// ============================================================================
// the method @see ITrackSelector
// ============================================================================
bool LoKi::Hybrid::TrackSelector::accept ( const LHCb::Track& aTrack ) const 
{
  if ( m_update_required ) 
  {
    StatusCode sc = decode() ;
    Assert ( sc.isSuccess() , "Unable to decode the code" ) ;
  }
  //
  return m_cut ( &aTrack ) ; 
}
// ============================================================================
// get the preambulo 
// ============================================================================
std::string LoKi::Hybrid::TrackSelector::preambulo() const 
{ return boost::algorithm::join( m_preambulo , "\n" ) ; }


// ============================================================================
/// Declaration of the Tool Factory
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi::Hybrid,TrackSelector)
// ============================================================================
// The END 
// ============================================================================
