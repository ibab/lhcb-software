
//--------------------------------------------------------------------------------
/** @file RichSmartIDHashFuncs.h
 *
 *  Header file to define hash functions for RichSmartIDs
 *
 *  CVS Log :-
 *  $Id: RichSmartIDHashFuncs.h,v 1.1 2005-01-13 12:21:18 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2005-01-11
 */
//--------------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHSMARTIDHASHFUNCS_H 
#define RICHKERNEL_RICHSMARTIDHASHFUNCS_H 1

// GaudiKernel
#include "GaudiKernel/stl_hash.h"

// Kernel
#include "Kernel/RichSmartID.h"

#ifdef __GNUC__
namespace __gnu_cxx
{
  template <> struct hash<RichSmartID>
  { size_t operator() ( const RichSmartID& s ) const { return s.key(); } };
  template <> struct hash<const RichSmartID>
  { size_t operator() ( const RichSmartID& s ) const { return s.key(); } };
  template <> struct hash<RichSmartID&>
  { size_t operator() ( const RichSmartID& s ) const { return s.key(); } };
  template <> struct hash<const RichSmartID&>
  { size_t operator() ( const RichSmartID& s ) const { return s.key(); } };
};
#endif

#endif // RICHKERNEL_RICHSMARTIDHASHFUNCS_H
