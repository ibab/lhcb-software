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
#include "LoKi/HybridLock.h"
#include "LoKi/IHybridTool.h"
#include "LoKi/IHybridFactory.h"
// ============================================================================
/** @file
 *  definition and implementation file for class LoKi::HybridTool
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
 */
namespace LoKi
{
  // ==========================================================================
  namespace Hybrid
  {
    // ========================================================================
    /** @class Tool
     *
     *  Concrete impelmentation of LoKi::IHybridTool interface
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
     *  @date   2004-06-29
     */
    class Tool : public extends2< LoKi::Hybrid::Base   ,
                                  LoKi::IHybridFactory ,
                                  LoKi::IHybridTool    >
    {
      // ======================================================================
      // friend factory forn instantiation
      friend class ToolFactory<LoKi::Hybrid::Tool> ;
      // ======================================================================
    public:
      // ======================================================================
      /// initialization of the tool
      virtual StatusCode initialize () ;
      /// finalization   of the tool
      virtual StatusCode finalize  () ;
      // ======================================================================
      /// the update handler
      void propHandler ( Property& p  )
      {
        //
        if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
        //
        debug() << "Property is updated: " << p << endmsg ;
        //
      }
      // ======================================================================
    public:
      // ======================================================================
      // predicates
      // ======================================================================
      /** "Factory": get the the object form python code
       *  @param pycode the python pseudo-code of the function
       *  @param cuts placeholder for the result
       *  @param context the context lines to be executed
       *  @return StatusCode
       */
      virtual StatusCode get
      ( const std::string& pycode  ,
        LoKi::Types::Cut&  cuts    ,
        const std::string& context )
      { return _get ( pycode , m_cuts  , cuts , context ) ; }
      // ======================================================================
      /** "Factory": get the the object form python code
       *  @param pycode the python pseudo-code of the function
       *  @param cuts placeholder for the  result
       *  @param context the context lines to be executed
       *  @return StatusCode
       */
      virtual StatusCode get
      ( const std::string& pycode  ,
        LoKi::Types::VCut& cuts    ,
        const std::string& context )
      { return _get ( pycode , m_vcuts , cuts , context  ) ; }
      // ======================================================================
      /** "Factory": get the the object form python code
       *  @param pycode the python pseudo-code of the function
       *  @param cuts placeholder for the result
       *  @param context the context lines to be executed
       *  @return StatusCode
       */
      virtual StatusCode get
      ( const std::string& pycode  ,
        LoKi::Types::ACut& cuts    ,
        const std::string& context )
      { return _get ( pycode , m_acuts , cuts , context ) ; }
      // ======================================================================
      /** "Factory": get the the object form python code
       *  @param pycode the python pseudo-code of the function
       *  @param cuts placeholder for the result
       *  @param context the context lines to be executed
       *  @return StatusCode
       */
      virtual StatusCode get
      ( const std::string&  pycode  ,
        LoKi::Types::PPCut& cuts    ,
        const std::string&  context )
      { return _get ( pycode , m_ppcuts , cuts , context ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // functions
      // ======================================================================
      /** "Factory": get the the object form python code
       *  @param pycode the python pseudo-code of the function
       *  @param func placehoplder for the result
       *  @param context the context lines to be executed
       *  @return StatusCode
       */
      virtual StatusCode get
      ( const std::string& pycode  ,
        LoKi::Types::Fun& func     ,
        const std::string& context )
      { return _get ( pycode , m_func  , func , context ) ; }
      // ======================================================================
      /** "Factory": get the the object form python code
       *  @param pycode the python pseudo-code of the function
       *  @param func placeholder for the result
       *  @param context the context lines to be executed
       *  @return StatusCode
       */
      virtual StatusCode get
      ( const std::string& pycode  ,
        LoKi::Types::VFun& func    ,
        const std::string& context )
      { return _get ( pycode , m_vfunc , func , context ) ; }
      // ======================================================================
      /** "Factory": get the the object form python code
       *  @param pycode the python pseudo-code of the function
       *  @param func placeholder for the result
       *  @param context the context lines to be executed
       *  @return StatusCode
       */
      virtual StatusCode get
      ( const std::string& pycode  ,
        LoKi::Types::AFun& func    ,
        const std::string& context )
      { return _get ( pycode , m_afunc , func , context ) ; }
      // ======================================================================
      /** "Factory": get the the object form python code
       *  @param pycode the python pseudo-code of the function
       *  @param func placeholder for the result
       *  @param context the context lines to be executed
       *  @return StatusCode
       */
      virtual StatusCode get
      ( const std::string&  pycode  ,
        LoKi::Types::PPFun& func    ,
        const std::string&  context )
      { return _get ( pycode , m_ppfunc , func , context ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // "maps"
      // ======================================================================
      /** "Factory": get the the object from python code
       *  @param pycode the python pseudo-code of the function
       *  @param func the placeholder for the result
       *  @param context the context lines to be executed
       *  @return StatusCode
       */
      virtual StatusCode get
      ( const std::string& pycode  ,
        LoKi::Types::Map&  func    ,
        const std::string& context )
      { return _get ( pycode , m_maps , func , context ) ; }
      // ======================================================================
      /** "Factory": get the the object form python code
       *  @param pycode the python pseudo-code of the function
       *  @param func the placeholder for the result
       *  @param context the context lines to be executed
       *  @return StatusCode
       */
      virtual StatusCode get
      ( const std::string& pycode  ,
        LoKi::Types::VMap& func    ,
        const std::string& context )
      { return _get ( pycode , m_vmaps , func , context ) ; }
      // ======================================================================
      /** "Factory": get the the object form python code
       *  @param pycode the python pseudo-code of the function
       *  @param func the placeholder for the result
       *  @param context the context lines to be executed
       *  @return StatusCode
       */
      virtual StatusCode get
      ( const std::string&  pycode  ,
        LoKi::Types::PPMap& func    ,
        const std::string&  context )
      { return _get ( pycode , m_ppmaps , func , context ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // "pipes"
      // ======================================================================
      /** "Factory": get the the object from python code
       *  @param pycode the python pseudo-code of the function
       *  @param func the placeholder for the result
       *  @param context the context lines to be executed
       *  @return StatusCode
       */
      virtual StatusCode get
      ( const std::string& pycode  ,
        LoKi::Types::Pipe& func    ,
        const std::string& context )
      { return _get ( pycode , m_pipes , func , context ) ; }
      // ======================================================================
      /** "Factory": get the the object form python code
       *  @param pycode the python pseudo-code of the function
       *  @param func the placeholder for the result
       *  @param context the context lines to be executed
       *  @return StatusCode
       */
      virtual StatusCode get
      ( const std::string&  pycode  ,
        LoKi::Types::VPipe& func    ,
        const std::string&  context )
      { return _get ( pycode , m_vpipes , func , context ) ; }
      // ======================================================================
      /** "Factory": get the the object form python code
       *  @param pycode the python pseudo-code of the function
       *  @param func the placeholder for the result
       *  @param context the context lines to be executed
       *  @return StatusCode
       */
      virtual StatusCode get
      ( const std::string&   pycode  ,
        LoKi::Types::PPPipe& func    ,
        const std::string&   context )
      { return _get ( pycode , m_pppipes , func , context ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // "fun-vals"
      // ======================================================================
      /** "Factory": get the the object from python code
       *  @param pycode the python pseudo-code of the function
       *  @param func the placeholder for the result
       *  @param context the context lines to be executed
       *  @return StatusCode
       */
      virtual StatusCode get
      ( const std::string&   pycode  ,
        LoKi::Types::FunVal& func    ,
        const std::string&   context )
      { return _get ( pycode , m_funvals , func , context ) ; }
      // ======================================================================
      /** "Factory": get the the object form python code
       *  @param pycode the python pseudo-code of the function
       *  @param func the placeholder for the result
       *  @param context the context lines to be executed
       *  @return StatusCode
       */
      virtual StatusCode get
      ( const std::string&    pycode  ,
        LoKi::Types::VFunVal& func    ,
        const std::string&    context )
      { return _get ( pycode , m_vfunvals , func , context ) ; }
      // ======================================================================
      /** "Factory": get the the object form python code
       *  @param pycode the python pseudo-code of the function
       *  @param func the placeholder for the result
       *  @param context the context lines to be executed
       *  @return StatusCode
       */
      virtual StatusCode get
      ( const std::string&     pycode  ,
        LoKi::Types::PPFunVal& func    ,
        const std::string&     context )
      { return _get ( pycode , m_ppfunvals , func , context ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // "cut-vals"
      // ======================================================================
      /** "Factory": get the the object from python code
       *  @param pycode the python pseudo-code of the function
       *  @param func the placeholder for the result
       *  @param context the context lines to be executed
       *  @return StatusCode
       */
      virtual StatusCode get
      ( const std::string&   pycode  ,
        LoKi::Types::CutVal& func    ,
        const std::string&   context )
      { return _get ( pycode , m_cutvals , func , context ) ; }
      // ======================================================================
      /** "Factory": get the the object form python code
       *  @param pycode the python pseudo-code of the function
       *  @param func the placeholder for the result
       *  @param context the context lines to be executed
       *  @return StatusCode
       */
      virtual StatusCode get
      ( const std::string&    pycode  ,
        LoKi::Types::VCutVal& func    ,
        const std::string&    context )
      { return _get ( pycode , m_vcutvals , func , context ) ; }
      // ======================================================================
      /** "Factory": get the the object form python code
       *  @param pycode the python pseudo-code of the function
       *  @param func the placeholder for the result
       *  @param context the context lines to be executed
       *  @return StatusCode
       */
      virtual StatusCode get
      ( const std::string&     pycode  ,
        LoKi::Types::PPCutVal& func    ,
        const std::string&     context )
      { return _get ( pycode , m_ppcutvals , func , context ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // "sources"
      // ======================================================================
      /** "Factory": get the the object from python code
       *  @param pycode the python pseudo-code of the function
       *  @param func the placeholder for the result
       *  @param context the context lines to be executed
       *  @return StatusCode
       */
      virtual StatusCode get
      ( const std::string&    pycode  ,
        LoKi::Types::Source&  func    ,
        const std::string&    context )
      { return _get ( pycode , m_sources , func , context ) ; }
      // ======================================================================
      /** "Factory": get the the object form python code
       *  @param pycode the python pseudo-code of the function
       *  @param func the placeholder for the result
       *  @param context the context lines to be executed
       *  @return StatusCode
       */
      virtual StatusCode get
      ( const std::string&     pycode  ,
        LoKi::Types::VSource&  func    ,
        const std::string&     context )
      { return _get ( pycode , m_vsources , func , context ) ; }
      // ======================================================================
      /** "Factory": get the the object form python code
       *  @param pycode the python pseudo-code of the function
       *  @param func the placeholder for the result
       *  @param context the context lines to be executed
       *  @return StatusCode
       */
      virtual StatusCode get
      ( const std::string&     pycode  ,
        LoKi::Types::PPSource& func    ,
        const std::string&     context )
      { return _get ( pycode , m_ppsources , func , context ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // predicates:
      // ======================================================================
      /// set the C++ predicate for LHCb::Particle
      virtual void set ( const LoKi::Types::Cuts&    cut )
      { LoKi::Hybrid::Base::_set ( m_cuts  , cut ) ; }
      /// set the C++ predicate for LHCb::Vertex
      virtual void set ( const LoKi::Types::VCuts&   cut )
      { LoKi::Hybrid::Base::_set ( m_vcuts , cut ) ; }
      /// set the C++ predicate for array of particles
      virtual void set ( const LoKi::Types::ACuts&   cut )
      { LoKi::Hybrid::Base::_set ( m_acuts , cut ) ; }
      /// set the C++ predicate for LHCb::ProtoParticle
      virtual void set ( const LoKi::Types::PPCuts&  cut )
      { LoKi::Hybrid::Base::_set ( m_ppcuts , cut ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // functions:
      // ======================================================================
      /// set the C++ function for LHCb::Particle
      virtual void set ( const LoKi::Types::Func&    cut )
      { LoKi::Hybrid::Base::_set ( m_func  , cut ) ; }
      /// set the C++ function for LHCb::Vertex
      virtual void set ( const LoKi::Types::VFunc&   cut )
      { LoKi::Hybrid::Base::_set ( m_vfunc  , cut ) ; }
      /// set the C++ function for arary of particles
      virtual void set ( const LoKi::Types::AFunc&   cut )
      { LoKi::Hybrid::Base::_set ( m_afunc  , cut ) ; }
      /// set the C++ function for LHCb::ProtoParticle
      virtual void set ( const LoKi::Types::PPFunc&  cut )
      { LoKi::Hybrid::Base::_set ( m_ppfunc  , cut ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // maps:
      // ======================================================================
      /// set the C++ "map" for LHCb::Particle
      virtual void set ( const LoKi::Types::Maps&    cut )
      { LoKi::Hybrid::Base::_set ( m_maps  , cut ) ; }
      /// set the C++ "map" for LHCb::Vertex
      virtual void set ( const LoKi::Types::VMaps&   cut )
      { LoKi::Hybrid::Base::_set ( m_vmaps  , cut ) ; }
      /// set the C++ "map" for LHCb::ProtoParticle
      virtual void set ( const LoKi::Types::PPMaps&  cut )
      { LoKi::Hybrid::Base::_set ( m_ppmaps  , cut ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // pipes:
      // ======================================================================
      /// set the C++ "pipe" for LHCb::Particle
      virtual void set ( const LoKi::Types::Pipes&    cut )
      { LoKi::Hybrid::Base::_set ( m_pipes   , cut ) ; }
      /// set the C++ "pipe" for LHCb::Vertex
      virtual void set ( const LoKi::Types::VPipes&   cut )
      { LoKi::Hybrid::Base::_set ( m_vpipes  , cut ) ; }
      /// set the C++ "pipe" for LHCb::ProtoParticle
      virtual void set ( const LoKi::Types::PPPipes&  cut )
      { LoKi::Hybrid::Base::_set ( m_pppipes  , cut ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // fun-vals:
      // ======================================================================
      /// set the C++ "fun-val" for LHCb::Particle
      virtual void set ( const LoKi::Types::FunVals&    cut )
      { LoKi::Hybrid::Base::_set ( m_funvals   , cut ) ; }
      /// set the C++ "fun-val" for LHCb::Vertex
      virtual void set ( const LoKi::Types::VFunVals&   cut )
      { LoKi::Hybrid::Base::_set ( m_vfunvals  , cut ) ; }
      /// set the C++ "fun-val" for LHCb::ProtoParticle
      virtual void set ( const LoKi::Types::PPFunVals&  cut )
      { LoKi::Hybrid::Base::_set ( m_ppfunvals  , cut ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // cut-vals:
      // ======================================================================
      /// set the C++ "cut-val" for LHCb::Particle
      virtual void set ( const LoKi::Types::CutVals&    cut )
      { LoKi::Hybrid::Base::_set ( m_cutvals   , cut ) ; }
      /// set the C++ "cut-val" for LHCb::Vertex
      virtual void set ( const LoKi::Types::VCutVals&   cut )
      { LoKi::Hybrid::Base::_set ( m_vcutvals  , cut ) ; }
      /// set the C++ "cut-val" for LHCb::ProtoParticle
      virtual void set ( const LoKi::Types::PPCutVals&   cut )
      { LoKi::Hybrid::Base::_set ( m_ppcutvals  , cut ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // sources
      // ======================================================================
      /// set the C++ "source" for LHCb::Particle
      virtual void set ( const LoKi::Types::Sources&    cut )
      { LoKi::Hybrid::Base::_set ( m_sources   , cut ) ; }
      /// set the C++ "source" for LHCb::Vertex
      virtual void set ( const LoKi::Types::VSources&   cut )
      { LoKi::Hybrid::Base::_set ( m_vsources  , cut ) ; }
      /// set the C++ "source" for LHCb::ProtoParticle
      virtual void set ( const LoKi::Types::PPSources&  cut )
      { LoKi::Hybrid::Base::_set ( m_ppsources  , cut ) ; }
      // ======================================================================
    protected:
      // ======================================================================
      /// constrcutor
      Tool
      ( const std::string& type   ,
        const std::string& name   ,
        const IInterface*  parent ) ;
      /// destrcutor
      virtual ~Tool();
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled
      Tool() ;                           // the default constructor is disabled
      /// the copy constructor is disabled
      Tool           ( const Tool& )  ;     // the copy constructor is disabled
      /// the assignement operator  is disabled
      Tool& operator=( const Tool& )  ;                       // no assignement
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
      // predicates:
      LoKi::Types::Cuts*       m_cuts       ;
      LoKi::Types::VCuts*      m_vcuts      ;
      LoKi::Types::ACuts*      m_acuts      ;
      LoKi::Types::PPCuts*     m_ppcuts     ;
      // functions:
      LoKi::Types::Func*       m_func       ;
      LoKi::Types::VFunc*      m_vfunc      ;
      LoKi::Types::AFunc*      m_afunc      ;
      LoKi::Types::PPFunc*     m_ppfunc     ;
      // maps:
      LoKi::Types::Maps*       m_maps       ;
      LoKi::Types::VMaps*      m_vmaps      ;
      LoKi::Types::PPMaps*     m_ppmaps     ;
      // pipes:
      LoKi::Types::Pipes*      m_pipes      ;
      LoKi::Types::VPipes*     m_vpipes     ;
      LoKi::Types::PPPipes*    m_pppipes    ;
      // fun-vals:
      LoKi::Types::FunVals*    m_funvals    ;
      LoKi::Types::VFunVals*   m_vfunvals   ;
      LoKi::Types::PPFunVals*  m_ppfunvals  ;
      // cut-vals:
      LoKi::Types::CutVals*    m_cutvals    ;
      LoKi::Types::VCutVals*   m_vcutvals   ;
      LoKi::Types::PPCutVals*  m_ppcutvals  ;
      // sources:
      LoKi::Types::Sources*    m_sources    ;
      LoKi::Types::VSources*   m_vsources   ;
      LoKi::Types::PPSources*  m_ppsources  ;
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
// helper method to sdave many lines:
// ============================================================================
template <class TYPE1,class TYPE2>
inline StatusCode LoKi::Hybrid::Tool::_get
( const std::string&                                            pycode  ,
  LoKi::Functor<TYPE1,TYPE2>*&                                  local   , 
  typename LoKi::Assignable<LoKi::Functor<TYPE1,TYPE2> >::Type& output  ,
  const std::string&                                            context )
{
  // prepare the actual python code
  std::string code = makeCode
    ( m_modules , m_actor , pycode , m_lines , context ) ;
  // define the scope:
  LoKi::Hybrid::Lock lock ( this ) ;   ///< ATTENTION: the scope is locked!!
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
  StatusCode sc = LoKi::Hybrid::Base::_get_ ( code , local , output ) ;
  if ( sc.isFailure() )
  { return Error ( "Invalid object for the code '" + pycode + "'"    ) ; } // RETURN
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// Standard constructor
// ============================================================================
LoKi::Hybrid::Tool::Tool
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : base_class ( type , name , parent )
    // predicates:
  , m_cuts       ( 0 )
  , m_vcuts      ( 0 )
  , m_acuts      ( 0 )
  , m_ppcuts     ( 0 )
    // functions:
  , m_func       ( 0 )
  , m_vfunc      ( 0 )
  , m_afunc      ( 0 )
  , m_ppfunc     ( 0 )
    // "maps":
  , m_maps       ( 0 )
  , m_vmaps      ( 0 )
  , m_ppmaps     ( 0 )
    // "pipes":
  , m_pipes      ( 0 )
  , m_vpipes     ( 0 )
  , m_pppipes    ( 0 )
    // "fun-vals":
  , m_funvals    ( 0 )
  , m_vfunvals   ( 0 )
  , m_ppfunvals  ( 0 )
    // "cut-vals":
  , m_cutvals    ( 0 )
  , m_vcutvals   ( 0 )
  , m_ppcutvals  ( 0 )
    // "sources":
  , m_sources    ( 0 )
  , m_vsources   ( 0 )
  , m_ppsources  ( 0 )
    //
  , m_modules    (   )
  , m_actor      ( "LoKi.Hybrid.Engine()" )
  , m_lines      (   )
{
  //
  m_modules.push_back ( "LoKiPhys.decorators"           ) ;
  m_modules.push_back ( "LoKiProtoParticles.decorators" ) ;
  m_modules.push_back ( "LoKiArrayFunctors.decorators"  ) ;
  m_modules.push_back ( "LoKiCore.functions"            ) ;
  m_modules.push_back ( "LoKiCore.math"                 ) ;
  //
  declareProperty
    ( "Modules" ,
      m_modules ,
      "Python modules to be imported" )
    -> declareUpdateHandler
    ( &LoKi::Hybrid::Tool::propHandler , this ) ;
  declareProperty
    ( "Actor"   ,
      m_actor   ,
      "The processing engine"         )
    -> declareUpdateHandler
    ( &LoKi::Hybrid::Tool::propHandler , this ) ;
  declareProperty
    ( "Lines"   ,
      m_lines   ,
      "Additional python lines to be used in Bender/Pythoni script" )
    -> declareUpdateHandler
    ( &LoKi::Hybrid::Tool::propHandler , this ) ;
  //
  // ==========================================================================
  // C++
  // ==========================================================================
  m_cpplines.push_back ( "#include \"LoKi/LoKiTracks.h\""         ) ;
  m_cpplines.push_back ( "#include \"LoKi/LoKiProtoParticles.h\"" ) ;
  m_cpplines.push_back ( "#include \"LoKi/LoKiPhys.h\""           ) ;
  m_cpplines.push_back ( "#include \"LoKi/LoKiArrayFunctors.h\""  ) ;
  // ==========================================================================
}
// ============================================================================
// Destructor (virtual and protected)
// ============================================================================
LoKi::Hybrid::Tool::~Tool() {}
// ============================================================================
// initialization of the tool
// ============================================================================
StatusCode LoKi::Hybrid::Tool::initialize ()
{
  StatusCode sc = LoKi::Hybrid::Base::initialize() ;
  if ( sc.isFailure() ) { return sc ; }
  // perform some printout in "debug" mode
  if ( msgLevel( MSG::DEBUG ) )
  { m_lines.insert( m_lines.begin() , "print dir()" ) ; }
  return StatusCode::SUCCESS ;
}
// ============================================================================
// finalization of the tool
// ============================================================================
StatusCode LoKi::Hybrid::Tool::finalize  ()
{
  // predicates:
  if ( 0 != m_cuts       ) { delete m_cuts       ; m_cuts       = 0  ; }
  if ( 0 != m_vcuts      ) { delete m_vcuts      ; m_vcuts      = 0  ; }
  if ( 0 != m_acuts      ) { delete m_acuts      ; m_acuts      = 0  ; }
  if ( 0 != m_ppcuts     ) { delete m_ppcuts     ; m_ppcuts     = 0  ; }
  // functions:
  if ( 0 != m_func       ) { delete m_func       ; m_func       = 0  ; }
  if ( 0 != m_vfunc      ) { delete m_vfunc      ; m_vfunc      = 0  ; }
  if ( 0 != m_afunc      ) { delete m_afunc      ; m_afunc      = 0  ; }
  if ( 0 != m_ppfunc     ) { delete m_ppfunc     ; m_ppfunc     = 0  ; }
  // maps:
  if ( 0 != m_maps       ) { delete m_maps       ; m_maps       = 0  ; }
  if ( 0 != m_vmaps      ) { delete m_vmaps      ; m_vmaps      = 0  ; }
  if ( 0 != m_ppmaps     ) { delete m_ppmaps     ; m_ppmaps     = 0  ; }
  // pipes:
  if ( 0 != m_pipes      ) { delete m_pipes      ; m_pipes      = 0  ; }
  if ( 0 != m_vpipes     ) { delete m_vpipes     ; m_vpipes     = 0  ; }
  if ( 0 != m_pppipes    ) { delete m_pppipes    ; m_pppipes    = 0  ; }
  // fun-vals:
  if ( 0 != m_funvals    ) { delete m_funvals    ; m_funvals    = 0  ; }
  if ( 0 != m_vfunvals   ) { delete m_vfunvals   ; m_vfunvals   = 0  ; }
  if ( 0 != m_ppfunvals  ) { delete m_ppfunvals  ; m_ppfunvals  = 0  ; }
  // cut-vals:
  if ( 0 != m_cutvals    ) { delete m_cutvals    ; m_cutvals    = 0  ; }
  if ( 0 != m_vcutvals   ) { delete m_vcutvals   ; m_vcutvals   = 0  ; }
  if ( 0 != m_ppcutvals  ) { delete m_ppcutvals  ; m_ppcutvals  = 0  ; }
  // sources:
  if ( 0 != m_sources    ) { delete m_sources    ; m_sources    = 0  ; }
  if ( 0 != m_vsources   ) { delete m_vsources   ; m_vsources   = 0  ; }
  if ( 0 != m_ppsources  ) { delete m_ppsources  ; m_ppsources  = 0  ; }
  //
  // finalize the base
  return LoKi::Hybrid::Base::finalize() ;
}
// ============================================================================
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi::Hybrid,Tool)
// ============================================================================
// The END
// ============================================================================
