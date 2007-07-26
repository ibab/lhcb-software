// $Id: HybridTool.cpp,v 1.1 2007-07-26 13:25:09 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/System.h" 
#include "GaudiKernel/ToolFactory.h" 
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
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2004-06-29
 */
namespace LoKi 
{
  namespace Hybrid 
  {
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
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2004-06-29
     */
    class Tool 
      : public    LoKi::Hybrid::Base 
      , public virtual IHybridTool 
      , public virtual IHybridFactory 
    {
      // friend factory forn instantiation
      friend class ToolFactory<LoKi::Hybrid::Tool> ;
    public:
      /// initialization of the tool 
      virtual StatusCode initialize () ;
      /// finalization   of the tool 
      virtual StatusCode finalize  () ;
    public:
      // ========================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param cuts placeholder for the result 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string& pycode , LoKi::Types::Cut& cuts ) 
      { return _get ( pycode , m_cuts  , cuts ) ; }
      // ========================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param cuts placeholder for the  result 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string& pycode , LoKi::Types::VCut& cuts ) 
      { return _get ( pycode , m_vcuts , cuts ) ; }
      // ========================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param cuts placeholder for the result
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string& pycode , LoKi::Types::ACut& cuts ) 
      { return _get ( pycode , m_acuts , cuts ) ; }
      // ========================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param func placehoplder for the result 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string& pycode , LoKi::Types::Fun& func ) 
      { return _get ( pycode , m_func  , func ) ; }
      // ========================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param func placeholder for the result 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string& pycode , LoKi::Types::VFun& func ) 
      { return _get ( pycode , m_vfunc , func ) ; }
      // ========================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param func placeholder for the result 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string& pycode , LoKi::Types::AFun& func ) 
      { return _get ( pycode , m_afunc , func ) ; }
      // ========================================================================
    public:
      /// set the C++ predicate for LHCb::Particle
      virtual void set ( const LoKi::Types::Cuts&    cut ) 
      { LoKi::Hybrid::Base::_set ( m_cuts  , cut ) ; }
      /// set the C++ predicate for LHCb::Vertex 
      virtual void set ( const LoKi::Types::VCuts&   cut ) 
      { LoKi::Hybrid::Base::_set ( m_vcuts , cut ) ; }
      /// set the C++ predicate for arary of particles 
      virtual void set ( const LoKi::Types::ACuts&   cut ) 
      { LoKi::Hybrid::Base::_set ( m_acuts , cut ) ; }
      /// set the C++ function for LHCb::Particle  
      virtual void set ( const LoKi::Types::Func&    cut ) 
      { LoKi::Hybrid::Base::_set ( m_func  , cut ) ; }
      /// set the C++ function for LHCb::Vertex 
      virtual void set ( const LoKi::Types::VFunc&   cut ) 
      { LoKi::Hybrid::Base::_set ( m_vfunc  , cut ) ; }
      /// set the C++ function for arary of particles 
      virtual void set ( const LoKi::Types::AFunc&   cut ) 
      { LoKi::Hybrid::Base::_set ( m_afunc  , cut ) ; }
    protected:
      /// constrcutor
      Tool
    ( const std::string& type   ,
      const std::string& name   ,
      const IInterface*  parent ) ;
    /// destrcutor 
    virtual ~Tool();
  private:
    // the default constructor is disabled 
    Tool() ; ///< the default constructor is disabled     
    // the copy constructor is disabled 
    Tool           ( const Tool& )  ; ///< the copy constructor is disabled 
    // the assignement operator  is disabled 
      Tool& operator=( const Tool& )  ; ///< no assignement 
    private:
    /// helper method to sdave many lines:
      template <class TYPE1,class TYPE2>
      inline StatusCode _get 
      ( const std::string& pycode , 
        TYPE1*&            local  , 
        TYPE2&             output ) ;
    protected:
      // local holder of cuts 
      LoKi::Types::Cuts*  m_cuts  ;
      LoKi::Types::VCuts* m_vcuts ;
      LoKi::Types::ACuts* m_acuts ;
      LoKi::Types::Func*  m_func  ;
      LoKi::Types::VFunc* m_vfunc ;
      LoKi::Types::AFunc* m_afunc ;    
      typedef std::vector<std::string> Modules ;
      Modules     m_modules ;
      std::string m_actor   ;
      typedef std::vector<std::string> Lines   ;
      Lines       m_lines   ;
    } ;
  } // end of namespace LoKi::Hybrid 
} // end of namespace LoKi
// ============================================================================
// helper method to sdave many lines:
// ============================================================================
template <class TYPE1,class TYPE2>
inline StatusCode LoKi::Hybrid::Tool::_get 
( const std::string& pycode , 
  TYPE1*&            local  , 
  TYPE2&             output ) 
{
  // prepare the actual python code 
  std::string code = makeCode  ( m_modules , m_actor , pycode , m_lines ) ;
  // define the scope:
  LoKi::Hybrid::Lock lock ( this ) ;   ///< ATTENTION: the scope is locked!!
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
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi::Hybrid,Tool);
// ============================================================================
// Standard constructor
// ============================================================================
LoKi::Hybrid::Tool::Tool 
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : LoKi::Hybrid::Base ( type , name , parent )
    //
  , m_cuts      ( 0 )
  , m_vcuts     ( 0 )
  , m_acuts     ( 0 )
  , m_func      ( 0 )
  , m_vfunc     ( 0 )
  , m_afunc     ( 0 )
    //
  , m_modules   (   ) 
  , m_actor     ( "LoKi.Hybrid.Engine()" ) 
  , m_lines     (   )
{
  // 
  declareInterface<LoKi::IHybridTool>   ( this ) ;
  declareInterface<LoKi::IHybridFactory>( this ) ;
  //
  m_modules.push_back ( "LoKiPhys.decorators"          ) ;
  m_modules.push_back ( "LoKiArrayFunctors.decorators" ) ;
  declareProperty ( "Modules" , m_modules , "Python modules to be imported" ) ;
  declareProperty ( "Actor"   , m_actor   , "The processing engine"         ) ;
  declareProperty ( "Lines"   , m_lines   , "Additional python lines"       ) ;
} 
// ============================================================================
/// Destructor (virtual and protected) 
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
  //
  if ( 0 != m_cuts   ) { delete m_cuts   ; m_cuts   = 0  ; }
  if ( 0 != m_vcuts  ) { delete m_vcuts  ; m_vcuts  = 0  ; }
  if ( 0 != m_acuts  ) { delete m_acuts  ; m_acuts  = 0  ; }
  if ( 0 != m_func   ) { delete m_func   ; m_func   = 0  ; }
  if ( 0 != m_vfunc  ) { delete m_vfunc  ; m_vfunc  = 0  ; }
  if ( 0 != m_afunc  ) { delete m_afunc  ; m_afunc  = 0  ; }
  // finalize the base 
  return LoKi::Hybrid::Base::finalize() ;
}
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
