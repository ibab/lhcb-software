// $Id: F77Utils.cpp,v 1.2 2007-06-29 13:21:06 ibelyaev Exp $
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <ctype.h>
#include <iostream>
// ============================================================================
// Generators 
// ============================================================================
#include "Generators/F77Utils.h"
// ============================================================================
/** @file 
 *  Implementation file for functionf from namespace F77Units
 *  @date 2006-10-03 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 */
// ============================================================================
//  Open Fortran file with status="UNKNOWN" 
// ============================================================================
StatusCode F77Utils::open 
( int                lun     , 
  const std::string& file    , 
  const bool         verbose )  
{ return openUnknown ( lun , file , verbose ) ; }
// ============================================================================
// Open "FORMATTED" Fortran file with status="UNKNOWN" 
// ============================================================================
StatusCode F77Utils::openFormatted 
( int                lun     , 
  const std::string& file    , 
  const bool         verbose )  
{ return openUnknownFormatted ( lun , file , verbose ) ; }
// ============================================================================
extern "C"
{
#ifdef WIN32 
  int __stdcall F77CLFILE   (int* LUN );
  int __stdcall F77ONFILE   (int* LUN ,const char* FILE, int LEN );
  int __stdcall F77OOFILE   (int* LUN ,const char* FILE, int LEN );
  int __stdcall F77OUFILE   (int* LUN ,const char* FILE, int LEN );
  int __stdcall F77ONFFILE  (int* LUN ,const char* FILE, int LEN );
  int __stdcall F77OOFFILE  (int* LUN ,const char* FILE, int LEN );
  int __stdcall F77OUFFILE  (int* LUN ,const char* FILE, int LEN );
  int __stdcall F77GETUNIT  (int* LUN ) ;
#define f77clfile  F77CLFILE  
#define f77onfile  F77ONFILE  
#define f77oofile  F77OOFILE  
#define f77oufile  F77OUFILE  
#define f77onffile F77ONFFILE  
#define f77ooffile F77OOFFILE  
#define f77ouffile F77OUFFILE
#define f77getunit F77GETUNIT
#else 
  int           f77clfile_  (int* LUN ) ;
  int           f77onfile_  (int* LUN , const char* FILE, int LEN ) ;
  int           f77oofile_  (int* LUN , const char* FILE, int LEN ) ;
  int           f77oufile_  (int* LUN , const char* FILE, int LEN ) ;
  int           f77onffile_ (int* LUN , const char* FILE, int LEN ) ;
  int           f77ooffile_ (int* LUN , const char* FILE, int LEN ) ;
  int           f77ouffile_ (int* LUN , const char* FILE, int LEN ) ;
  int           f77getunit_ (int* LUN ) ;
#define f77clfile  f77clfile_
#define f77onfile  f77onfile_
#define f77oofile  f77oofile_
#define f77oufile  f77oufile_
#define f77onffile f77onffile_
#define f77ooffile f77ooffile_
#define f77ouffile f77ouffile_
#define f77getunit f77getunit_
#endif 
}
// ==========================================================================
// close the opened F77 file 
// ==========================================================================
StatusCode F77Utils::close ( int lun , const bool verbose ) 
{ 
  const int result = f77clfile( &lun )  ;
  if ( result != lun ) 
  { 
    std::cerr << "F77Utils::close()               ERROR Error in closing LUN=" 
              << lun << std::endl ; 
  }
  else if ( verbose ) 
  { 
    std::cout << "F77Utils::close()               INFO  LUN=" 
              << lun << " is closed " << std::endl ; 
  }
  return result != lun ?  StatusCode::FAILURE : StatusCode::SUCCESS ;
} 
// ==========================================================================
// Open Fortran file with status="NEW" 
// ==========================================================================
StatusCode F77Utils::openNew 
( int                lun     , 
  const std::string& file    , 
  const bool         verbose )  
{
  const int result = f77onfile ( &lun , file.c_str() , file.size() ) ;
  if ( result != lun ) 
  { 
    std::cerr << "F77Utils::openNew               ERROR Error in opening LUN=" << lun 
              << " FILE='"<< file << "' "<< std::endl ; 
  }
  else if ( verbose ) 
  {
    std::cout << "F77Utils::openNew               INFO  LUN=" << lun 
              << " FILE='"<< file << "' is opened "<< std::endl ; 
  }
  return result != lun ? StatusCode::FAILURE : StatusCode::SUCCESS ;
} 
// ==========================================================================
// Open Fortran file with status="OLD" 
// ==========================================================================
StatusCode F77Utils::openOld 
( int                lun     , 
  const std::string& file    , 
  const bool         verbose )  
{
  const int result = f77oofile ( &lun , file.c_str() , file.size() ) ;
  if ( result != lun ) 
  { 
    std::cerr << "F77Utils::openOld               ERROR Error in opening LUN=" << lun 
              << " FILE='"<< file << "' "<< std::endl ; 
  }
  else if ( verbose ) 
  {
    std::cout << "F77Utils::openOld               INFO  LUN=" << lun 
              << " FILE='"<< file << "' is opened "<< std::endl ; 
  }  
  return result != lun ? StatusCode::FAILURE : StatusCode::SUCCESS ;
} 
// ==========================================================================
// Open Fortran file with status="UNKNOWN" 
// ==========================================================================
StatusCode F77Utils::openUnknown 
( int                lun     , 
  const std::string& file    , 
  const bool         verbose )  
{
  const int result = f77oufile ( &lun , file.c_str() , file.size() ) ;
  if ( result != lun ) 
  { 
    std::cerr << "F77Utils::openUnknown           ERROR Error in opening LUN=" << lun 
              << " FILE='"<< file << "' "<< std::endl ; 
  }
  else if ( verbose ) 
  {
    std::cout << "F77Utils::openUnknown           INFO  LUN=" << lun 
              << " FILE='"<< file << "' is opened "<< std::endl ; 
  }    
  return result != lun ? StatusCode::FAILURE : StatusCode::SUCCESS ;
} 
// ==========================================================================
// Open "FORMATTED" Fortran file with status="NEW" 
// ==========================================================================
StatusCode F77Utils::openNewFormatted 
( int                lun     , 
  const std::string& file    , 
  const bool         verbose )  
{
  const int result = f77onffile ( &lun , file.c_str() , file.size() ) ;
  if ( result != lun ) 
  { 
    std::cerr << "F77Utils::openNewFormatted      ERROR Error in opening LUN=" << lun 
              << " FILE='"<< file << "' "<< std::endl ; 
  }
  else if ( verbose ) 
  {
    std::cout << "F77Utils::openNewFormatted      INFO  LUN=" << lun 
              << " FILE='"<< file << "' is opened "<< std::endl ; 
  }    
  return result != lun ? StatusCode::FAILURE : StatusCode::SUCCESS ;
}
// ==========================================================================
// Open "FORMATTED" Fortran file with status="OLD" 
// ==========================================================================
StatusCode F77Utils::openOldFormatted 
( int                lun     , 
  const std::string& file    , 
  const bool         verbose )  
{
  const int result = f77ooffile ( &lun , file.c_str() , file.size() ) ;
  if ( result != lun ) 
  { 
    std::cerr << "F77Utils::openOldFormatted      ERROR Error in opening LUN=" << lun 
              << " FILE='"<< file << "' "<< std::endl ; 
  }
  else if ( verbose ) 
  {
    std::cout << "F77Utils::openOldFormatted      INFO  LUN=" << lun 
              << " FILE='"<< file << "' is opened "<< std::endl ; 
  }  
  return result != lun ? StatusCode::FAILURE : StatusCode::SUCCESS ;
}
// ==========================================================================
// Open "FORMATTED" Fortran file with status="UNKNOWN" 
// ==========================================================================
StatusCode F77Utils::openUnknownFormatted 
( int                lun     , 
  const std::string& file    , 
  const bool         verbose )  
{
  const int result = f77ouffile ( &lun , file.c_str() , file.size() ) ;
  if ( result != lun ) 
  { 
    std::cerr << "F77Utils::openUnknownFormatted  ERROR Error in opening LUN=" << lun 
              << " FILE='"<< file << "' "<< std::endl ; 
  }
  else if ( verbose )
  {
    std::cout << "F77Utils::openUnknownFormatted  INFO  LUN=" << lun 
              << " FILE='"<< file << "' is opened "<< std::endl ; 
  }  
  return result != lun ? StatusCode::FAILURE : StatusCode::SUCCESS ;
}
// ==========================================================================
// find free fortran unit
// ==========================================================================
int F77Utils::getUnit( const bool verbose ) 
{
  int n = 0 ;
  const int result = f77getunit ( &n ) ;
  if ( 0 >= result || 100 <= result ) 
  {
    std::cerr << "F77Utils::getUnit               ERROR No free units" << std::endl ;
  } 
  else if ( verbose )  
  {
    std::cout << "F77Utils::getUnit               INFO  Free unit LUN=" << result << std::endl ;
  }
  return result ;
}
// ==========================================================================



// ============================================================================
// The END 
// ============================================================================
