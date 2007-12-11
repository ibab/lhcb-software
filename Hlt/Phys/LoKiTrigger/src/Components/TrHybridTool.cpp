// $Id: TrHybridTool.cpp,v 1.3 2007-12-11 19:01:40 ibelyaev Exp $
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
       *  @param context the context lines to be executed 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string&   pycode  , 
        LoKi::Types::TrCut&  cuts    , 
        const std::string&   context ) 
      { return _get ( pycode , m_trcuts  , cuts , context ) ; }
      // ========================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param cuts the placeholder for the result 
       *  @param context the context lines to be executed 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string&   pycode  ,
        LoKi::Types::RVCut&  cuts    ,
        const std::string&   context ) 
      { return _get ( pycode , m_rvcuts  , cuts , context ) ; }
      // ========================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param cuts the placeholder for the result 
       *  @param context the context lines to be executed 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string&   pycode  , 
        LoKi::Types::TTrCut& cuts    ,
        const std::string&   context ) 
      { return _get ( pycode , m_ttrcuts  , cuts , context ) ; }
      // ========================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param cuts the placeholder for the result 
       *  @param context the context lines to be executed 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string&   pycode  , 
        LoKi::Types::TrVCut& cuts    ,
        const std::string&   context )
      { return _get ( pycode , m_trvcuts  , cuts , context ) ; }
      // ========================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param cuts the placeholder for the result 
       *  @param context the context lines to be executed 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string&   pycode  , 
        LoKi::Types::RVVCut& cuts    , 
        const std::string&   context )
      { return _get ( pycode , m_rvvcuts  , cuts , context ) ; }
      // ========================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param func the placeholder for the result 
       *  @param context the context lines to be executed 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string&   pycode  ,
        LoKi::Types::TrFun&  func    ,
        const std::string&   context )
      { return _get ( pycode , m_trfunc  , func , context ) ; }
      // ========================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param func the placeholder for the result 
       *  @param context the context lines to be executed 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string&   pycode  , 
        LoKi::Types::RVFun&  func    ,
        const std::string&   context )
      { return _get ( pycode , m_rvfunc  , func , context ) ; }
      // ========================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param func the placeholder for the result 
       *  @param context the context lines to be executed 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string&   pycode  , 
        LoKi::Types::TTrFun& func    ,
        const std::string&   context ) 
      { return _get ( pycode , m_ttrfunc  , func , context ) ; }
      // ========================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param func the placeholder for the result 
       *  @param context the context lines to be executed 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string&   pycode  , 
        LoKi::Types::TrVFun& func    ,
        const std::string&   context ) 
      { return _get ( pycode , m_trvfunc  , func , context ) ; }
      // ========================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param func the placeholder for the result 
       *  @param context the context lines to be executed 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string&   pycode  , 
        LoKi::Types::RVVFun& func    , 
        const std::string&   context ) 
      { return _get ( pycode , m_rvvfunc  , func , context ) ; }
      // ========================================================================
    public:
      // ========================================================================
      // functional part for LHCb::Track
      // ========================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param func the placeholder for the result 
       *  @param context the context lines to be executed 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string&   pycode  , 
        LoKi::Types::TrMap&  func    , 
        const std::string&   context )
      { return _get ( pycode , m_trMaps  , func , context ) ; }
      // ========================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param func the placeholder for the result 
       *  @param context the context lines to be executed 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string&   pycode  , 
        LoKi::Types::TrPipe& func    , 
        const std::string&   context ) 
      { return _get ( pycode , m_trPipes  , func , context ) ; }
      // ========================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param func the placeholder for the result 
       *  @param context the context lines to be executed 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string&     pycode  , 
        LoKi::Types::TrFunVal& func    , 
        const std::string&     context ) 
      { return _get ( pycode , m_trFunVals  , func , context ) ; }
      // ========================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param func the placeholder for the result 
       *  @param context the context lines to be executed 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string&      pycode  , 
        LoKi::Types::TrElement& func    , 
        const std::string&      context ) 
      { return _get ( pycode , m_trElements  , func , context ) ; }
      // ========================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param func the placeholder for the result 
       *  @param context the context lines to be executed 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string&     pycode  , 
        LoKi::Types::TrSource& func    , 
        const std::string&     context )
      { return _get ( pycode , m_trSources  , func , context ) ; }
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
    public: // functional part for Tracks 
      /// set the C++ "maps"     for Tracks 
      virtual void set ( const LoKi::Types::TrMaps&     fun ) 
      { LoKi::Hybrid::Base::_set ( m_trMaps      , fun ) ; }
      /// set the C++ "pipes"    for Tracks 
      virtual void set ( const LoKi::Types::TrPipes&    fun ) 
      { LoKi::Hybrid::Base::_set ( m_trPipes     , fun ) ; }
      /// set the C++ "funvals"  for Tracks 
      virtual void set ( const LoKi::Types::TrFunVals&  fun ) 
      { LoKi::Hybrid::Base::_set ( m_trFunVals   , fun ) ; }
      /// set the C++ "elements" for Tracks 
      virtual void set ( const LoKi::Types::TrElements& fun ) 
      { LoKi::Hybrid::Base::_set ( m_trElements  , fun ) ; }
      /// set the C++ "sources"  for Tracks 
      virtual void set ( const LoKi::Types::TrSources&  fun ) 
      { LoKi::Hybrid::Base::_set ( m_trSources   , fun ) ; }
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
      /// helper method to save many lines:
      template <class TYPE1,class TYPE2>
      inline StatusCode _get 
      ( const std::string& pycode  , 
        TYPE1*&            local   , 
        TYPE2&             output  , 
        const std::string& context ) ;
    protected:
      // local holder of cuts 
      LoKi::Types::TrCuts*     m_trcuts     ;
      LoKi::Types::RVCuts*     m_rvcuts     ;
      LoKi::Types::TTrCuts*    m_ttrcuts    ;
      LoKi::Types::TrVCuts*    m_trvcuts    ;
      LoKi::Types::RVVCuts*    m_rvvcuts    ;
      //
      LoKi::Types::TrFunc*     m_trfunc     ;
      LoKi::Types::RVFunc*     m_rvfunc     ;
      LoKi::Types::TTrFunc*    m_ttrfunc    ;    
      LoKi::Types::TrVFunc*    m_trvfunc    ;    
      LoKi::Types::RVVFunc*    m_rvvfunc    ;    
      // functional parts for LHCb::Tracks
      LoKi::Types::TrMaps*     m_trMaps     ;
      LoKi::Types::TrPipes*    m_trPipes    ;
      LoKi::Types::TrFunVals*  m_trFunVals  ;
      LoKi::Types::TrElements* m_trElements ;
      LoKi::Types::TrSources*  m_trSources  ;
      //
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
( const std::string&    pycode  , 
  TYPE1*&               local   , 
  TYPE2&                output  , 
  const std::string&    context ) 
{
  // consistency check:
  const TYPE1* tmp = &output ;
  StatusCode sc = ( 0 != tmp ) ? StatusCode::SUCCESS : StatusCode::FAILURE ;
  // prepare the actual python code 
  std::string code = 
    makeCode  ( m_modules , m_actor , pycode , m_lines , context ) ;
  // define the scope:
  LoKi::Hybrid::TrLock lock ( this ) ;   ///< ATTENTION: the scope is locked!!
  // clear the placeholder:
  if ( 0 != local ) { delete local ; local = 0 ; }
  // execute the code 
  sc = executeCode ( code ) ;
  if ( sc.isFailure() ) 
  { return Error ( "Error from LoKi::Hybrid::Base::executeCode"      ) ; } // RETURN 
  if ( 0 == local     ) 
  { return Error ( "Invalid object for the code '" + pycode + "'" ) ; } // RETURN 
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
  , m_trMaps      ( 0 ) 
  , m_trPipes     ( 0 ) 
  , m_trFunVals   ( 0 ) 
  , m_trElements  ( 0 ) 
  , m_trSources   ( 0 )
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
  declareProperty 
    ( "Modules" , m_modules , "Python modules to be imported"          ) ;
  declareProperty 
    ( "Actor"   , m_actor   , "The processing engine"                  ) ;
  declareProperty 
    ( "Lines"   , m_lines   , "Additional Python lines to be executed" ) ;
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
  { m_lines.insert ( m_lines.end() , "print dir()" ) ; }
  return StatusCode::SUCCESS ;
} 
// ============================================================================
// finalization of the tool 
// ============================================================================
StatusCode LoKi::Hybrid::TrTool::finalize  () 
{
  //
  if ( 0 != m_trcuts     ) { delete m_trcuts     ; m_trcuts     = 0  ; }
  if ( 0 != m_rvcuts     ) { delete m_rvcuts     ; m_rvcuts     = 0  ; }
  if ( 0 != m_ttrcuts    ) { delete m_ttrcuts    ; m_ttrcuts    = 0  ; }
  if ( 0 != m_trvcuts    ) { delete m_trvcuts    ; m_trvcuts    = 0  ; }
  if ( 0 != m_rvvcuts    ) { delete m_rvvcuts    ; m_rvvcuts    = 0  ; }
  //
  if ( 0 != m_trfunc     ) { delete m_trfunc     ; m_trfunc     = 0  ; }
  if ( 0 != m_rvfunc     ) { delete m_rvfunc     ; m_rvfunc     = 0  ; }
  if ( 0 != m_ttrfunc    ) { delete m_ttrfunc    ; m_ttrfunc    = 0  ; }
  if ( 0 != m_trvfunc    ) { delete m_trvfunc    ; m_trvfunc    = 0  ; }
  if ( 0 != m_rvvfunc    ) { delete m_rvvfunc    ; m_rvvfunc    = 0  ; }
  //
  if ( 0 != m_trMaps     ) { delete m_trMaps     ; m_trMaps     = 0  ; }
  if ( 0 != m_trPipes    ) { delete m_trPipes    ; m_trPipes    = 0  ; }
  if ( 0 != m_trFunVals  ) { delete m_trFunVals  ; m_trFunVals  = 0  ; }
  if ( 0 != m_trElements ) { delete m_trElements ; m_trElements = 0  ; }
  if ( 0 != m_trSources  ) { delete m_trSources  ; m_trSources  = 0  ; }
  //  
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
  { Warning ( "set(): std::vector<LHCb::Track*>* set to NULL by "+_who ) ; }
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
  { Warning ( "set(): std::vector<LHCb::RecVertex*>* set to NULL by "+_who ) ; }
  //
  m_vertices = vertices ;
  //
  return StatusCode::SUCCESS ;
} 
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
