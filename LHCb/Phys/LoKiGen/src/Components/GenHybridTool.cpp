// $Id: GenHybridTool.cpp,v 1.1 2007-07-25 15:07:58 ibelyaev Exp $
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
 */
namespace LoKi 
{
  namespace Hybrid 
  {
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
      ( const std::string& pycode , LoKi::Types::GCut&  cuts ) 
      { return _get ( pycode , m_gcuts  , cuts ) ; }
      // ========================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param cuts the placeholder for the result 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string& pycode , LoKi::Types::GVCut&  cuts ) 
      { return _get ( pycode , m_gvcuts  , cuts ) ; }
      // ========================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param func the placeholder for the result 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string& pycode , LoKi::Types::GFun& func )
      { return _get ( pycode , m_gfunc  , func ) ; }
      // ========================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param func the placeholder for the result 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string& pycode , LoKi::Types::GVFun&  func ) 
      { return _get ( pycode , m_gvfunc  , func ) ; }
      // ========================================================================
    public:
      /// set the C++ predicate for HepMC::GenParticle
      virtual void set ( const LoKi::Types::GCuts&   cut ) 
      { LoKi::Hybrid::Base::_set ( m_gcuts , cut ) ; }
      /// set the C++ predicate for HepMC::GenVertex
      virtual void set ( const LoKi::Types::GVCuts&   cut ) 
      { LoKi::Hybrid::Base::_set ( m_gvcuts , cut ) ; }
      /// set the C++ function for HepMC::GenParticle
      virtual void set ( const LoKi::Types::GFunc&   cut ) 
      { LoKi::Hybrid::Base::_set ( m_gfunc  , cut ) ; }
      /// set the C++ function for HepMC::GenVertex
      virtual void set ( const LoKi::Types::GVFunc&   cut ) 
      { LoKi::Hybrid::Base::_set ( m_gvfunc  , cut ) ; }
    protected:
      /// constrcutor
      GenTool
      ( const std::string& type   ,
        const std::string& name   ,
        const IInterface*  parent ) ;
      /// destrcutor 
      virtual ~GenTool();
    private:
      // the default constructor is disabled 
      GenTool() ; ///< the default constructor is disabled     
      // the copy constructor is disabled 
      GenTool           ( const GenTool& )  ; ///< the copy constructor is disabled 
      // the assignement operator  is disabled 
      GenTool& operator=( const GenTool& )  ; ///< no assignement 
    private:
      /// helper method to sdave many lines:
      template <class TYPE1,class TYPE2>
      inline StatusCode _get 
      ( const std::string& pycode , 
        TYPE1*&            local  , 
        TYPE2&             output ) ;
    protected:
      // local holder of cuts 
      LoKi::Types::GCuts*   m_gcuts   ;
      LoKi::Types::GVCuts*  m_gvcuts  ;
      LoKi::Types::GFunc*   m_gfunc   ;    
      LoKi::Types::GVFunc*  m_gvfunc  ;    
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
inline StatusCode LoKi::Hybrid::GenTool::_get 
( const std::string& pycode , 
  TYPE1*&            local  , 
  TYPE2&             output ) 
{
  // prepare the actual python code 
  std::string code = makeCode  ( m_modules , m_actor , pycode , m_lines ) ;
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
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi::Hybrid,GenTool);
// ============================================================================
// Standard constructor
// ============================================================================
LoKi::Hybrid::GenTool::GenTool 
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : LoKi::Hybrid::Base ( type , name , parent )
    //
  , m_gcuts       ( 0 )
  , m_gvcuts      ( 0 )
  , m_gfunc       ( 0 )
  , m_gvfunc      ( 0 )
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
  m_modules.push_back ( "LoKiGenMC.decorators" ) ;
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
  //
  if ( 0 != m_gcuts    ) { delete m_gcuts    ; m_gcuts    = 0  ; }
  if ( 0 != m_gvcuts   ) { delete m_gvcuts   ; m_gvcuts   = 0  ; }
  if ( 0 != m_gfunc    ) { delete m_gfunc    ; m_gfunc    = 0  ; }
  if ( 0 != m_gvfunc   ) { delete m_gvfunc   ; m_gvfunc   = 0  ; }
  // finalize the base 
  return LoKi::Hybrid::Base::finalize() ;
}
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
