// $Id: Bender.cpp,v 1.1.1.1 2004-01-22 22:29:45 ibelyaev Exp $
// ============================================================================ 
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================ 
// $Log: not supported by cvs2svn $ 
// ============================================================================ 
// Incldue files 
// ============================================================================ 
#include "Python.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
// ============================================================================ 
// GaudiKernel
// ============================================================================ 
#include "GaudiKernel/System.h" 
// ============================================================================

int main ( int argc, char** argv ) 
{
  if ( argc < 2 ) 
  { 
    std::cout << "ERROR: insufficient command arguments" << std::endl ; 
    exit(1) ; 
  };
  
  Py_Initialize();
  // Get the Python version
  std::string fullversion = Py_GetVersion();
  std::string version ( fullversion , 0 , fullversion.find_first_of ( ' ' ) ) ;
  std::string vers    
    ( version     , 0 , 
      version.find_first_of( '.' , version.find_first_of('.') + 1 ) );
  std::cout << "Python version: [" << vers << "]" << std::endl;
  
  // Startup commands
  PyRun_SimpleString( "print dir()" );
  
  std::cout << "Running now: " << argv[1] << std::endl;
  std::ifstream file(argv[1]);
  std::stringstream str;
  if( file ) {
    char ch;
    while( file.get(ch) ) str.put(ch);
    PyRun_SimpleString( const_cast<char*>(str.str().c_str()) );
    file.close();
  }
  else 
  {
    std::cout << "ERROR: could not open file " << argv[1] << std::endl;
    std::cerr << "ERROR: could not open file " << argv[1] << std::endl;
    exit(1) ;
  }
  std::cout << "Exiting now " << std::endl;
  exit ( 0 ) ;
};

// ============================================================================ 
// The EDN 
// ============================================================================ 

