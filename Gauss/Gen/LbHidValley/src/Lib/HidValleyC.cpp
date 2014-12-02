// $Id: HidValleyC.cpp,v 1.2 2006-11-30 12:37:01 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2006/10/28 14:06:41  ibelyaev
// New package: "Hidden Valley" production 
// 
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
/** @file 
 *  implementationof C++ interface to the main 
 *  routine for Hidden Valley production 
 *
 *  Matt Strassler <strasslr@noether.phys.washington.edu>:
 *
 * "This program simulates q qbar -> Z' -> v-quarks, followed by
 * v-showering and v-hadronization to v-pions.  The current program
 * makes various approximations and is not suitable for precision 
 * studies, but should be good at the factor-of-two level.
 *  In particular, the v-hadronization code uses Pythias QCD
 * hadronization code, scaled up in energy, and this is not a perfect 
 * simulation of the real model. Also, the Z' charges are not precisely 
 * those discussed in ref [1]; but in any case the model of [1] was 
 * just an example, and many other models exist.  Updates to this 
 * program will gradually correct these issues."
 *
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
}

// ============================================================================
/// C++ interface for "main" fortran program HidVal
// ============================================================================
int HidValley::HidVal ( int seed , int events , const std::string& file ) 
{ 
  return hiddenvalley ( &seed, &events, file.c_str() , file.size() ) ; 
}

// ============================================================================
bool HidValley::setPar ( std::string n , double value )
{
  // convert to upper case 
  std::transform ( n.begin() , n.end() , n.begin() , toupper ) ;
  const double _value = value ;
  // jump into FORTRAN
  hvsetpar ( &value , n.c_str() , n.size() ) ;
  return _value == HidValley::getPar ( n ) ;
}

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
{ 
  return 0 == hvevent (&dummy) ; 
}


// ============================================================================
/// The END 
// ============================================================================
