// $Id: CaloHashMap.h,v 1.4 2003-06-16 13:27:54 sponce Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2002/04/27 15:31:02  ibelyaev
//  add CaloHash.h for efficient hashing on Win32 platform
//
// Revision 1.2  2002/04/22 16:57:18  ibelyaev
//  new files to eliminate the dependency of 'Event' on 'Calo'
//
// Revision 1.1  2002/04/22 16:49:51  ibelyaev
//  new files to eliminate the dependency of 'Event' on 'Calo'
//
// ============================================================================
#ifndef KERNEL_CALOHASHMAP_H 
#define KERNEL_CALOHASHMAP_H 1
// Include files
#ifdef WIN32
#include "GaudiKernel/HashTable.h" // GaudiKernel (only for Visual-C Win32)
#include "Kernel/CaloHash.h"
#include <vector>            
#else 
#if defined (__GNUC__) && ( __GNUC__ <= 2 )
#include <hash_map>
using std::hash_map;
#else
#include <ext/hash_map>
using __gnu_cxx::hash_map;
#endif
#endif 

/** @class CaloHashMap CaloHashMap.h Kernel/CaloHashMap.h
 *
 *  @brief trick to hide std::hash_map on Win32
 *
 *  An auxillary class to hide underlying std::hash_map 
 *  implementation for Microsoft Visual-C++ (Win32)  
 *
 *  The public interface is minimal! 
 *
 *  @warning the implementation is platform dependent!!!
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   20/03/2002
 */
template<class KEY, class VALUE>
class CaloHashMap 
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
  typedef  HashTable<Key,Value,CaloHash<Key> > Map  ; 
  typedef  std::vector<Key>                    Keys ;
#else 
  /** the  map itself
   *  @warning the actual type is platform-dependent!
   */
  typedef  hash_map<Key,Value> Map      ;
#endif
  
  /** the type of iterator 
   *  @warning the actual type is platform-dependent!
   */
  typedef  typename Map::iterator iterator;
  
  /** the type of const_iterator 
   *  @warning the actual type is platform-dependent!
   */
  typedef  typename Map::const_iterator const_iterator ;
  
public:
  
  /// Standard constructor
  CaloHashMap () 
    : m_map   () 
#ifdef WIN32 
    , m_keys  () 
#endif 
  {}
  
  /// destructor (virtual)
  virtual ~CaloHashMap() { clear() ; };
  
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
  inline Value& operator[] ( const Key& key )
#ifdef WIN32 
  {
    // find the key 
    Map::value_type* value = m_map.find( key ) ;
    if( 0 != value ) { return value->second ; }    // RETURN ! 
    // insert the key 
    m_map.insert( key , Value() );
    value = m_map.find( key ) ;
    return value->second ;
  };
#else 
  { return m_map[key]; };
#endif
  
  /** erase the sequence from the map
   *  @warning implementation  is platform-dependent
   *  @param first the "begin" iterator for the sequence 
   *  @param last  the "end"   iterator for the sequence 
   */
  void erase( iterator first , 
              iterator last  )
  {
#ifndef WIN32 
    m_map.erase( first , last ); 
#else 
    if( !m_keys.empty() ) { m_keys.clear() ; }
    for( ; first != last ; ++first ) 
      { m_keys.push_back( first->first ); }
    for( Keys::const_iterator ikey = m_keys.begin() ; 
         m_keys.end() != ikey ; ++ikey ) { m_map.remove( *ikey  ) ; }
    m_keys.clear();
#endif
  };
  
  /** erase the sequence from the map
   *  @warning implementation  is platform-dependent
   *  @param it the "begin" iterator for the sequence 
   *  @param last  the "end"   iterator for the sequence 
   */
  void erase( iterator it )
  {
#ifndef WIN32 
    m_map.erase  ( it        ); 
#else 
    m_map.remove ( it->first );
#endif
  };
  
  /** remove/erase element from the map (by key) 
   *  @warning implementation  is platform-dependent
   *  @param key key of element to be removed 
   *  @return true if element is removed
   */
  void erase( const Key& key ) 
  { 
#ifndef WIN32
    m_map.erase ( key ) ;
#else
    m_map.remove( key ) ;
#endif 
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
  typename Map::iterator begin ()       { return m_map.begin () ; }

  /** iterator for sequential access to the content of the "map"
   *  @return begin-iterator (const version) 
   */
  typename Map::const_iterator begin () const { return m_map.begin () ; }

  /** iterator for sequential access to the content of the "map"
   *  @return end-iterator (non-const version) 
   */
  typename Map::iterator end() { return m_map.end   () ; }

  /** iterator for sequential access to the content of the "map"
   *  @return end-iterator (const version) 
   */
  typename Map::const_iterator end() const { return m_map.end   () ; }

private:

  // the map iteself 
  Map  m_map  ; ///< the map itself 

#ifdef WIN32
  // helper container of keys to be removed, see "erase" method 
  Keys m_keys ; ///< helper conatiner to keep the keys to be removed 
#endif 
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // KERNEL_CALOHASHMAP_H
// ============================================================================
