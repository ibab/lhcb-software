
//--------------------------------------------------------------------------------
/** @file StringHashFuncs.h
 *
 *  Header file to define hash functions for std::string
 *
 *  CVS Log :-
 *  $Id: StringHashFuncs.h,v 1.1 2005-01-13 12:21:18 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2005-01-11
 */
//--------------------------------------------------------------------------------
#ifndef RICHKERNEL_STRINGHASHFUNCS_H 
#define RICHKERNEL_STRINGHASHFUNCS_H 1

// GaudiKernel
#include "GaudiKernel/stl_hash.h"

#ifdef __GNUC__
namespace __gnu_cxx
{
  template <> struct hash<std::string>
  { size_t operator() ( const std::string& s ) const { return __gnu_cxx_hash_string ( s.c_str() ); } };
  template <> struct hash<const std::string>
  { size_t operator() ( const std::string& s ) const { return __gnu_cxx_hash_string ( s.c_str() ); } };
  template <> struct hash<std::string&>  
  { size_t operator() ( const std::string& s ) const { return __gnu_cxx_hash_string ( s.c_str() ); } };
  template <> struct hash<const std::string&>
  { size_t operator() ( const std::string& s ) const { return __gnu_cxx_hash_string ( s.c_str() ); } };
};
#endif

#endif // RICHKERNEL_STRINGHASHFUNCS_H
