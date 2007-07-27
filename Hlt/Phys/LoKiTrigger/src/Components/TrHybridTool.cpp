// $Id: TrHybridTool.cpp,v 1.1.1.1 2007-07-27 15:56:42 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <deque>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/System.h" 
#include "GaudiKernel/ToolFactory.h" 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/HybridBase.h"
#include "LoKi/TrHybridLock.h"
#include "LoKi/ITrHybridTool.h"
#include "LoKi/ITrHybridFactory.h"
// ============================================================================
/** @file 
 *  definition and implementation file for class LoKi::Hybrid::TrTool
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
 *  @date 2004-06-29
 */
namespace LoKi 
{
  namespace Hybrid 
  {
    /** @class TrTool 
     *
     *  Concrete impelmentation of LoKi::ITrHybridTool interface 
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
    class TrTool 
      : public      LoKi::Hybrid::Base 
      , public virtual ITrHybridTool 
      , public virtual ITrHybridFactory 
    {
      // friend factory forn instantiation
      friend class ToolFactory<LoKi::Hybrid::TrTool> ;
    public:
      /// initialization of the tool 
      virtual StatusCode initialize () ;
      /// finalization   of the tool 
      virtual StatusCode finalize  () ;
    public:
      // ========================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param cuts the placeholder for the result 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string& pycode , LoKi::Types::TrCut&  cuts ) 
      { return _get ( pycode , m_trcuts  , cuts ) ; }
      // ========================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param cuts the placeholder for the result 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string& pycode , LoKi::Types::RVCut&  cuts ) 
      { return _get ( pycode , m_rvcuts  , cuts ) ; }
      // ========================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param cuts the placeholder for the result 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string& pycode , LoKi::Types::TTrCut& cuts ) 
      { return _get ( pycode , m_ttrcuts  , cuts ) ; }
      // ========================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param cuts the placeholder for the result 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string& pycode , LoKi::Types::TrVCut& cuts )
      { return _get ( pycode , m_trvcuts  , cuts ) ; }
      // ========================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param cuts the placeholder for the result 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string& pycode , LoKi::Types::RVVCut& cuts ) 
      { return _get ( pycode , m_rvvcuts  , cuts ) ; }
      // ========================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param func the placeholder for the result 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string& pycode , LoKi::Types::TrFun& func ) 
      { return _get ( pycode , m_trfunc  , func ) ; }
      // ========================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param func the placeholder for the result 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string& pycode , LoKi::Types::RVFun&  func ) 
      { return _get ( pycode , m_rvfunc  , func ) ; }
      // ========================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param func the placeholder for the result 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string& pycode , LoKi::Types::TTrFun& func ) 
      { return _get ( pycode , m_ttrfunc  , func ) ; }
      // ========================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param func the placeholder for the result 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string& pycode , LoKi::Types::TrVFun& func )
      { return _get ( pycode , m_trvfunc  , func ) ; }
      // ========================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param func the placeholder for the result 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string& pycode , LoKi::Types::RVVFun& func ) 
      { return _get ( pycode , m_rvvfunc  , func ) ; }
      // ========================================================================
    public:
      // ========================================================================
      /** set the pointer to some reference container of tracks 
       *  @param tracks ponter to the reference container of tracks 
       *  @param who    the client 
       *  @return Status Code 
       */
      StatusCode set 
      ( std::vector<LHCb::Track*>*     tracks   , 
        INamedInterface*               who      ) ;
      // ========================================================================
      /** set the pointer to some reference container of reconstructed vertices 
       *  @param vertices ponter to the reference container of reocntructed vertices 
       *  @param who    the client 
       *  @return Status Code 
       */
      StatusCode set 
      ( std::vector<LHCb::RecVertex*>* vertices , 
        INamedInterface*               who      ) ;
      // ========================================================================
    public:
      /// set the C++ predicate for LHCb::Track
      virtual void set ( const LoKi::Types::TrCuts&    cut ) 
      { LoKi::Hybrid::Base::_set ( m_trcuts   , cut ) ; }
      /// set the C++ predicate for LHCb::RecVertex 
      virtual void set ( const LoKi::Types::RVCuts&    cut ) 
      { LoKi::Hybrid::Base::_set ( m_rvcuts   , cut ) ; }
      /// set the C++ predicate for Track-Track pair 
      virtual void set ( const LoKi::Types::TTrCuts&   cut ) 
      { LoKi::Hybrid::Base::_set ( m_ttrcuts  , cut ) ; }
      /// set the C++ predicate for Track-Vertex pair
      virtual void set ( const LoKi::Types::TrVCuts&   cut ) 
      { LoKi::Hybrid::Base::_set ( m_trvcuts  , cut ) ; }
      /// set the C++ predicate for Vertex-Vertex pair
      virtual void set ( const LoKi::Types::RVVCuts&   cut ) 
      { LoKi::Hybrid::Base::_set ( m_rvvcuts  , cut ) ; }
      /// set the C++ function for LHCb::Track
      virtual void set ( const LoKi::Types::TrFunc&    fun ) 
      { LoKi::Hybrid::Base::_set ( m_trfunc  , fun ) ; }
      /// set the C++ function for LHCb::RecVertex 
      virtual void set ( const LoKi::Types::RVFunc&    fun ) 
      { LoKi::Hybrid::Base::_set ( m_rvfunc  , fun ) ; }
      /// set the C++ function for Track-Track pair 
      virtual void set ( const LoKi::Types::TTrFunc&   fun ) 
      { LoKi::Hybrid::Base::_set ( m_ttrfunc  , fun ) ; }
      /// set the C++ function for Track-Vertex pair
      virtual void set ( const LoKi::Types::TrVFunc&   fun ) 
      { LoKi::Hybrid::Base::_set ( m_trvfunc  , fun ) ; }
      /// set the C++ function for Vertex-Vertex pair
      virtual void set ( const LoKi::Types::RVVFunc&   fun ) 
      { LoKi::Hybrid::Base::_set ( m_rvvfunc  , fun ) ; }
    public:
      /// get the reference container of tracks   (needed from Trigger) 
      virtual std::vector<LHCb::Track*>*     tracks   () const ;
      /// get the reference container of vertices (needed for Trigger) 
      virtual std::vector<LHCb::RecVertex*>* vertices () const ;
    protected:
      /// constrcutor
      TrTool
      ( const std::string& type   ,
        const std::string& name   ,
        const IInterface*  parent ) ;
      /// destrcutor 
      virtual ~TrTool();
    private:
      // the default constructor is disabled 
      TrTool() ; ///< the default constructor is disabled     
      // the copy constructor is disabled 
      TrTool           ( const TrTool& )  ; ///< the copy constructor is disabled 
      // the assignement operator  is disabled 
      TrTool& operator=( const TrTool& )  ; ///< no assignement 
    private:
      /// helper method to sdave many lines:
      template <class TYPE1,class TYPE2>
      inline StatusCode _get 
      ( const std::string& pycode , 
        TYPE1*&            local  , 
        TYPE2&             output ) ;
    protected:
      // local holder of cuts 
      LoKi::Types::TrCuts*  m_trcuts  ;
      LoKi::Types::RVCuts*  m_rvcuts  ;
      LoKi::Types::TTrCuts* m_ttrcuts ;
      LoKi::Types::TrVCuts* m_trvcuts ;
      LoKi::Types::RVVCuts* m_rvvcuts ;
      //
      LoKi::Types::TrFunc*  m_trfunc  ;
      LoKi::Types::RVFunc*  m_rvfunc  ;
      LoKi::Types::TTrFunc* m_ttrfunc ;    
      LoKi::Types::TrVFunc* m_trvfunc ;    
      LoKi::Types::RVVFunc* m_rvvfunc ;    
      typedef std::vector<std::string> Modules ;
      Modules     m_modules ;
      std::string m_actor   ;
      typedef std::vector<std::string> Lines   ;
      Lines       m_lines   ;
      /// the reference  container of tracks 
      std::vector<LHCb::Track*>*      m_tracks    ;
      /// the reference  container of tracks 
      std::vector<LHCb::RecVertex*>*  m_vertices  ;
    } ;
  } // end of namespace Hybrid 
} // end of namespace LoKi
// ============================================================================
// helper method to save many lines:
// ============================================================================
template <class TYPE1,class TYPE2>
inline StatusCode LoKi::Hybrid::TrTool::_get 
( const std::string& pycode , 
  TYPE1*&            local  , 
  TYPE2&             output ) 
{
  // consistency check:
  const TYPE1* tmp = &output ;
  StatusCode sc = ( 0 != tmp ) ? StatusCode::SUCCESS : StatusCode::FAILURE ;
  // prepare the actual python code 
  std::string code = makeCode  ( m_modules , m_actor , pycode , m_lines ) ;
  // define the scope:
  LoKi::Hybrid::TrLock lock ( this ) ;   ///< ATTENTION: the scope is locked!!
  // clear the placeholder:
  if ( 0 != local ) { delete local ; local = 0 ; }
  // execute the code 
  sc = executeCode ( code ) ;
  if ( sc.isFailure() ) 
  { return Error ( "Error from LoKi::Hybrid::Base::executeCode"      ) ; } // RETURN 
  if ( 0 == local     ) 
  { return Error ( "Invalid object for the code '"+pycode+"'" ) ; } // RETURN 
  // assign the result 
  output = *local ;                                                // ASSIGN
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi::Hybrid,TrTool);
// ============================================================================
// Standard constructor
// ============================================================================
LoKi::Hybrid::TrTool::TrTool 
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : LoKi::Hybrid::Base ( type , name , parent )
    //
  , m_trcuts      ( 0 )
  , m_rvcuts      ( 0 )
  , m_ttrcuts     ( 0 )
  , m_trvcuts     ( 0 )
  , m_rvvcuts     ( 0 )
    //
  , m_trfunc      ( 0 )
  , m_rvfunc      ( 0 )
  , m_ttrfunc     ( 0 )
  , m_trvfunc     ( 0 )
  , m_rvvfunc     ( 0 )
    //
  , m_modules   (   ) 
  , m_actor     ( "LoKi.Hybrid.TrEngine()" ) 
  , m_lines     () 
    ///
  , m_tracks     ( 0 ) 
  , m_vertices   ( 0 )
{
  // 
  declareInterface<LoKi::ITrHybridTool>   ( this ) ;
  declareInterface<LoKi::ITrHybridFactory>( this ) ;
  //
  m_modules.push_back ( "LoKiTrigger.decorators"  ) ;
  declareProperty ( "Modules" , m_modules , "Python modules to be imported"          ) ;
  declareProperty ( "Actor"   , m_actor   , "The processing engine"                  ) ;
  declareProperty ( "Lines"   , m_lines   , "Additional Python lines to be executed" ) ;
} 
// ============================================================================
/// Destructor (virtual and protected) 
// ============================================================================
LoKi::Hybrid::TrTool::~TrTool() {}
// ============================================================================
// initialization of the tool 
// ============================================================================
StatusCode LoKi::Hybrid::TrTool::initialize () 
{
  StatusCode sc = LoKi::Hybrid::Base::initialize() ;
  if ( sc.isFailure() ) { return sc ; }  
  m_lines.insert ( m_lines.begin() , "_HltTracks   = _actor.tracks   () " ) ;
  m_lines.insert ( m_lines.begin() , "_HltVertices = _actor.vertices () " ) ;
  if ( msgLevel ( MSG::DEBUG ) ) 
  { m_lines.insert( m_lines.end() , "print dir()" ) ; }
  return StatusCode::SUCCESS ;
} 
// ============================================================================
// finalization of the tool 
// ============================================================================
StatusCode LoKi::Hybrid::TrTool::finalize  () 
{
  //
  if ( 0 != m_trcuts   ) { delete m_trcuts   ; m_trcuts   = 0  ; }
  if ( 0 != m_rvcuts   ) { delete m_rvcuts   ; m_rvcuts   = 0  ; }
  if ( 0 != m_ttrcuts  ) { delete m_ttrcuts  ; m_ttrcuts  = 0  ; }
  if ( 0 != m_trvcuts  ) { delete m_trvcuts  ; m_trvcuts  = 0  ; }
  if ( 0 != m_rvvcuts  ) { delete m_rvvcuts  ; m_rvvcuts  = 0  ; }
  //
  if ( 0 != m_trfunc   ) { delete m_trfunc   ; m_trfunc   = 0  ; }
  if ( 0 != m_rvfunc   ) { delete m_rvfunc   ; m_rvfunc   = 0  ; }
  if ( 0 != m_ttrfunc  ) { delete m_ttrfunc  ; m_ttrfunc  = 0  ; }
  if ( 0 != m_trvfunc  ) { delete m_trvfunc  ; m_trvfunc  = 0  ; }
  if ( 0 != m_rvvfunc  ) { delete m_rvvfunc  ; m_rvvfunc  = 0  ; }
  // finalize the base 
  return LoKi::Hybrid::Base::finalize() ;
}
// ============================================================================
// get the reference container of tracks   (needed from Trigger) 
// ============================================================================
std::vector<LHCb::Track*>*     
LoKi::Hybrid::TrTool::tracks   () const 
{
  if ( 0 == m_tracks && msgLevel ( MSG::DEBUG ) )
  { Warning ( "tracks(): std::vector<LHCb::Track*>* points to NULL" ) ; }
  return m_tracks ;
}
// ============================================================================
// get the reference container of vertices (needed for Trigger) 
// ============================================================================
std::vector<LHCb::RecVertex*>* 
LoKi::Hybrid::TrTool::vertices () const 
{
  if ( 0 == m_vertices && msgLevel ( MSG::DEBUG ) )
  { Warning ( "vertices(): std::vector<LHCb::RecVertex*>* points to NULL" ) ; }
  return m_vertices ;
}
// ============================================================================
/*  set the pointer to some reference container of tracks 
 *  @param tracks ponter to the reference container of tracks 
 *  @param who    the client 
 *  @return Status Code 
 */
// ============================================================================
StatusCode 
LoKi::Hybrid::TrTool::set 
( std::vector<LHCb::Track*>*     tracks   , 
  INamedInterface*               who      ) 
{
  static const std::string s_unknown = "<unknown>" ;
  //
  const std::string& _who = ( 0 != who ) ? who->name() : s_unknown ;
  //
  if ( 0 == tracks && msgLevel ( MSG::DEBUG ) ) 
  { Warning ( "set(): std::vector<LHCb::Track*>* set to NULL by "+_who ).ignore() ; }
  //
  m_tracks = tracks ;
  //
  return StatusCode::SUCCESS ;
}
// ========================================================================
/*  set the pointer to some reference container of reconstructed vertices 
 *  @param vertices ponter to the reference container of reocntructed vertices 
 *  @param who    the client 
 *  @return Status Code 
 */
// ============================================================================
StatusCode 
LoKi::Hybrid::TrTool::set 
( std::vector<LHCb::RecVertex*>* vertices , 
  INamedInterface*               who      ) 
{
  static const std::string s_unknown = "<unknown>" ;
  //
  const std::string& _who = ( 0 != who ) ? who->name() : s_unknown ;
  //
  if ( 0 == vertices && msgLevel ( MSG::DEBUG ) ) 
  { Warning ( "set(): std::vector<LHCb::RecVertex*>* set to NULL by "+_who ).ignore() ; }
  //
  m_vertices = vertices ;
  //
  return StatusCode::SUCCESS ;
}// ========================================================================

// ============================================================================
// The END 
// ============================================================================
