// $Id: CaloHashMap.h,v 1.7 2004-07-27 14:08:58 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2004/07/27 14:01:37  ibelyaev
//  modification/simplification of classes CaloMap&CaloHashMap
//
// ============================================================================
#ifndef KERNEL_CALOHASHMAP_H 
#define KERNEL_CALOHASHMAP_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/stl_hash.h" 
// ============================================================================
// Kernel
// ============================================================================
#include "Kernel/CaloHash.h"
// ============================================================================

/** @class CaloHashMap CaloHashMap.h Kernel/CaloHashMap.h
 *
 *  The public interface is minimal! 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   20/03/2002
 */
template<class KEY, class VALUE>
class CaloHashMap: public std::unary_function<const KEY,VALUE> 
{
public:
  
  /// type of the key 
  typedef  KEY                   Key      ;
  /// type of the mapped value  
  typedef  VALUE                 Value    ;
  /// own type 
  typedef  HashMap<KEY,VALUE>    Self     ;
  
  /// the  map itself
  typedef typename __gnu_cxx::hash_map
  < Key           ,                // Key type 
    Value         ,                // Value type  
    CaloHash<Key> >                // Hash function
  MAP ;
  
  /// the type of iterator 
  typedef  typename MAP::iterator               iterator               ;
  
  /// the type of const_iterator 
  typedef  typename MAP::const_iterator         const_iterator         ;
  
public:
  
  /// Standard constructor (empty)
  HashMap() : m_map  () {};
  
  /// destructor (virtual)
  virtual ~HashMap() { clear() ; };
  
  /// empty container? 
  bool          empty  () const { return m_map.empty() ; }
  
  /** get the size ( == number of elements )
   *  @return number of elements 
   */
  size_t         size  () const { return m_map.size() ; }
  
  /** iterator for sequential access to the content of the "map"
   *  @return begin-iterator (non-const version) 
   */
  iterator       begin ()       { return m_map.begin () ; }
  
  /** iterator for sequential access to the content of the "map"
   *  @return begin-iterator (const version) 
   */
  const_iterator begin () const { return m_map.begin () ; }
  
  /** iterator for sequential access to the content of the "map"
   *  @return end-iterator (non-const version) 
   */
  iterator       end   ()       { return m_map.end   () ; }
  
  /** iterator for sequential access to the content of the "map"
   *  @return end-iterator (const version) 
   */
  const_iterator end   () const { return m_map.end   () ; }
  
  /** access to elements by the key    (non-const!)
   *  @param key  key
   *  @return reference to the value
   */
  inline       Value& operator[] ( const Key& key )
  { return this->operator() ( key ) ; }
  
  /** access to elements by the key    ('pseudo'-const)
   *  @param key  key
   *  @return reference to the value
   */
  inline const Value& operator[] ( const Key& key ) const 
  { return this->operator() ( key ) ; }
  
  /** access with functor interface 
   *  @param key  key
   *  @return reference to the value
   */
  inline       Value& operator() ( const Key& key ) 
  { return m_map[key]; };
  
  /** access with functor interface (pseudo-const)
   *  @param key  key
   *  @return reference to the value
   */
  inline const Value& operator() ( const Key& key ) const 
  {
    Self* self = const_cast<Self*> (this);
    return self->operator() ( key );
  };
  /** remove/erase element from the map (by key) 
   *  @param key key of element to be removed 
   *  @return true if element is removed
   */
  void erase ( const Key& key ) 
  { m_map.erase        ( key ) ; } ;
  
  /** erase the sequence from the map
   *  @param it the "begin" iterator for the sequence 
   *  @param last  the "end"   iterator for the sequence 
   */
  void erase ( iterator it )
  { m_map.erase      ( it ) ; } ;
  
  /** erase the sequence from the map
   *  @param first the "begin" iterator for the sequence 
   *  @param last  the "end"   iterator for the sequence 
   */
  void erase ( iterator first , 
               iterator last  )
  {  m_map.erase( first , last ); } ;
  
  /// clear the content of all containers 
  void clear() { m_map.clear() ; };
  
private:
  
  mutable MAP m_map        ; // the underlying  MAP container
  
}; // end of class CaloHashMap

// ============================================================================
// The End 
// ============================================================================
#endif // KERNEL_CALOHASHMAP_H
// ============================================================================
