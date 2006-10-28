// $Id: HidValleyC.cpp,v 1.1.1.1 2006-10-28 14:06:41 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1.1.1 $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <ctype.h>
#include <string>
#include <cstdlib>
#include <algorithm>
// ============================================================================
// local
// ============================================================================
#include "LbHidValley/HidValley.h"
// ============================================================================
/** @impementationof C++ interface to the main 
 *  routine for Hidden Valley production 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-10-20
 */
// ============================================================================
extern "C" 
{
  // ==========================================================================
#ifdef WIN32
  int    __stdcall   HIDDENVALLEY  ( int*, int* , const char* , int ) ;
  int    __stdcall   HVEVENT       ( int*                           ) ;
  double __stdcall   HVSETPAR      ( double*    , const char* , int ) ;
  double __stdcall   HVGETPAR      (              const char* , int ) ;
#define hiddenvalley HIDDENVALLEY
#define hvevent      HVEVENT 
#define hvsetpar     HVSETPAR
#define hvgetpar     HVGETPAR
#else
  int                hiddenvalley_ ( int*, int* , const char* , int ) ;
  int                hvevent_      ( int*                           ) ;
  double             hvsetpar_     ( double*    , const char* , int ) ;
  double             hvgetpar_     (              const char* , int ) ;
#define hiddenvalley hiddenvalley_
#define hvevent      hvevent_
#define hvsetpar     hvsetpar_ 
#define hvgetpar     hvgetpar_ 
#endif
} ;
// ============================================================================
/// C++ interface for "main" fortran program HidVal
// ============================================================================
int HidValley::HidVal ( int seed , int events , const std::string& file ) 
{ return hiddenvalley ( &seed, &events, file.c_str() , file.size() ) ; } ;
// ============================================================================
bool HidValley::setPar ( std::string n , double value )
{
  // convert to upper case 
  std::transform ( n.begin() , n.end() , n.begin() , toupper ) ;
  const double _value = value ;
  // jump into FORTRAN
  hvsetpar ( &value , n.c_str() , n.size() ) ;
  return _value == HidValley::getPar ( n ) ;
} ;
// ============================================================================
double HidValley::getPar ( std::string n ) 
{
  // convert to upper case 
  std::transform ( n.begin() , n.end() , n.begin() , toupper ) ;
  // jump into FORTRAN
  return hvgetpar ( n.c_str() , n.size() ) ;
}
// ============================================================================
bool HidValley::hvEvent( int dummy ) 
{ return 0 == hvevent (&dummy) ; } ;
// ============================================================================


// ============================================================================
/// The END 
// ============================================================================
