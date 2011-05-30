// $Id: $
// ============================================================================
// Include files 
// ============================================================================
// LbOniaPairs 
// ============================================================================
#include "LbOniaPairs/OniaPairs.h"
// ============================================================================
/** @file
 *  Implementation of helper fiunctions for Onia Pairs Production
 *  @author Alexey Novoselov 
 *  @author Alexey Nososelov Alexey.Novoselov@cern.ch
 *  @date 2011-05-30
 *  
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
extern "C" 
{
#ifdef WIN32
  double __stdcall   ONIAPAIRSSETPAR      ( double*    , const char* , int ) ;
  double __stdcall   ONIAPAIRSGETPAR      (              const char* , int ) ;
#define opsetpar     ONIAPAIRSSETPAR
#define opgetpar     ONIAPAIRSGETPAR
#else
  double             oniapairssetpar_     ( double*    , const char* , int ) ;
  double             oniapairsgetpar_     (              const char* , int ) ;
#define opsetpar     oniapairssetpar_ 
#define opgetpar     oniapairsgetpar_ 
#endif
} 
// ============================================================================
bool OniaPairs::SetPar ( std::string n , double value )
{
  // convert to upper case 
  std::transform ( n.begin() , n.end() , n.begin() , toupper ) ;
  const double _value = value ;
  // jump into FORTRAN
  opsetpar ( &value , n.c_str() , n.size() ) ;
  return _value == OniaPairs::GetPar ( n ) ;
} 
// ============================================================================
double OniaPairs::GetPar ( std::string n ) 
{
  // convert to upper case 
  std::transform ( n.begin() , n.end() , n.begin() , toupper ) ;
  // jump into FORTRAN
  return opgetpar ( n.c_str() , n.size() ) ;
}
// ============================================================================
// The END 
// ============================================================================
