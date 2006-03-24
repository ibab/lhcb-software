
//--------------------------------------------------------------------------------
/** @file RichSmartIDHashFuncs.h
 *
 *  Header file to define hash functions for RichSmartIDs
 *
 *  CVS Log :-
 *  $Id: RichSmartIDHashFuncs.h,v 1.3 2006-03-24 10:53:51 jonrob Exp $
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2005-01-11
 */
//--------------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHSMARTIDHASHFUNCS_H 
#define RICHKERNEL_RICHSMARTIDHASHFUNCS_H 1

// GaudiKernel
#include "GaudiKernel/HashMap.h"

// Kernel
#include "Kernel/RichSmartID.h"

#ifdef __GNUC__

// GNU hash funcs
namespace __gnu_cxx
{
  template <> struct hash<LHCb::RichSmartID>
  { inline size_t operator() ( const LHCb::RichSmartID& s ) const { return (size_t)s.key(); } };
  template <> struct hash<const LHCb::RichSmartID>
  { inline size_t operator() ( const LHCb::RichSmartID& s ) const { return (size_t)s.key(); } };
  template <> struct hash<LHCb::RichSmartID&>
  { inline size_t operator() ( const LHCb::RichSmartID& s ) const { return (size_t)s.key(); } };
  template <> struct hash<const LHCb::RichSmartID&>
  { inline size_t operator() ( const LHCb::RichSmartID& s ) const { return (size_t)s.key(); } };
}

// GaudiUtils::Hash
namespace GaudiUtils 
{
  template <> struct Hash<LHCb::RichSmartID>
  { inline size_t operator() ( const LHCb::RichSmartID& s ) const { return (size_t)s.key(); } };
  template <> struct Hash<const LHCb::RichSmartID>
  { inline size_t operator() ( const LHCb::RichSmartID& s ) const { return (size_t)s.key(); } };
  template <> struct Hash<LHCb::RichSmartID&>
  { inline size_t operator() ( const LHCb::RichSmartID& s ) const { return (size_t)s.key(); } };
  template <> struct Hash<const LHCb::RichSmartID&>
  { inline size_t operator() ( const LHCb::RichSmartID& s ) const { return (size_t)s.key(); } };
}

#endif

#endif // RICHKERNEL_RICHSMARTIDHASHFUNCS_H
