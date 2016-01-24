
//--------------------------------------------------------------------------------
/** @file RichSmartIDHashFuncs.h
 *
 *  Header file to define hash functions for RichSmartIDs
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

// std::hash specializations
#include <functional>
namespace std
{
  template <> struct hash<LHCb::RichSmartID>
  { inline constexpr size_t operator() ( const LHCb::RichSmartID& s ) const noexcept { return (size_t)s.key(); } };
  template <> struct hash<const LHCb::RichSmartID>
  { inline constexpr size_t operator() ( const LHCb::RichSmartID& s ) const noexcept { return (size_t)s.key(); } };
  template <> struct hash<LHCb::RichSmartID&>
  { inline constexpr size_t operator() ( const LHCb::RichSmartID& s ) const noexcept { return (size_t)s.key(); } };
  template <> struct hash<const LHCb::RichSmartID&>
  { inline constexpr size_t operator() ( const LHCb::RichSmartID& s ) const noexcept { return (size_t)s.key(); } };
}


#ifdef __GNUC__

// // GNU hash funcs
// #include <ext/hash_map>
// namespace __gnu_cxx
// {
//   template <> struct hash<LHCb::RichSmartID>
//   { inline constexpr size_t operator() ( const LHCb::RichSmartID& s ) const noexcept { return (size_t)s.key(); } };
//   template <> struct hash<const LHCb::RichSmartID>
//   { inline constexpr size_t operator() ( const LHCb::RichSmartID& s ) const noexcept { return (size_t)s.key(); } };
//   template <> struct hash<LHCb::RichSmartID&>
//   { inline constexpr size_t operator() ( const LHCb::RichSmartID& s ) const noexcept { return (size_t)s.key(); } };
//   template <> struct hash<const LHCb::RichSmartID&>
//   { inline constexpr size_t operator() ( const LHCb::RichSmartID& s ) const noexcept { return (size_t)s.key(); } };
// }

// hash_value function for Boost
namespace LHCb
{
  inline std::size_t hash_value( const LHCb::RichSmartID& s )
  {
    boost::hash<LHCb::RichSmartID::KeyType> hasher;
    return hasher( s.key() );
  }
}

// GaudiUtils::Hash
namespace GaudiUtils
{
  template <> struct Hash<LHCb::RichSmartID>
  { inline constexpr size_t operator() ( const LHCb::RichSmartID& s ) const noexcept { return (size_t)s.key(); } };
  template <> struct Hash<const LHCb::RichSmartID>
  { inline constexpr size_t operator() ( const LHCb::RichSmartID& s ) const noexcept { return (size_t)s.key(); } };
  template <> struct Hash<LHCb::RichSmartID&>
  { inline constexpr size_t operator() ( const LHCb::RichSmartID& s ) const noexcept { return (size_t)s.key(); } };
  template <> struct Hash<const LHCb::RichSmartID&>
  { inline constexpr size_t operator() ( const LHCb::RichSmartID& s ) const noexcept { return (size_t)s.key(); } };
}

#endif

#endif // RICHKERNEL_RICHSMARTIDHASHFUNCS_H
