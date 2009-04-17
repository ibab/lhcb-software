// $Id: HybridBase.cpp,v 1.10 2009-04-17 14:48:56 cattanem Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <sstream>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/System.h" 
// ============================================================================
// local
// ============================================================================
#include "LoKi/HybridBase.h"
#include "LoKi/ILoKiSvc.h"
// ============================================================================
// Boots 
// ============================================================================
#include "boost/algorithm/string/trim.hpp"
// ============================================================================
// Python 
// ============================================================================
#if defined(__GNUC__)
#undef _POSIX_C_SOURCE // Hack to suppress duplicate definition warning
#endif
#include "Python.h"
// ============================================================================
/** @file 
 *  Implementation file for class LoKi::HybridTool
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
// ============================================================================
namespace 
{
  /// replace all new lines with "newline+comments"
  std::string addComment ( std::string str )
  {
    static const std::string comm = "\n##! " ;
    std::string::size_type pos = str.find ( '\n' ) ;
    while ( std::string::npos != pos ) 
    {
      str.replace ( pos , 1 , comm );
      pos = str.find ( '\n' , pos + 2 ) ;
    }
    return str ;
  }
  /// prepare the actual code: trim and remove the paired quotes 
  std::string trimCode ( std::string code ) 
  {
    // trim leading and trailing blanks:
    boost::trim ( code ) ;
    if ( code.empty() )                { return code ; } 
    // check for paired quotes:
    std::string::const_iterator ifront = code.begin () + 1 ;
    std::string::const_iterator iback  = code.end   () - 1 ;
    //
    if ( *ifront != *iback ) { return code ; }
    // 
    if ( '\'' != *ifront && '"' != *ifront ) { return code ; }
    //
    return trimCode ( std::string ( ifront , iback ) ) ;
  }
}
// ============================================================================
// Standard constructor
// ============================================================================
LoKi::Hybrid::Base::Base
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : GaudiTool ( type , name , parent )
  //
  , m_pyInit   ( false )
  , m_showCode ( false ) 
{
  declareProperty( "ShowCode" , m_showCode , "Flag to display the prepared python code") ;
} 
// ============================================================================
// Destructor (virtual and protected) 
// ============================================================================
LoKi::Hybrid::Base::~Base() {} 
// ============================================================================
// initialization of the tool 
// ============================================================================
StatusCode LoKi::Hybrid::Base::initialize () 
{
  StatusCode sc = GaudiTool::initialize() ;
  if ( sc.isFailure() ) { return sc ; }  
  // force the loading/initialization of  LoKi Service 
  svc<LoKi::ILoKiSvc>( "LoKiSvc" , true ) ;
  // Initialize python if it not yet done 
  if ( !Py_IsInitialized() ) 
  {
    info() << "Initialization of Python is triggered" << endmsg ;
    Py_Initialize () ;
    m_pyInit = true ;
  }  
  return StatusCode::SUCCESS ;
} 
// ============================================================================
// finalization of the tool 
// ============================================================================
StatusCode LoKi::Hybrid::Base::finalize  () 
{
  // finalize python (if the owner)
  if ( Py_IsInitialized() && m_pyInit ) 
  {
    info() << "Finalization   of Python is triggered" << endmsg ;
    std::string line ; 
    line += "                                                           # " + name() + "\n" ;
    line += "import atexit                                              # " + name() + "\n" ;
    line += "if atexit._exithandlers :                                  # " + name() + "\n" ;
    line += "\tprint 'ATTENTION: Clear non-empty atexit._exithandlers'  # " + name() + "\n" ;
    line += "\tprint 'len(_exithandlers)=',len(atexit._exithandlers)    # " + name() + "\n" ;
    line += "\tatexit._exithandlers=[]                                  # " + name() + "\n" ;
    line += "                                                           # " + name() + "\n" ;    
    const int result =  PyRun_SimpleString ( line.c_str() ) ;
    if ( 0 != result ) 
    {
      Warning ( "Error code from PyRun_SimpleString" , 1000 + result ).ignore() ;
      warning() << "The problematic code is \n" << line << endmsg ;
      if ( PyErr_Occurred() ) { PyErr_Print() ; } 
    }
    Py_Finalize () ;
  }
  // finalize the base 
  return GaudiTool::finalize() ;
}
// ============================================================================
/* execute the python code 
 * @attention the method must be invoked only with appropriate scope!!!
 * @param code python code 
 * @return status code 
 */
// ============================================================================
StatusCode LoKi::Hybrid::Base::executeCode ( const std::string& pycode ) const 
{
  // Check the proper python environment:
  if ( !Py_IsInitialized() ) { return Error("Python is not initialized yet!") ; }
  
  // play with raw Python
  
  // local scope 
  PyObject* locals  = PyEval_GetLocals  () ;
  if ( 0 == locals ) 
  {
    info() << "PyEval_GetLocals:  'locals'  points to NULL" << endmsg ;
    if ( PyErr_Occurred() ) { PyErr_Print() ; }
  }
  // global scope 
  PyObject* globals = PyEval_GetGlobals () ;
  bool globnew = false ;
  if ( 0 == globals ) 
  { 
    info() << "PyEval_GetGlobals: 'globals' points to NULL" << endmsg ;
    if ( PyErr_Occurred() ) { PyErr_Print() ; }
    globals  = PyDict_New() ;
    if ( PyErr_Occurred() ) { PyErr_Print() ; }
    globnew  = true ;
    PyDict_SetItemString ( globals, "__builtins__" , PyEval_GetBuiltins() ) ;
    if ( PyErr_Occurred() ) { PyErr_Print() ; }
  }
  // ===========
  // ATTENTION! 
  // ===========
  // execute Python 'code' 
  PyObject* result = PyRun_String 
    ( const_cast<char*> ( pycode.c_str() ) ,                // EXECUTE CODE 
      Py_file_input  , globals  , locals ) ;
  
  bool ok = true ;
  if ( PyErr_Occurred()        ) { PyErr_Print() ; ok = false ; }
  
  if ( 0 != globals && globnew ) { Py_XDECREF( globals ) ; }
  
  if ( 0 != result             ) { Py_XDECREF ( result )      ; }
  else if ( PyErr_Occurred()   ) { PyErr_Print() ; ok = false ; }
  else { ok = false ; }
  
  if ( !ok ) 
  {
    err () << " Error has occured in Python: the problematic code is : "
           << endmsg 
           << pycode  
           << endmsg ;
    return Error( " Error has occured in Python " ) ;
  }

  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  prepare the actual python code from the functor:
 *  @param modules list of modules 
 *  @param actor the actual actor
 *  @param code the code for the functor 
 *  @param lines more python lines to be used 
 *  @return the valid python code 
 */
// ============================================================================
std::string LoKi::Hybrid::Base::makeCode 
( const LoKi::Hybrid::Base::Strings& modules , 
  const std::string&                 actor   ,
  const std::string&                 code    , 
  const LoKi::Hybrid::Base::Strings& lines   ,
  const std::string&                 context ) const 
{
  std::string _code = code ;
  {
    std::string::size_type pos = _code.find ( '\n' ) ;
    while ( std::string::npos != pos ) 
    {
      _code.replace( pos , 1 , " " ) ;
      pos = _code.find ('\n') ;
    }
  }
  // trim and remove the paired quotes:
  _code = trimCode ( _code ) ;
  //
  std::ostringstream stream ;
  // start the code:
  stream << "# " << std::string(78,'=') << std::endl ;
  stream << "# python pseudomodule, generated for the tool '" 
         << name() << "'" << std::endl ;
  stream << "# " << std::string(78,'=') << std::endl ;
  stream << "# Arguments: " << std::endl ;
  stream << "# \tcode    = " << Gaudi::Utils::toString ( _code   )  << std::endl ;
  stream << "# \tactor   = " << Gaudi::Utils::toString ( actor   )  << std::endl ;
  stream << "# \tmodules = " << Gaudi::Utils::toString ( modules )  << std::endl ;
  stream << "# \tlines   = " << Gaudi::Utils::toString ( lines   )  << std::endl ;
  stream << "# \tcontext = " << Gaudi::Utils::toString ( addComment ( "\n" + context ) )  << std::endl ;
  stream << "# " << std::string(78,'=') << std::endl ;
  // define imported modules:
  stream << "##        MODULES :" << std::endl ;
  for ( Strings::const_iterator imodule = modules.begin() ; 
        modules.end() != imodule ; ++imodule ) 
  {
    stream << "from " << (*imodule) << " import *" << std::endl ;
  }
  stream << "## End of MODULES  " << std::endl ;
  stream << "## The ACTOR :"              << std::endl ;
  stream << "_actor=" << actor << std::endl ;
  // put additional lines:
  stream << "##        LINES :" << std::endl ;
  for ( Strings::const_iterator iline = lines.begin() ; 
        lines.end() != iline ; ++iline ) 
  { stream << (*iline) << std::endl ; }
  stream << "## end of LINES  " << std::endl ;
  // put the context 
  stream << "##        CONTEXT :" << std::endl ;
  if ( !context.empty() ) { stream << context << std::endl ; }
  stream << "## End of CONTEXT  " << std::endl ;
  stream << "##        CODE :"    << std::endl ;  
  stream << "_code="  << _code    << std::endl ;
  stream << "## End of CODE :"    << std::endl ;  
  stream << "sc=_actor.process('" << name() << "',_code)" << std::endl ;
  stream << "# " << std::string(78,'=') << std::endl ;
  stream << "# The END "                << std::endl ;
  stream << "# " << std::string(78,'=') << std::endl ; 
  //
  std::string result = stream.str() ;
  if ( msgLevel ( MSG::DEBUG ) || showCode() ) 
  {
    debug() << "Generated Python code:" << std::endl 
            << result 
            << endmsg ;  
  }
  ///
  return result ;
}
// ============================================================================



// ============================================================================
// The END 
// ============================================================================
