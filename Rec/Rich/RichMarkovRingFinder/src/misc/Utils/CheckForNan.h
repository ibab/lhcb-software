// $Id: CheckForNan.h,v 1.1 2008-06-20 09:54:23 jonrob Exp $
// ============================================================================
#ifndef UTILS_CHECKFORNAN_H 
#define UTILS_CHECKFORNAN_H 1
// ============================================================================
// Include files
// ============================================================================
#if defined(_WIN32)
#include <float.h>
namespace Lester
{
  inline int lfin ( double x ) { return  _finite ( x ) ; }
  inline int lnan ( double x ) { return  _isnan  ( x ) ; }
  inline int linf ( double x ) { return ( _finite(x) ? 0 : ( x < 0 ? -1 : 1 ) ); }
}
#elif defined(__ICC)
#include <mathimf.h>
namespace Lester
{
  inline int lfin ( double x ) { return  isfinite ( x ) ; }
  inline int lnan ( double x ) { return  isnan    ( x ) ; }
  inline int linf ( double x ) { return  isinf    ( x ) ; }
}
#else // GCC
#include <cmath>
namespace Lester
{
  //inline int lfin ( double x ) { return  finite ( x ) ; }
  inline int lfin ( double x ) { return  std::isfinite ( x ) ; }
  inline int lnan ( double x ) { return  std::isnan    ( x ) ; }
  inline int linf ( double x ) { return  std::isinf    ( x ) ; }
}
#endif
// ============================================================================
// The END 
// ============================================================================
#endif // UTILS_CHECKFORNAN_H
// ============================================================================
