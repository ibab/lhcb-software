// $Id: F77Utils.cpp,v 1.1 2006-10-04 13:36:46 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <ctype.h>
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
/** @fn open
 *  Open Fortran file with status="UNKNOWN" 
 *  @param lun logical file number 
 *  @param file fiel name 
 *  @return status code 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-10-03
 */
StatusCode F77Utils::open ( int lun , const std::string& file )  
{ return openUnknown ( lun , file ) ; }
// ============================================================================
/** @fn openFormatted
 *  Open "FORMATTED" Fortran file with status="UNKNOWN" 
 *  @param lun logical file number 
 *  @param file fiel name 
 *  @return status code 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-10-03
 */
StatusCode F77Utils::openFormatted ( int lun , const std::string& file )  
{ return openUnknownFormatted ( lun , file ) ; }
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
#define f77clfile  F77CLFILE  
#define f77onfile  F77ONFILE  
#define f77oofile  F77OOFILE  
#define f77oufile  F77OUFILE  
#define f77onffile F77ONFFILE  
#define f77ooffile F77OOFFILE  
#define f77ouffile F77OUFFILE  
#else 
  int           f77clfile_  (int* LUN );
  int           f77onfile_  (int* LUN , const char* FILE, int LEN ) ;
  int           f77oofile_  (int* LUN , const char* FILE, int LEN ) ;
  int           f77oufile_  (int* LUN , const char* FILE, int LEN ) ;
  int           f77onffile_ (int* LUN , const char* FILE, int LEN ) ;
  int           f77ooffile_ (int* LUN , const char* FILE, int LEN ) ;
  int           f77ouffile_ (int* LUN , const char* FILE, int LEN ) ;
#define f77clfile  f77clfile_
#define f77onfile  f77onfile_
#define f77oofile  f77oofile_
#define f77oufile  f77oufile_
#define f77onffile f77onffile_
#define f77ooffile f77ooffile_
#define f77ouffile f77ouffile_
#endif 
} ;

// ==========================================================================
/** @fn close 
 *  close the opened F77 file 
 *  @param lun logical file number 
 *  @return status code 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-10-03
 */
// ==========================================================================
StatusCode F77Utils::close ( int lun ) 
{ 
  return lun != f77clfile( &lun ) ?  StatusCode::FAILURE : StatusCode::SUCCESS ;
} ;
// ==========================================================================
/** @fn openNew 
 *  Open Fortran file with status="NEW" 
 *  @param lun logical file number 
 *  @param file fiel name 
 *  @return status code 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-10-03
 */
// ==========================================================================
StatusCode F77Utils::openNew ( int lun , const std::string& file )  
{
  return 
    lun != f77onfile ( &lun , file.c_str() , file.size() ) ? 
    StatusCode::FAILURE : StatusCode::SUCCESS ;
} ;
// ==========================================================================
/** @fn openOld
 *  Open Fortran file with status="OLD" 
 *  @param lun logical file number 
 *  @param file fiel name 
 *  @return status code 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-10-03
 */
// ==========================================================================
StatusCode F77Utils::openOld     ( int lun , const std::string& file ) 
{
  return 
    lun != f77oofile ( &lun , file.c_str() , file.size() ) ? 
    StatusCode::FAILURE : StatusCode::SUCCESS ;
} ;
// ==========================================================================
/** @fn openUnknown
 *  Open Fortran file with status="UNKNOWN" 
 *  @param lun logical file number 
 *  @param file fiel name 
 *  @return status code 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-10-03
 */
// ==========================================================================
StatusCode F77Utils::openUnknown ( int lun , const std::string& file ) 
{
  return 
    lun != f77oufile ( &lun , file.c_str() , file.size() ) ? 
    StatusCode::FAILURE : StatusCode::SUCCESS ;
} ;
// ==========================================================================
/** @fn openNewFormatted 
 *  Open "FORMATTED" Fortran file with status="NEW" 
 *  @param lun logical file number 
 *  @param file fiel name 
 *  @return status code 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-10-03
 */
// ==========================================================================
StatusCode F77Utils::openNewFormatted ( int lun , const std::string& file ) 
{
  return 
    lun != f77onffile ( &lun , file.c_str() , file.size() ) ? 
    StatusCode::FAILURE : StatusCode::SUCCESS ;
};
// ==========================================================================
/** @fn openOldFormatted 
 *  Open "FORMATTED" Fortran file with status="OLD" 
 *  @param lun logical file number 
 *  @param file fiel name 
 *  @return status code 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-10-03
 */
// ==========================================================================
StatusCode F77Utils::openOldFormatted ( int lun , const std::string& file ) 
{
  return 
    lun != f77ooffile ( &lun , file.c_str() , file.size() ) ? 
    StatusCode::FAILURE : StatusCode::SUCCESS ;
};
// ==========================================================================
/** @fn openUnknownFormatted
 *  Open "FORMATTED" Fortran file with status="UNKNOWN" 
 *  @param lun logical file number 
 *  @param file fiel name 
 *  @return status code 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-10-03
 */
// ==========================================================================
StatusCode F77Utils::openUnknownFormatted 
( int lun , const std::string& file ) 
{
  return 
    lun != f77ouffile ( &lun , file.c_str() , file.size() ) ? 
    StatusCode::FAILURE : StatusCode::SUCCESS ;
};
// ==========================================================================


// ============================================================================
// The END 
// ============================================================================
