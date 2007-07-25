// $Id: MCHybridTool.cpp,v 1.1 2007-07-25 15:16:07 ibelyaev Exp $
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
#include "LoKi/MCHybridLock.h"
#include "LoKi/IMCHybridTool.h"
#include "LoKi/IMCHybridFactory.h"
// ============================================================================
// Local 
// ============================================================================
/** @file 
 *  definitione and the implementation file for class LoKi::HybridTool
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
    /** @class MCTool 
     *
     *  Concrete impelmentation of LoKi::IMCHybridTool interface 
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
    class MCTool 
      : public LoKi::Hybrid::Base 
      , public virtual IMCHybridTool 
      , public virtual IMCHybridFactory 
    {
      // friend factory forn instantiation
      friend class ToolFactory<LoKi::Hybrid::MCTool> ;
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
      ( const std::string& pycode , LoKi::Types::MCCut&  cuts ) 
      { return _get ( pycode , m_mccuts  , cuts ) ; }
      // ========================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param cuts the placeholder for the result 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string& pycode , LoKi::Types::MCVCut&  cuts ) 
      { return _get ( pycode , m_mcvcuts  , cuts ) ; }
      // ========================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param func the placeholder for the result 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string& pycode , LoKi::Types::MCFun& func )
      { return _get ( pycode , m_mcfunc  , func ) ; }
      // ========================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param func the placeholder for the result 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string& pycode , LoKi::Types::MCVFun&  func ) 
      { return _get ( pycode , m_mcvfunc  , func ) ; }
      // ========================================================================
    public:
      /// set the C++ predicate for LHCb::MCParticle
      virtual void set ( const LoKi::Types::MCCuts&    cut ) 
      { LoKi::Hybrid::Base::_set ( m_mccuts  , cut ) ; }
      /// set the C++ predicate for LHCb::MCVertex 
      virtual void set ( const LoKi::Types::MCVCuts&   cut ) 
      { LoKi::Hybrid::Base::_set ( m_mcvcuts , cut ) ; }
      /// set the C++ function for LHCb::MCParticle  
      virtual void set ( const LoKi::Types::MCFunc&    cut ) 
      { LoKi::Hybrid::Base::_set ( m_mcfunc  , cut ) ; }
      /// set the C++ function for LHCb::MCVertex 
      virtual void set ( const LoKi::Types::MCVFunc&   cut ) 
      { LoKi::Hybrid::Base::_set ( m_mcvfunc  , cut ) ; }
    protected:
      /// constrcutor
      MCTool
      ( const std::string& type   ,
        const std::string& name   ,
        const IInterface*  parent ) ;
      /// destrcutor 
      virtual ~MCTool();
    private:
      // the default constructor is disabled 
      MCTool() ; ///< the default constructor is disabled     
      // the copy constructor is disabled 
      MCTool( const MCTool& )  ; ///< the copy constructor is disabled 
      // the assignement operator  is disabled 
      MCTool& operator=( const MCTool& )  ; ///< no assignement 
    private:
      /// helper method to sdave many lines:
      template <class TYPE1,class TYPE2>
      inline StatusCode _get 
      ( const std::string& pycode , 
        TYPE1*&            local  , 
        TYPE2&             output ) ;
    protected:
      // local holder of cuts 
      LoKi::Types::MCCuts*  m_mccuts  ;
      LoKi::Types::MCVCuts* m_mcvcuts ;
      LoKi::Types::MCFunc*  m_mcfunc  ;
      LoKi::Types::MCVFunc* m_mcvfunc ;
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
inline StatusCode 
LoKi::Hybrid::MCTool::_get 
( const std::string& pycode , 
  TYPE1*&            local  , 
  TYPE2&             output ) 
{
  // prepare the actual python code 
  std::string code = makeCode  ( m_modules , m_actor , pycode , m_lines ) ;
  // define and lock the scope:
  LoKi::Hybrid::MCLock lock ( this ) ;   ///< ATTENTION: the scope is locked!!
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
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi::Hybrid,MCTool);
// ============================================================================
// Standard constructor
// ============================================================================
LoKi::Hybrid::MCTool::MCTool 
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : LoKi::Hybrid::Base ( type , name , parent )
    //
  , m_mccuts      ( 0 )
  , m_mcvcuts     ( 0 )
  , m_mcfunc      ( 0 )
  , m_mcvfunc     ( 0 )
    //
  , m_modules   (   ) 
  , m_actor     ( "LoKi.Hybrid.MCEngine()" ) 
  , m_lines     ()
{
  // 
  declareInterface<LoKi::IMCHybridTool>   ( this ) ;
  declareInterface<LoKi::IMCHybridFactory>( this ) ;
  //
  m_modules.push_back ( "LoKiMC.decorators"    ) ;
  m_modules.push_back ( "LoKiGenMC.decorators" ) ;
  declareProperty ( "Modules" , m_modules , "Python modules to be imported"          ) ;
  declareProperty ( "Actor"   , m_actor   , "The processing engine"                  ) ;
  declareProperty ( "Lines"   , m_lines   , "Additional Python lines to be executed" ) ;
} 
// ============================================================================
/// Destructor (virtual and protected) 
// ============================================================================
LoKi::Hybrid::MCTool::~MCTool() {}
// ============================================================================
// initialization of the tool 
// ============================================================================
StatusCode LoKi::Hybrid::MCTool::initialize () 
{
  StatusCode sc = LoKi::Hybrid::Base::initialize() ;
  if ( sc.isFailure() ) { return sc ; }  
  if ( msgLevel ( MSG::DEBUG ) ) { m_lines.insert( m_lines.begin() , "dir()") ; }
  return StatusCode::SUCCESS ;
} 
// ============================================================================
// finalization of the tool 
// ============================================================================
StatusCode LoKi::Hybrid::MCTool::finalize  () 
{
  //
  if ( 0 != m_mccuts   ) { delete m_mccuts   ; m_mccuts   = 0  ; }
  if ( 0 != m_mcvcuts  ) { delete m_mcvcuts  ; m_mcvcuts  = 0  ; }
  if ( 0 != m_mcfunc   ) { delete m_mcfunc   ; m_mcfunc   = 0  ; }
  if ( 0 != m_mcvfunc  ) { delete m_mcvfunc  ; m_mcvfunc  = 0  ; }
  // finalize the base 
  return LoKi::Hybrid::Base::finalize() ;
}
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
