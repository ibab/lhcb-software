// $Id: $
// ============================================================================
// Include files 
// ============================================================================
#include <algorithm>
// ============================================================================
// LbOniaPairs 
// ============================================================================
#include "LbOniaPairs/OniaPairs.h"
// ============================================================================
extern "C" 
{
#ifdef WIN32
  double __stdcall   ONIAPAIRSSETPAR      ( double*    , const char* , int ) ;
  double __stdcall   ONIAPAIRSGETPAR      (              const char* , int ) ;
  void   __stdcall   ONIAPAIRSPRINTX      (                                ) ;
#define opsetpar     ONIAPAIRSSETPAR
#define opgetpar     ONIAPAIRSGETPAR
#define opprintx     ONIAPAIRSPRINTX
#else
  double             oniapairssetpar_     ( double*    , const char* , int ) ;
  double             oniapairsgetpar_     (              const char* , int ) ;
  void               oniapairsprintx_     (                                ) ;
#define opsetpar     oniapairssetpar_ 
#define opgetpar     oniapairsgetpar_ 
#define opprintx     oniapairsprintx_ 
#endif
} ;
// ============================================================================
bool OniaPairs::SetPar ( std::string n , double value )
{
  // convert to upper case 
  std::transform ( n.begin() , n.end() , n.begin() , toupper ) ;
  const double _value = value ;
  // jump into FORTRAN
  opsetpar ( &value , n.c_str() , n.size() ) ;
  return _value == OniaPairs::GetPar ( n ) ;
} ;
// ============================================================================
double OniaPairs::GetPar ( std::string n ) 
{
  // convert to upper case 
  std::transform ( n.begin() , n.end() , n.begin() , toupper ) ;
  // jump into FORTRAN
  return opgetpar ( n.c_str() , n.size() ) ;
}
// ============================================================================
void OniaPairs::PrintCSTable ( ) 
{
  // jump into FORTRAN
  return opprintx ( ) ;
}
// ============================================================================
// The END 
// ============================================================================
