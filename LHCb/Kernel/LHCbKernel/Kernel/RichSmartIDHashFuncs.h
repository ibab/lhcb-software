
//--------------------------------------------------------------------------------
/** @file RichSmartIDHashFuncs.h
 *
 *  Header file to define hash functions for RichSmartIDs
 *
 *  CVS Log :-
 *  $Id: RichSmartIDHashFuncs.h,v 1.1 2006-02-01 14:59:21 papanest Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.2  2005/12/13 17:27:25  jonrob
 *  another update
 *
 *  Revision 1.1  2005/01/13 12:21:18  jonrob
 *  Add new files to CVS
 *
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
namespace __gnu_cxx
{
  template <> struct hash<LHCb::RichSmartID>
  { size_t operator() ( const LHCb::RichSmartID& s ) const { return s.key(); } };
  template <> struct hash<const LHCb::RichSmartID>
  { size_t operator() ( const LHCb::RichSmartID& s ) const { return s.key(); } };
  template <> struct hash<LHCb::RichSmartID&>
  { size_t operator() ( const LHCb::RichSmartID& s ) const { return s.key(); } };
  template <> struct hash<const LHCb::RichSmartID&>
  { size_t operator() ( const LHCb::RichSmartID& s ) const { return s.key(); } };
};
#endif

#endif // RICHKERNEL_RICHSMARTIDHASHFUNCS_H
