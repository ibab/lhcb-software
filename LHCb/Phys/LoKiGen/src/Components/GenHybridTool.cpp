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
#include "LoKi/GenHybridLock.h"
#include "LoKi/IGenHybridTool.h"
#include "LoKi/IGenHybridFactory.h"
// ============================================================================
// Local 
// ============================================================================
/** @file 
 *  definitione and the implementation file for class LoKi::Hybrid::GenTool
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
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
namespace LoKi 
{
  // ==========================================================================
  namespace Hybrid 
  {
    // ========================================================================
    /** @class GenTool 
     *
     *  Concrete impelmentation of LoKi::IGenHybridTool interface 
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
    class GenTool 
      : public       LoKi::Hybrid::Base 
      , public virtual IGenHybridTool 
      , public virtual IGenHybridFactory 
    {
      // friend factory forn instantiation
      friend class ToolFactory<LoKi::Hybrid::GenTool> ;
    public:
      // ======================================================================
      /// initialization of the tool 
      virtual StatusCode initialize () ;
      /// finalization   of the tool 
      virtual StatusCode finalize  () ;
      // ======================================================================
    public:
      // ======================================================================
      // predicates 
      // ======================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param cuts the placeholder for the result 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string& pycode  , 
        LoKi::Types::GCut& cuts    , 
        const std::string& context ) 
      { return _get ( pycode , m_gcuts  , cuts , context ) ; }
      // ======================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param cuts the placeholder for the result 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string&  pycode  , 
        LoKi::Types::GVCut& cuts    ,
        const std::string&  context ) 
      { return _get ( pycode , m_gvcuts  , cuts , context ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // functions 
      // ======================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param func the placeholder for the result 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string& pycode  , 
        LoKi::Types::GFun& func    , 
        const std::string& context ) 
      { return _get ( pycode , m_gfunc  , func , context ) ; }
      // ======================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param func the placeholder for the result 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string&  pycode  , 
        LoKi::Types::GVFun& func    , 
        const std::string&  context ) 
      { return _get ( pycode , m_gvfunc  , func , context ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // maps 
      // ======================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param func the placeholder for the result 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string&  pycode  , 
        LoKi::Types::GMap&  func    , 
        const std::string&  context ) 
      { return _get ( pycode , m_gmap  , func , context ) ; }
      // ======================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param func the placeholder for the result 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string&   pycode  , 
        LoKi::Types::GVMap&  func    , 
        const std::string&   context ) 
      { return _get ( pycode , m_gvmap  , func , context ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // pipes 
      // ======================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param func the placeholder for the result 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string&  pycode  , 
        LoKi::Types::GPipe& func    , 
        const std::string&  context ) 
      { return _get ( pycode , m_gpipe  , func , context ) ; }
      // ======================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param func the placeholder for the result 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string&   pycode  , 
        LoKi::Types::GVPipe& func    , 
        const std::string&   context ) 
      { return _get ( pycode , m_gvpipe  , func , context ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // fun-vals
      // ======================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param func the placeholder for the result 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string&    pycode  , 
        LoKi::Types::GFunVal& func    , 
        const std::string&    context ) 
      { return _get ( pycode , m_gfunval  , func , context ) ; }
      // ======================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param func the placeholder for the result 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string&     pycode  , 
        LoKi::Types::GVFunVal& func    , 
        const std::string&     context ) 
      { return _get ( pycode , m_gvfunval  , func , context ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // cut-vals
      // ======================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param func the placeholder for the result 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string&    pycode  , 
        LoKi::Types::GCutVal& func    , 
        const std::string&    context ) 
      { return _get ( pycode , m_gcutval  , func , context ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // sources
      // ======================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param func the placeholder for the result 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string&     pycode  , 
        LoKi::Types::GSource&  func    , 
        const std::string&     context ) 
      { return _get ( pycode , m_gsource  , func , context ) ; }
      // ======================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param func the placeholder for the result 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string&      pycode  , 
        LoKi::Types::GVSource&  func    , 
        const std::string&      context ) 
      { return _get ( pycode , m_gvsource , func , context ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // predicates 
      // ======================================================================
      /// set the C++ predicate for HepMC::GenParticle
      virtual void set ( const LoKi::Types::GCuts&   cut ) 
      { LoKi::Hybrid::Base::_set ( m_gcuts , cut ) ; }
      /// set the C++ predicate for HepMC::GenVertex
      virtual void set ( const LoKi::Types::GVCuts&   cut ) 
      { LoKi::Hybrid::Base::_set ( m_gvcuts , cut ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // functions 
      // ======================================================================
      /// set the C++ function for HepMC::GenParticle
      virtual void set ( const LoKi::Types::GFunc&   cut ) 
      { LoKi::Hybrid::Base::_set ( m_gfunc  , cut ) ; }
      /// set the C++ function for HepMC::GenVertex
      virtual void set ( const LoKi::Types::GVFunc&   cut ) 
      { LoKi::Hybrid::Base::_set ( m_gvfunc  , cut ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // maps 
      // ======================================================================
      /// set the C++ "map" for HepMC::GenParticle
      virtual void set ( const LoKi::Types::GMaps&   cut ) 
      { LoKi::Hybrid::Base::_set ( m_gmap  , cut ) ; }
      /// set the C++ "map" for HepMC::GenVertex
      virtual void set ( const LoKi::Types::GVMaps&   cut ) 
      { LoKi::Hybrid::Base::_set ( m_gvmap  , cut ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // pipes
      // ======================================================================
      /// set the C++ "pipe" for HepMC::GenParticle
      virtual void set ( const LoKi::Types::GPipes&   cut ) 
      { LoKi::Hybrid::Base::_set ( m_gpipe  , cut ) ; }
      /// set the C++ "pipe" for HepMC::GenVertex
      virtual void set ( const LoKi::Types::GVPipes&   cut ) 
      { LoKi::Hybrid::Base::_set ( m_gvpipe , cut ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // funvals
      // ======================================================================
      /// set the C++ "fun-val" for HepMC::GenParticle
      virtual void set ( const LoKi::Types::GFunVals&   cut ) 
      { LoKi::Hybrid::Base::_set ( m_gfunval  , cut ) ; }
      /// set the C++ "pipe" for HepMC::GenVertex
      virtual void set ( const LoKi::Types::GVFunVals&   cut ) 
      { LoKi::Hybrid::Base::_set ( m_gvfunval , cut ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // cutvals
      // ======================================================================
      /// set the C++ "cut-val" for HepMC::GenParticle
      virtual void set ( const LoKi::Types::GCutVals&   cut ) 
      { LoKi::Hybrid::Base::_set ( m_gcutval  , cut ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // sources
      // ======================================================================
      /// set the C++ "source" for HepMC::GenParticle
      virtual void set ( const LoKi::Types::GSources&   cut ) 
      { LoKi::Hybrid::Base::_set ( m_gsource  , cut ) ; }
      /// set the C++ "source" for HepMC::GenVertex
      virtual void set ( const LoKi::Types::GVSources&   cut ) 
      { LoKi::Hybrid::Base::_set ( m_gvsource , cut ) ; }
      // ======================================================================
    protected:
      // ======================================================================
      /// constrcutor
      GenTool
      ( const std::string& type   ,
        const std::string& name   ,
        const IInterface*  parent ) ;
      /// destrcutor 
      virtual ~GenTool();
      // ======================================================================
    private:
      // ======================================================================
      // the default constructor is disabled 
      GenTool() ; ///< the default constructor is disabled     
      // the copy constructor is disabled 
      GenTool           ( const GenTool& )  ; ///< the copy constructor is disabled 
      // the assignement operator  is disabled 
      GenTool& operator=( const GenTool& )  ; ///< no assignement 
      // ======================================================================
    private:
      // ======================================================================
      /// helper method to save many lines:
      template <class TYPE1,class TYPE2>
      inline StatusCode _get 
      ( const std::string& pycode  , 
        TYPE1*&            local   , 
        TYPE2&             output  , 
        const std::string& context ) ;
      // ======================================================================
    protected:
      // ======================================================================
      //
      //   local holders of cuts 
      //
      // predicates:
      LoKi::Types::GCuts*      m_gcuts      ;
      LoKi::Types::GVCuts*     m_gvcuts     ;
      // functions:
      LoKi::Types::GFunc*      m_gfunc      ;    
      LoKi::Types::GVFunc*     m_gvfunc     ;    
      // maps:
      LoKi::Types::GMaps*      m_gmap       ;    
      LoKi::Types::GVMaps*     m_gvmap      ;    
      // pipes: 
      LoKi::Types::GPipes*     m_gpipe      ;    
      LoKi::Types::GVPipes*    m_gvpipe     ;    
      // fun-vals: 
      LoKi::Types::GFunVals*   m_gfunval    ;    
      LoKi::Types::GVFunVals*  m_gvfunval   ;    
      // cut-vals: 
      LoKi::Types::GCutVals*   m_gcutval    ;    
      // sources:
      LoKi::Types::GSources*    m_gsource   ;    
      LoKi::Types::GVSources*   m_gvsource  ;    
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
inline StatusCode LoKi::Hybrid::GenTool::_get 
( const std::string& pycode  , 
  TYPE1*&            local   , 
  TYPE2&             output  ,
  const std::string& context ) 
{
  // prepare the actual python code 
  std::string code = 
    makeCode  ( m_modules , m_actor , pycode , m_lines , context ) ;
  // define and lock the scope:
  LoKi::Hybrid::GenLock lock ( this ) ;   ///< ATTENTION: the scope is locked!!
  // clear the placeholder:
  if ( 0 != local ) { delete local ; local = 0 ; }
  // execute the code 
  StatusCode sc = executeCode ( code ) ;
  if ( sc.isFailure() ) 
  { return Error ( "Error from LoKi::Hybrid::Base::executeCode"      ) ; } // RETURN 
  if ( 0 == local     ) 
  { return Error ( "Invaid object for the code '"+pycode+"'" ) ; } // RETURN 
  // assign the result 
  output = *local ;                                                // ASSIGN
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi::Hybrid,GenTool)
// ============================================================================
// Standard constructor
// ============================================================================
LoKi::Hybrid::GenTool::GenTool 
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : LoKi::Hybrid::Base ( type , name , parent )
  // predicates:
  , m_gcuts       ( 0 )
  , m_gvcuts      ( 0 )
  // functions:
  , m_gfunc       ( 0 )
  , m_gvfunc      ( 0 )
  // maps:
  , m_gmap        ( 0 )
  , m_gvmap       ( 0 )
  // pipes:
  , m_gpipe       ( 0 )
  , m_gvpipe      ( 0 )
  // fun-vals:
  , m_gfunval     ( 0 )
  , m_gvfunval    ( 0 )
  // cut-vals:
  , m_gcutval     ( 0 )
  // sources
  , m_gsource     ( 0 )
  , m_gvsource    ( 0 )
  //
  , m_modules   (   ) 
  , m_actor     ( "LoKi.Hybrid.GenEngine()" ) 
  , m_lines     ()
{
  // 
  declareInterface<LoKi::IGenHybridTool>   ( this ) ;
  declareInterface<LoKi::IGenHybridFactory>( this ) ;
  //
  m_modules.push_back ( "LoKiGen.decorators"   ) ;
  // m_modules.push_back ( "LoKiGenMC.decorators" ) ;
  m_modules.push_back ( "LoKiCore.functions"   ) ;
  //
  declareProperty ( "Modules" , m_modules , "Python modules to be imported"          ) ;
  declareProperty ( "Actor"   , m_actor   , "The processing engine"                  ) ;
  declareProperty ( "Lines"   , m_lines   , "Additional Python lines to be executed" ) ;
} 
// ============================================================================
/// Destructor (virtual and protected) 
// ============================================================================
LoKi::Hybrid::GenTool::~GenTool() {}
// ============================================================================
// initialization of the tool 
// ============================================================================
StatusCode LoKi::Hybrid::GenTool::initialize () 
{
  StatusCode sc = LoKi::Hybrid::Base::initialize() ;
  if ( sc.isFailure() ) { return sc ; }  
  if ( msgLevel ( MSG::DEBUG ) ) { m_lines.insert( m_lines.begin() , "dir()") ; }
  return StatusCode::SUCCESS ;
} 
// ============================================================================
// finalization of the tool 
// ============================================================================
StatusCode LoKi::Hybrid::GenTool::finalize  () 
{
  // predicates:
  if ( 0 != m_gcuts     ) { delete m_gcuts     ; m_gcuts     = 0  ; }
  if ( 0 != m_gvcuts    ) { delete m_gvcuts    ; m_gvcuts    = 0  ; }
  // functions:
  if ( 0 != m_gfunc     ) { delete m_gfunc     ; m_gfunc     = 0  ; }
  if ( 0 != m_gvfunc    ) { delete m_gvfunc    ; m_gvfunc    = 0  ; }
  // maps:
  if ( 0 != m_gmap      ) { delete m_gmap      ; m_gmap      = 0  ; }
  if ( 0 != m_gvmap     ) { delete m_gvmap     ; m_gvmap     = 0  ; }
  // pipes:
  if ( 0 != m_gpipe     ) { delete m_gpipe     ; m_gpipe     = 0  ; }
  if ( 0 != m_gvpipe    ) { delete m_gvpipe    ; m_gvpipe    = 0  ; }
  // funvals:
  if ( 0 != m_gfunval   ) { delete m_gfunval   ; m_gfunval   = 0  ; }
  if ( 0 != m_gvfunval  ) { delete m_gvfunval  ; m_gvfunval  = 0  ; }
  // cutvals:
  if ( 0 != m_gcutval   ) { delete m_gcutval   ; m_gcutval   = 0  ; }
  // sources:
  if ( 0 != m_gsource   ) { delete m_gsource   ; m_gsource   = 0  ; }
  if ( 0 != m_gvsource  ) { delete m_gvsource  ; m_gvsource  = 0  ; }
  //
  // finalize the base 
  return LoKi::Hybrid::Base::finalize() ;
}
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
