// $Id: Bender.cpp,v 1.2 2004-02-10 12:35:03 ibelyaev Exp $
// ============================================================================ 
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================ 
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2004/01/22 22:29:45  ibelyaev
// New location of 'old' package
// 
// ============================================================================ 
// Include files 
// ============================================================================ 
// STD & STL 
// ============================================================================ 
#include <cstdio>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
// ============================================================================ 
// Special for Unixes
// ============================================================================ 
#if defined(linux) || defined(sun)
#include "dlfcn.h"
#endif
#if !defined(WIN32)
#include <unistd.h>
#endif
// ============================================================================ 
// Python 
// ============================================================================ 
#include "Python.h"
// ============================================================================ 
// GaudiKernel
// ============================================================================ 
#include "GaudiKernel/StatusCode.h" 
#include "GaudiKernel/System.h" 
// ============================================================================

void       printHelp () ;
StatusCode processTheFile ( const std::string& name ) ;

int main ( int argc, char** argv ) 
{
  
  typedef  std::vector<std::string> Arguments ;
  
  Arguments Args ;
  { for( int ia = 1 ; ia < argc ; ++ia  ) 
  { Args.push_back ( std::string( argv[ia] )  ) ; } ; };
  
  
  if( Args.end() != std::find( Args.begin() , Args.end() , "-h"     ) ||
      Args.end() != std::find( Args.begin() , Args.end() , "-H"     ) ||
      Args.end() != std::find( Args.begin() , Args.end() , "--help" ) ||
      Args.end() != std::find( Args.begin() , Args.end() , "--HELP" )  )
  {
    printHelp() ;
    exit(0) ;
  };
  
  Py_Initialize();
  
  PySys_SetArgv( argc , argv ) ;  
  
  // Get the Python version
  std::string fullversion = Py_GetVersion();
  std::string version ( fullversion , 0 , fullversion.find_first_of ( ' ' ) ) ;
  std::string vers    
    ( version     , 0 , 
      version.find_first_of( '.' , version.find_first_of('.') + 1 ) );
  std::cout << "Python version: [" << vers << "]" << std::endl;
  
#if defined(linux) || defined(sun)
  // This is hack to make global the python symbols 
  // which are needed by the other python modules 
  // (eg. readline, math, etc,) libraries.
  { 
    std::string libname = "libpython" + vers + ".so";
    ::dlopen( libname.c_str() , RTLD_GLOBAL | RTLD_LAZY ) ;
  }
  //
#endif
  
  // Startup commands
  PyRun_SimpleString( "print dir()" ) ;
  
  for( Arguments::const_iterator iarg = Args.begin() ; 
       Args.end() != iarg ; ++iarg ) 
  {    
    // find all "*.py" arguments 
    if( std::string::npos == iarg->find(".py") ) { continue ; }
    // process the file 
    StatusCode sc = processTheFile( *iarg ) ;
    if( sc.isFailure() ) 
    { 
      std::cerr << " ERROR in processing file '" << *iarg << "'" << std::endl ; 
    }    
  }
  
  // before the interactive loop 
  PyRun_SimpleString     ( "print dir()" );
  
  if( Args.end() != std::find( Args.begin() , Args.end() , "-i"       ) ||
      Args.end() != std::find( Args.begin() , Args.end() , "-I"       ) ||
      Args.end() != std::find( Args.begin() , Args.end() , "--prompt" ) || 
      Args.end() != std::find( Args.begin() , Args.end() , "--PROMPT" )  )
  {
    //
    // ++Py_InteractiveFlag ;
    
    std::cout << " Invoke interactive prompt " << std::endl;    
    
#if !defined( WIN32 )
    //
    PyRun_SimpleString( "import rlcompleter");
    //
    PyRun_SimpleString( "rlcompleter.readline.parse_and_bind('tab: complete')");
    //
#endif
    
    // start the interactive loop 
    PyRun_InteractiveLoop ( stdin , "\0") ;  
  };
  
  
  // finalize python 
  std::cout << "Finalize Python" << std::endl;
  if ( Py_IsInitialized() ) { Py_Finalize() ;}
  
  // finalize python 
  std::cout << "Exiting now " << std::endl;
  exit ( 0 ) ;
};


/** @fn processTheFile 
 *
 *  execute python script
 *
 *  @athor Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2004-01-29
 */
StatusCode processTheFile( const std::string& name )
{ 
  //
  std::cout << "Running now '" << name << "'"<< std::endl;    
  //
#if !defined(WIN32)
  FILE* fp = fopen( name.c_str() , "r" );
  if( NULL != fp ) 
  {
    PyRun_SimpleFile( fp , const_cast<char*>(name.c_str()) );
    fclose( fp );
    return StatusCode::SUCCESS ;
  }
#else
  // This trick is needed to avoid 
  // problem with different versions of C -libraries  
  std::ifstream file( name ) ;
  std::stringstream str;
  if( file ) 
  {
    char ch;
    while( file.get(ch) ) { str.put(ch); }
    PyRun_SimpleString( const_cast<char*>(str.str().c_str()) );
    file.close();
    return StatusCode::SUCCESS ;
  }
#endif
  //
  std::cerr << " ERROR , could not open the file '" + name + "'" << std::endl;
  //
  return StatusCode::FAILURE ;
};

// ============================================================================ 
/** @fn printHelp
 *  print the help
 *  @author Vanya BELYAEV
 *  @date   2004-01-29 
 */
// ============================================================================ 
void printHelp()
{
  std::cout << " Usage : " << std::endl 
            << " Bender.exe [option] [file1] [file2] [file3] ... " 
            << std::endl 
            << "\tOptions and arguments " << std::endl 
            << "\t -i       : force  interactive prompt   " << std::endl
            << "\t -I       : force  interactive prompt   " << std::endl 
            << "\t --prompt : force  interactive prompt   " << std::endl 
            << "\t --PROMPT : force  interactive prompt   " << std::endl 
            << "\t -h       : print  this help            " << std::endl 
            << "\t --help   : print  this help            " << std::endl 
            << "\t --HELP   : print  this help            " << std::endl 
            << "\t file1    : script file to be executed  " << std::endl 
            << "\t file2    : script file to be executed  " << std::endl 
            << "\t file3    : script file to be executed  " << std::endl 
            << "\t file..n  : script file to be executed  " << std::endl ;
};
// ============================================================================ 

// ============================================================================ 
// The EDN 
// ============================================================================ 

