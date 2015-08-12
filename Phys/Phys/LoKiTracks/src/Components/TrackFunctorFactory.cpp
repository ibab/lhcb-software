// $Id$
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/System.h" 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/HybridBase.h"
#include "LoKi/TrackFactoryLock.h"
#include "LoKi/ITrackFunctorFactory.h"
#include "LoKi/ITrackFunctorAntiFactory.h"
// ============================================================================
/** @file 
 *
 *  definition and implementation file for class LoKi::Hybrid::TrackFunctorFactory
 *  
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2004-06-29
 *
 *  $Revision$
 *  Last Modification $Date$ 
 *                 by $Author$ 
 *
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Hybrid 
  {
    // ========================================================================
    /** @class TrackFunctorFactory
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
    class TrackFunctorFactory 
    // : public virtual extends2< LoKi::Hybrid::Base             ,
    // LoKi::ITrackFunctorFactory     ,
    // LoKi::ITrackFunctorAntiFactory >
      : public virtual LoKi::Hybrid::Base         
      , public virtual LoKi::ITrackFunctorFactory 
      , public virtual LoKi::ITrackFunctorAntiFactory 
    {
      // ======================================================================
      // friend factory for instantiation
      friend class ToolFactory<LoKi::Hybrid::TrackFunctorFactory> ;
      // ======================================================================
    public:
      // ======================================================================
      /// initialization of the tool 
      virtual StatusCode initialize () ;
      /// finalization   of the tool 
      virtual StatusCode finalize   () ;
      // ======================================================================
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
      ( const std::string&     pycode  , 
        LoKi::Types::TrCutVal& func    , 
        const std::string&     context ) 
      { return _get ( pycode , m_trCutVals  , func , context ) ; }
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
    public: // "Anti-Factory"
      // ========================================================================
      /// set the C++ predicate for LHCb::Track
      virtual void set ( const LoKi::Types::TrCuts&    cut ) 
      { LoKi::Hybrid::Base::_set ( m_trcuts   , cut ) ; }
      /// set the C++ function for LHCb::Track
      virtual void set ( const LoKi::Types::TrFunc&    fun ) 
      { LoKi::Hybrid::Base::_set ( m_trfunc  , fun ) ; }
      // ======================================================================
    public: // "Anti-factory": functional part for Tracks 
      // ======================================================================
      /// set the C++ "maps"     for Tracks 
      virtual void set ( const LoKi::Types::TrMaps&     fun ) 
      { LoKi::Hybrid::Base::_set ( m_trMaps      , fun ) ; }
      /// set the C++ "pipes"    for Tracks 
      virtual void set ( const LoKi::Types::TrPipes&    fun ) 
      { LoKi::Hybrid::Base::_set ( m_trPipes     , fun ) ; }
      /// set the C++ "funvals"  for Tracks 
      virtual void set ( const LoKi::Types::TrFunVals&  fun ) 
      { LoKi::Hybrid::Base::_set ( m_trFunVals   , fun ) ; }
      /// set the C++ "cutvals"  for Tracks 
      virtual void set ( const LoKi::Types::TrCutVals&  fun ) 
      { LoKi::Hybrid::Base::_set ( m_trCutVals   , fun ) ; }
      /// set the C++ "sources"  for Tracks 
      virtual void set ( const LoKi::Types::TrSources&  fun ) 
      { LoKi::Hybrid::Base::_set ( m_trSources   , fun ) ; }
      // ======================================================================
    protected:
      // ======================================================================
      /// constrcutor
      TrackFunctorFactory
      ( const std::string& type   ,
        const std::string& name   ,
        const IInterface*  parent ) ;
      /// destrcutor 
      virtual ~TrackFunctorFactory();
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      TrackFunctorFactory () ; //the default constructor is disabled     
      /// the copy constructor is disabled 
      TrackFunctorFactory           ( const TrackFunctorFactory& )  ;
      /// the assignement operator  is disabled 
      TrackFunctorFactory& operator=( const TrackFunctorFactory& )  ; 
      // ======================================================================
    private:
      // ======================================================================
      /// helper method to save many lines:
      template <class TYPE1,class TYPE2>
      inline StatusCode _get 
      ( const std::string&                                            pycode  , 
        LoKi::Functor<TYPE1,TYPE2>*&                                  local   , 
        typename LoKi::Assignable<LoKi::Functor<TYPE1,TYPE2> >::Type& output  , 
        const std::string&                                            context ) ;
      // ======================================================================
    protected:
      // ======================================================================
      // local holder of cuts 
      LoKi::Types::TrCuts*     m_trcuts     ;
      LoKi::Types::TrFunc*     m_trfunc     ;
      // functional parts for LHCb::Tracks
      LoKi::Types::TrMaps*     m_trMaps     ;
      LoKi::Types::TrPipes*    m_trPipes    ;
      LoKi::Types::TrFunVals*  m_trFunVals  ;
      LoKi::Types::TrCutVals*  m_trCutVals  ;
      LoKi::Types::TrSources*  m_trSources  ;
      //
      typedef std::vector<std::string> Modules ;
      Modules     m_modules ;
      std::string m_actor   ;
      typedef std::vector<std::string> Lines   ;
      Lines       m_lines   ;
      // ======================================================================
    } ;
    // ========================================================================
  } //                                            end of namespace LoKi::Hybrid 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// helper method to save many lines:
// ============================================================================
template <class TYPE1,class TYPE2>
inline StatusCode LoKi::Hybrid::TrackFunctorFactory::_get 
( const std::string&                                            pycode  , 
  LoKi::Functor<TYPE1,TYPE2>*&                                  local   , 
  typename LoKi::Assignable<LoKi::Functor<TYPE1,TYPE2> >::Type& output  , 
  const std::string&                                            context ) 
{
  // ==========================================================================
  // consistency check:
  const LoKi::Functor<TYPE1,TYPE2>* tmp = &output ;
  StatusCode sc = ( 0 != tmp ) ? StatusCode::SUCCESS : StatusCode::FAILURE ;
  // prepare the actual python code 
  std::string code = 
    makeCode  ( m_modules , m_actor , pycode , m_lines , context ) ;
  // define the scope:  ATTENTION: the scope is locked!!
  LoKi::Hybrid::TrackFactoryLock lock ( this ) ; 
  //
  // move it into base class ???
  //
  // // clear the placeholder:
  // if ( 0 != local ) { delete local ; local = 0 ; }
  // // execute the code 
  // sc = executeCode ( code ) ;
  // if ( sc.isFailure() ) 
  // { return Error ( "Error from LoKi::Hybrid::Base::executeCode"      ) ; } // RETURN 
  // if ( 0 == local     ) 
  // { return Error ( "Invalid object for the code '" + pycode + "'"    ) ; } // RETURN 
  // // assign the result 
  // output = *local ;                                                        // ASSIGN
  //
  //
  // use the base class method 
  sc = LoKi::Hybrid::Base::_get_ ( code , local , output ) ;
  if ( sc.isFailure() )
  { return Error ( "Invalid object for the code '" + pycode + "'"    ) ; } // RETURN
  //
  //
  return StatusCode::SUCCESS ;
  // =========================================================================
}
// ============================================================================
// Standard constructor
// ============================================================================
LoKi::Hybrid::TrackFunctorFactory::TrackFunctorFactory 
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
// : base_class ( type , name , parent )
  : LoKi::Hybrid::Base( type , name , parent )
//
  , m_trcuts      ( 0 )
//
  , m_trfunc      ( 0 )
  //
  , m_trMaps      ( 0 ) 
  , m_trPipes     ( 0 ) 
  , m_trFunVals   ( 0 ) 
  , m_trCutVals   ( 0 ) 
  , m_trSources   ( 0 )
//
  , m_modules   (   ) 
  , m_actor     ( "LoKi.Hybrid.TrackEngine()" ) 
  , m_lines     () 
///
{
  declareInterface<LoKi::ITrackFunctorFactory>     (this) ;
  declareInterface<LoKi::ITrackFunctorAntiFactory> (this) ;  
  //
  m_modules.push_back ( "LoKiTracks.decorators"  ) ;
  m_modules.push_back ( "LoKiCore.math"          ) ;
  m_modules.push_back ( "LoKiCore.functions"     ) ;
  //
  declareProperty 
    ( "Modules" , 
      m_modules , 
      "Python modules to be imported"            ) ;
  declareProperty 
    ( "Actor"   , 
      m_actor   , 
      "The processing engine"                    ) ;
  declareProperty 
    ( "Lines"   , 
      m_lines   , 
      "Additional Python lines to be executed"   ) ;
  // ==========================================================================
  // C++
  // ==========================================================================
  m_cpplines.push_back ( "#include \"LoKi/LoKiTracks.h\"" ) ;
  // ==========================================================================
} 
// ============================================================================
/// Destructor (virtual and protected) 
// ============================================================================
LoKi::Hybrid::TrackFunctorFactory::~TrackFunctorFactory() {}
// ============================================================================
// initialization of the tool 
// ============================================================================
StatusCode LoKi::Hybrid::TrackFunctorFactory::initialize () 
{
  //
  StatusCode sc = LoKi::Hybrid::Base::initialize() ;
  if ( sc.isFailure() ) { return sc ; }  
  //
  if ( msgLevel ( MSG::DEBUG ) ) 
  { m_lines.insert ( m_lines.end() , "print dir()" ) ; }
  //
  return StatusCode::SUCCESS ;
} 
// ============================================================================
// finalization of the tool 
// ============================================================================
StatusCode LoKi::Hybrid::TrackFunctorFactory::finalize  () 
{
  //
  if ( 0 != m_trcuts     ) { delete m_trcuts     ; m_trcuts     = 0  ; }
  if ( 0 != m_trfunc     ) { delete m_trfunc     ; m_trfunc     = 0  ; }
  //
  if ( 0 != m_trMaps     ) { delete m_trMaps     ; m_trMaps     = 0  ; }
  if ( 0 != m_trPipes    ) { delete m_trPipes    ; m_trPipes    = 0  ; }
  if ( 0 != m_trFunVals  ) { delete m_trFunVals  ; m_trFunVals  = 0  ; }
  if ( 0 != m_trCutVals  ) { delete m_trCutVals  ; m_trCutVals  = 0  ; }
  if ( 0 != m_trSources  ) { delete m_trSources  ; m_trSources  = 0  ; }
  //  
  // finalize the base 
  return LoKi::Hybrid::Base::finalize() ;
}
// ============================================================================
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi::Hybrid,TrackFunctorFactory)
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
