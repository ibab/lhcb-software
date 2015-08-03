// $Id: HltFactory.cpp,v 1.2 2008-09-23 13:13:36 ibelyaev Exp $
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
#include "LoKi/HltLock.h"
#include "LoKi/IHltFactory.h"
#include "LoKi/IHltAntiFactory.h"
// ============================================================================
// Local 
// ============================================================================
/** @file 
 *
 *  definitione and the implementation file for class LoKi::Hybrid::HltFactory
 *  
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-09-18
 */
namespace LoKi 
{
  // ==========================================================================
  namespace Hybrid 
  {
    // ========================================================================
    /** @class HltFactory 
     *
     *  Concrete impelmentation of LoKi::Hybrid::IHltFActory interface 
     *
     *  This file is a part of LoKi project - 
     *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
     *
     *  The package has been designed with the kind help from
     *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
     *  contributions and advices from G.Raven, J.van Tilburg, 
     *  A.Golutvin, P.Koppenburg have been used in the design.
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-18
     */
    class HltFactory
      : public         LoKi::Hybrid::Base 
      , public virtual LoKi::Hybrid::IHltFactory
      , public virtual LoKi::Hybrid::IHltAntiFactory
    {
      // ======================================================================
      // friend factory forn instantiation
      friend class ToolFactory<LoKi::Hybrid::HltFactory> ;
      // ======================================================================
    public:
      // ======================================================================
      /// initialization of the tool 
      virtual StatusCode initialize () ;
      /// finalization   of the tool 
      virtual StatusCode finalize   () ;
      // ======================================================================
    public: // L0-functors 
      // ======================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param cuts the placeholder for the result 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string&   pycode  , 
        LoKi::Types::L0_Cut& cuts    , 
        const std::string&   context ) 
      { return _get ( pycode , m_l0_cuts  , cuts , context ) ; }
      // ======================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param cuts the placeholder for the result 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string&   pycode  , 
        LoKi::Types::L0_Fun& cuts    ,
        const std::string&   context ) 
      { return _get ( pycode , m_l0_func , cuts , context ) ; }
      // ======================================================================
    public: // ODIN-functors 
      // ======================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param cuts the placeholder for the result 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string&   pycode  , 
        LoKi::Types::ODIN_Cut& cuts    , 
        const std::string&   context ) 
      { return _get ( pycode , m_o1_cuts  , cuts , context ) ; }
      // ======================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param cuts the placeholder for the result 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string&   pycode  , 
        LoKi::Types::ODIN_Fun& cuts    ,
        const std::string&   context ) 
      { return _get ( pycode , m_o1_func , cuts , context ) ; }
      // ======================================================================
    public: // HLT-functors 
      // ======================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param cuts the placeholder for the result 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string&    pycode  , 
        LoKi::Types::HLT_Cut& cuts    , 
        const std::string&    context ) 
      { return _get ( pycode , m_hlt_cuts  , cuts , context ) ; }
      // ======================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param cuts the placeholder for the result 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string&    pycode  , 
        LoKi::Types::HLT_Fun& cuts    ,
        const std::string&    context ) 
      { return _get ( pycode , m_hlt_func , cuts , context ) ; }
      // ======================================================================
    public: // L0-functors 
      // ======================================================================
      /// set the C++ predicate 
      virtual void set ( const LoKi::Types::L0_Cuts& cut ) 
      { LoKi::Hybrid::Base::_set ( m_l0_cuts , cut ) ; }
      /// set the C++ function 
      virtual void set ( const LoKi::Types::L0_Func& cut ) 
      { LoKi::Hybrid::Base::_set ( m_l0_func , cut ) ; }
      // ======================================================================
    public: // O1-functors 
      // ======================================================================
      /// set the C++ predicate 
      virtual void set ( const LoKi::Types::ODIN_Cuts& cut ) 
      { LoKi::Hybrid::Base::_set ( m_o1_cuts , cut ) ; }
      /// set the C++ function 
      virtual void set ( const LoKi::Types::ODIN_Func& cut ) 
      { LoKi::Hybrid::Base::_set ( m_o1_func , cut ) ; }
      // ======================================================================
    public: // HLT-functors 
      // ======================================================================
      /// set the C++ predicate 
      virtual void set ( const LoKi::Types::HLT_Cuts& cut ) 
      { LoKi::Hybrid::Base::_set ( m_hlt_cuts , cut ) ; }
      /// set the C++ function 
      virtual void set ( const LoKi::Types::HLT_Func& cut ) 
      { LoKi::Hybrid::Base::_set ( m_hlt_func , cut ) ; }
      // ======================================================================
    protected:
      // ======================================================================
      /// constrcutor
      HltFactory
      ( const std::string& type   ,
        const std::string& name   ,
        const IInterface*  parent ) ;
      /// destructor 
      virtual ~HltFactory(); // destrcutor 
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      HltFactory () ;                    // the default constructor is disabled     
      // the copy constructor is disabled 
      HltFactory ( const HltFactory& )  ;   // the copy constructor is disabled 
      // the assignement operator  is disabled 
      HltFactory& operator=( const HltFactory& )  ;           // no assignement 
      // ======================================================================
    private:
      // ======================================================================
      /// helper method to sdave many lines:
      template <class TYPE1,class TYPE2>
      inline StatusCode _get 
      ( const std::string& pycode  , 
        TYPE1*&            local   , 
        TYPE2&             output  , 
        const std::string& context ) ;
      // ======================================================================
    private:
      // ======================================================================
      /// L0-functors :
      LoKi::Types::L0_Cuts*    m_l0_cuts  ;                        // predicate 
      LoKi::Types::L0_Func*    m_l0_func  ;                        //  function
      /// ODIN-functors :
      LoKi::Types::ODIN_Cuts*  m_o1_cuts  ;                        // predicate 
      LoKi::Types::ODIN_Func*  m_o1_func  ;                        //  function
      /// HLT-functors :
      LoKi::Types::HLT_Cuts*   m_hlt_cuts ;                        // predicate 
      LoKi::Types::HLT_Func*   m_hlt_func ;                        //  function
      // ======================================================================
      typedef std::vector<std::string> Modules ;
      Modules     m_modules ;
      std::string m_actor   ;
      typedef std::vector<std::string> Lines   ;
      Lines       m_lines   ;
      // ======================================================================
    } ;
    // ========================================================================
  } // end of namespace LoKi::Hybrid 
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
// helper method to sdave many lines:
// ============================================================================
template <class TYPE1,class TYPE2>
inline StatusCode LoKi::Hybrid::HltFactory::_get 
( const std::string& pycode  , 
  TYPE1*&            local   , 
  TYPE2&             output  ,
  const std::string& context ) 
{
  // prepare the actual python code 
  std::string code = 
    makeCode  ( m_modules , m_actor , pycode , m_lines , context ) ;
  // define and lock the scope:
  LoKi::Hybrid::HltLock lock ( this ) ;   ///< ATTENTION: the scope is locked!!
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
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi::Hybrid,HltFactory)
// ============================================================================
// Standard constructor
// ============================================================================
LoKi::Hybrid::HltFactory::HltFactory
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : LoKi::Hybrid::Base ( type , name , parent )
  // L0-functors 
  , m_l0_cuts  ( 0 )
  , m_l0_func  ( 0 )
  // O1-functors 
  , m_o1_cuts  ( 0 )
  , m_o1_func  ( 0 )
  // HLT-functors 
  , m_hlt_cuts ( 0 )
  , m_hlt_func ( 0 )
  //
  , m_modules   (   ) 
  , m_actor     ( "LoKi.Hybrid.HltEngine()" ) 
  , m_lines     ()
{
  //
  declareInterface<LoKi::Hybrid::IHltFactory>     ( this ) ;
  declareInterface<LoKi::Hybrid::IHltAntiFactory> ( this ) ;
  //
  m_modules.push_back ( "LoKiHlt.decorators"   ) ;
  //
  declareProperty ( "Modules" , m_modules , 
                    "Python modules to be imported"          ) ;
  declareProperty ( "Actor"   , m_actor   , 
                    "The processing engine"                  ) ;
  declareProperty ( "Lines"   , m_lines   , 
                    "Additional Python lines to be executed" ) ;

  //
  //
  // Treat C++
  //
  m_cpplines.push_back ( "#include \"LoKi/LoKiHlt.h\""            ) ;
  m_cpplines.push_back ( "#include \"LoKi/LoKiNumbers.h\""        ) ;
  //
}
// ============================================================================
// Destructor (virtual and protected) 
// ============================================================================
LoKi::Hybrid::HltFactory::~HltFactory() {}
// ============================================================================
// initialization of the tool 
// ============================================================================
StatusCode LoKi::Hybrid::HltFactory::initialize () 
{
  StatusCode sc = LoKi::Hybrid::Base::initialize() ;
  if ( sc.isFailure() ) { return sc ; }  
  if ( msgLevel ( MSG::DEBUG ) ) { m_lines.insert( m_lines.begin() , "dir()") ; }
  return StatusCode::SUCCESS ;
} 
// ============================================================================
// finalization of the tool 
// ============================================================================
StatusCode LoKi::Hybrid::HltFactory::finalize  () 
{
  // functors 
  if ( 0 != m_l0_cuts  ) { delete m_l0_cuts  ; m_l0_cuts  = 0  ; }
  if ( 0 != m_l0_func  ) { delete m_l0_func  ; m_l0_func  = 0  ; }
  if ( 0 != m_o1_cuts  ) { delete m_o1_cuts  ; m_o1_cuts  = 0  ; }
  if ( 0 != m_o1_func  ) { delete m_o1_func  ; m_o1_func  = 0  ; }
  if ( 0 != m_hlt_cuts ) { delete m_hlt_cuts ; m_hlt_cuts = 0  ; }
  if ( 0 != m_hlt_func ) { delete m_hlt_func ; m_hlt_func = 0  ; }
  // finalize the base 
  return LoKi::Hybrid::Base::finalize() ;
}
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
