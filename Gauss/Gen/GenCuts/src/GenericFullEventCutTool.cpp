// $Id: $
// =============================================================================
// Include files 
// =============================================================================
// STD & STL 
// =============================================================================
#include <cmath>
// =============================================================================
// GaudiKernel
// =============================================================================
#include "GaudiKernel/ToolFactory.h"
// =============================================================================
// GenEvent
// =============================================================================
#include "Event/HepMCEvent.h"
// =============================================================================
// Generators 
// =============================================================================
#include "MCInterfaces/IFullGenEventCutTool.h"
// =============================================================================
// LoKi
// =============================================================================
#include "LoKi/GenTypes.h"
#include "LoKi/IGenDecay.h"
#include "LoKi/IGenHybridFactory.h"
#include "LoKi/FilterTool.h"
// =============================================================================
namespace LoKi 
{
  // ===========================================================================
  /** @class   FullGenEventCut
   *  
   *  Simple generic implementation of IFullGenEventCutTool interface
   *  to check if event is "good"
   *
   *  Select event with at leats two J/psi:
   *  @code 
   * 
   *    tool.Code = " count ( 'J/psi(1S)' == GID )  > 1 "
   *
   *  @endcode 
   *
   *  @see IFullGenEventCutTool
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   */
  class FullGenEventCut
  // : public extends1<LoKi::FilterTool,IFullGenEventCutTool>
    : public         LoKi::FilterTool
    , public virtual IFullGenEventCutTool
  {
    // =========================================================================
    // friend factor for instantiation 
    friend class ToolFactory<LoKi::FullGenEventCut> ;
    // =========================================================================
  public:
    // =========================================================================
    /// Initialization
    virtual StatusCode initialize () { return LoKi::FilterTool::initialize () ; }  
    /// Finalization 
    virtual StatusCode finalize   () ;
    // =========================================================================
    /** Apply the cut on a event.
     *  @param theEvents      Container of all interactions in the event.
     *  @param theCollisions  Container of hard process informations of each 
     *                            pile-up interactions of the event.
     *  @return    true  if the full event passes the cut.
     *  @see IFullGEnEventrCutTool
     */  
    virtual bool studyFullEvent
    ( LHCb::HepMCEvents*       theEvents        ,
      LHCb::GenCollisions*  /* theCollisions */ ) const ;
    // =========================================================================
  public:
    // =========================================================================
    /// perform the actual decoding of the functor 
    virtual StatusCode decode () ; // perform the actual decoding of the functor 
    // =========================================================================
  protected:
    // =========================================================================
    /** standard constructor 
     *  @param type the tool type (???)
     *  @param name the actual instance name 
     *  @param parent the tool parent 
     */
    FullGenEventCut 
    ( const std::string&  type   ,                     //    the tool type (???)
      const std::string&  name   ,                     // the tool isntance name 
      const IInterface*   parent ) ;                   //        the tool parent 
   /// virtual & protected desctructor 
    virtual ~FullGenEventCut () ;             // virtual & protected desctructor 
    // =========================================================================
  private:
    // =========================================================================
    /// the default constructor is disabled 
    FullGenEventCut () ;                  // the default constructor is disabled 
    /// the copy constructor is disabled 
    FullGenEventCut ( const FullGenEventCut& ) ;          // no copy constructor 
    /// assignement operator is disabled 
    FullGenEventCut& operator= ( const FullGenEventCut& );     // no assignement 
    // =========================================================================
  private:
    // =========================================================================
    /// the actual code 
    LoKi::Types::GCutVal   m_cutval ;  // the actual code 
    // =========================================================================
  } ;
  // ===========================================================================
} //                                                       end of namespace LoKi
// =============================================================================
namespace 
{
  // ===========================================================================
  typedef  LoKi::BasicFunctors<LoKi::GenTypes::GenContainer>::BooleanConstant _BOOLC ;
  // ===========================================================================
}
// =============================================================================
/*  standard constructor 
 *  @param type the tool type (???)
 *  @param name the actual instance name 
 *  @param parent the tool parent 
 */
// =============================================================================
LoKi::FullGenEventCut::FullGenEventCut 
( const std::string&  type   ,                         //    the tool type (???)
  const std::string&  name   ,                         // the tool isntance name 
  const IInterface*   parent )                         //        the tool parent
// : base_class ( type , name , parent ) 
  : LoKi::FilterTool ( type , name , parent ) 
//
  , m_cutval   ( _BOOLC ( false )  ) 
///
{
  //
  declareInterface<IFullGenEventCutTool> ( this ) ;
  //
  addToPreambulo ( "from LoKiGen.decorators     import *" ) ;
  addToPreambulo ( "from LoKiNumbers.decorators import *" ) ;
  addToPreambulo ( "from LoKiCore.functions     import *" ) ;
  addToPreambulo ( "from LoKiCore.math          import *" ) ;
  //
  StatusCode sc = setProperty ( "Factory" , "LoKi::Hybrid::GenTool/GenFactory") ;
  Assert ( sc.isSuccess() , "Unable to reset 'Factory' property" , sc ) ;
}
// ==============================================================================
// destructor 
// ==============================================================================
LoKi::FullGenEventCut::~FullGenEventCut () {}
// ==============================================================================
// Finalization 
// ==============================================================================
StatusCode LoKi::FullGenEventCut::finalize () 
{
  // reset the functor 
  m_cutval = _BOOLC ( false )  ;
  // finalize the base class 
  return LoKi::FilterTool::finalize () ;
}
// ============================================================================
// perform the actual decoding of the functor 
// ============================================================================
StatusCode LoKi::FullGenEventCut::decode() 
{
  //
  typedef LoKi::IGenHybridFactory FACTORY ;
  typedef LoKi::Types::GCutVal    FUNCTOR ;
  //
  m_cutval = _BOOLC ( false )  ;
  return i_decode<FACTORY>( m_cutval ) ;
}
// ============================================================================
/*  Apply the cut on a event.
 *  @param theEvents      Container of all interactions in the event.
 *  @param theCollisions  Container of hard process informations of each 
 *                            pile-up interactions of the event.
 *  @return    true  if the full event passes the cut.
 *  @see IFullGEnEventrCutTool
 */
// ============================================================================
bool LoKi::FullGenEventCut::studyFullEvent
( LHCb::HepMCEvents*       theEvents        ,
  LHCb::GenCollisions*  /* theCollisions */ ) const 
{
  //
  if ( 0 == theEvents ) 
  {
    Error ( "LHCb::HepMCEvents* points to NULL, reject event") ;
    return false ;                                             // RETURN
  }
  //
  Assert ( !updateRequired () , "Update is required!" ) ;
  //
  // copy all particles into one constainer 
  LoKi::GenTypes::GenContainer particles ;
  //
  for ( LHCb::HepMCEvents::const_iterator ievent = theEvents->begin() ;
        theEvents -> end() != ievent ;  ++ievent ) 
  {
    //
    const LHCb::HepMCEvent* event = *ievent ;
    if ( 0 == event )                   { continue ; }         // CONTINUE 
    //
    const HepMC::GenEvent*  evt   = event -> pGenEvt () ;
    if ( 0 == evt   )                   { continue ; }         // CONTINUE 
    //
    particles.insert ( particles.end ()          , 
                       evt -> particles_begin () , 
                       evt -> particles_end   () ) ;                   
  }
  //
  // evaluate the functor:
  const bool result = m_cutval ( particles ) ;
  //
  counter( "#accept" ) += result ;
  //
  return result ;
}
// ============================================================================
// factory
// ============================================================================
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi,FullGenEventCut)
// ============================================================================
// The END 
// ============================================================================
