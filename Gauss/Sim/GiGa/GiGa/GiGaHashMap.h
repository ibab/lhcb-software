// $Id: GiGaHashMap.h,v 1.4 2007-10-03 15:21:09 gcorti Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2003/07/07 16:48:09  ranjard
// v14r2 - fix for gcc 3.2
//
// Revision 1.2  2002/12/03 21:46:59  ibelyaev
//  small upgrade to improve CaloSim code
//
// Revision 1.1  2002/05/04 20:20:11  ibelyaev
//  see $GIGAROOT/doc/release.notes (4 May 2002)
//
// ============================================================================
#ifndef GIGA_GIGAHASHMAP_H 
#define GIGA_GIGAHASHMAP_H 1
// Include files
#ifdef WIN32
#include "GaudiKernel/HashMap.h" // GaudiKernel (only for Visual-C Win32)
#include <vector>            
#else 
#if defined (__GNUC__) && ( __GNUC__ <= 2 )
#include <hash_map>
#else
#include <ext/hash_map>
using __gnu_cxx::hash_map;
using __gnu_cxx::hash;
#endif
#endif 
#include "GiGa/GiGaHash.h"

/** @class GiGaHashMap GiGaHashMap.h GiGa/GiGaHashMap.h
 *
 *  @brief trick to hide std::hash_map on Win32
 *
 *  An auxillary class to hide underlying std::hash_map 
 *  implementation for Microsoft Visual-C++ (Win32)  
 *
 *  The code actually is imported from LHCb Calo software.
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   20/03/2002
 */
template<class KEY, class VALUE>
class GiGaHashMap 
{
public:
  
  /// type of the key 
  typedef  KEY                      Key      ;
  /// type of the mapped value  
  typedef  VALUE                    Value    ;
  
#ifdef WIN32 
  /** the  map itself
   *  @warning the actual type is platform-dependent!
   */
  typedef  GaudiUtils::HashMap<Key,Value,GiGaHash<Key> > Map  ; 
#else 
  /** the  map itself
   *  @warning the actual type is platform-dependent!
   */
  typedef  hash_map<Key,Value> Map      ;
#endif
  
  /** the type of iterator 
   *  @warning the actual type is platform-dependent!
   */
  typedef typename Map::iterator iterator;
  
  /** the type of const_iterator 
   *  @warning the actual type is platform-dependent!
   */
  typedef typename Map::const_iterator const_iterator ;
  
public:
  
  /// Standard constructor
  GiGaHashMap () 
    : m_map   () 
  {}
  
  /// destructor (virtual)
  virtual ~GiGaHashMap() { clear() ; };
  
  /** access to elements by the key    (non-const!)
   *  for Linux the implementation is trivial
   * 
   *  @warning the implementation is platform-dependent!
   *
   *   - for Linux the implementation is trivial 
   *   - for WIN32 some tricks are applied 
   *
   *  @param key  key
   *  @return reference to the value
   */
  inline Value& operator() ( const Key& key )
  { return m_map[key]; };

  /** access to elements by the key    (non-const!)
   *  for Linux the implementation is trivial
   * 
   *  @warning the implementation is platform-dependent!
   *
   *   - for Linux the implementation is trivial 
   *   - for WIN32 some tricks are applied 
   *
   *  @param key  key
   *  @return reference to the value
   */
  inline Value& operator[] ( const Key& key ) { return (*this)( key ); }
  
  /** erase the sequence from the map
   *  @warning implementation  is platform-dependent
   *  @param first the "begin" iterator for the sequence 
   *  @param last  the "end"   iterator for the sequence 
   */
  void erase( iterator first , 
              iterator last  )
  {
    m_map.erase( first , last ); 
  };
  
  /** erase the sequence from the map
   *  @warning implementation  is platform-dependent
   *  @param it the "begin" iterator for the sequence 
   *  @param last  the "end"   iterator for the sequence 
   */
  void erase( iterator it )
  {
    m_map.erase  ( it        ); 
  };
  
  /** remove/erase element from the map (by key) 
   *  @warning implementation  is platform-dependent
   *  @param key key of element to be removed 
   *  @return true if element is removed
   */
  void erase( const Key& key ) 
  { 
    m_map.erase ( key ) ;
  };

  /// clear the content of all containers 
  void clear() { m_map.clear() ; }
  
  /** get the size ( == number of elements )
   *  @return number of elements 
   */
  unsigned int size() const { return m_map.size() ; }

  /** iterator for sequential access to the content of the "map"
   *  @return begin-iterator (non-const version) 
   */
  typename Map::iterator begin () { return m_map.begin () ; }

  /** iterator for sequential access to the content of the "map"
   *  @return begin-iterator (const version) 
   */
  typename Map::const_iterator begin () const {
    return m_map.begin ();
  }

  /** iterator for sequential access to the content of the "map"
   *  @return end-iterator (non-const version) 
   */
  typename Map::iterator end() { return m_map.end   () ; }

  /** iterator for sequential access to the content of the "map"
   *  @return end-iterator (const version) 
   */
  typename Map::const_iterator end() const { return m_map.end() ; }

private:

  // the map iteself 
  Map  m_map  ; ///< the map itself 

};

// ============================================================================
// The End 
// ============================================================================
#endif // GIGA_GIGAHASHMAP_H
// ============================================================================
