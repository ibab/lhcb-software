// $Id: CaloMap.h,v 1.5 2004-07-27 14:01:37 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef KERNEL_CALOMAP_H 
#define KERNEL_CALOMAP_H 1
// ============================================================================
// Include files
// ============================================================================
#include <map>                   
// ============================================================================

/** @class CaloMap CaloMap.h Kernel/CaloMap.h
 *
 *  The public interface is minimal
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   20/03/2002
 */
template<class KEY, class VALUE>
class CaloMap 
{  
public:
  
  /// type of the key 
  typedef  KEY                                  Key                    ;
  /// type of the mapped value  
  typedef  VALUE                                Value                  ;
  /// the  map itself
  typedef  std::map<Key,Value>                  MAP                    ;
  /// the type of iterator 
  typedef  typename MAP::iterator               iterator               ;
  /// the type of const_iterator 
  typedef  typename MAP::const_iterator         const_iterator         ;
  /// the type of iterator 
  typedef  typename MAP::reverse_iterator       reverse_iterator       ;
  /// the type of const_iterator 
  typedef  typename MAP::const_reverse_iterator const_reverse_iterator ;
  
public:
  
  /// Standard constructor
  Map() : m_map  () {};
  
  /// destructor (virtual)
  virtual ~Map() { clear() ; };
  
  /** get the size ( == number of elements )
   *  @return number of elements 
   */
  unsigned int   size  () const { return m_map.size() ; }
  
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
  
  /** iterator for sequential access to the content of the "map"
   *  @return reverse begin-iterator (non-const version) 
   */
  reverse_iterator       rbegin ()       { return m_map.rbegin () ; }
  
  /** iterator for sequential access to the content of the "map"
   *  @return reverse begin-iterator (const version) 
   */
  const_reverse_iterator rbegin () const { return m_map.rbegin () ; }
  
  /** iterator for sequential access to the content of the "map"
   *  @return reverse end-iterator (non-const version) 
   */
  reverse_iterator       rend   ()       { return m_map.rend   () ; }
  
  /** iterator for sequential access to the content of the "map"
   *  @return reverse end-iterator (const version) 
   */
  const_reverse_iterator rend   () const { return m_map.rend   () ; }
  
  /** access to elements by the key    (non-const!)
   *  @param key  key
   *  @return reference to the value
   */
  inline       Value& operator[] ( const Key& key ) 
  { return m_map[key]; };
  
  /** access to elements by the key    ('pseudo'-const)
   *  @param key  key
   *  @return reference to the value
   */
  inline const Value& operator[] ( const Key& key ) const 
  { return m_map[key]; };
  
  /** access with functor interface 
   *  @param key  key
   *  @return reference to the value
   */
  inline       Value& operator() ( const Key& key ) 
  { return (*this)[ key ] ; };
  
  /** access with functor interface (pseudo-const)
   *  @param key  key
   *  @return reference to the value
   */
  inline const Value& operator() ( const Key& key ) const 
  { return (*this)[ key ] ; };
  
  /** remove/erase element from the map (by key) 
   *  @param key key of element to be removed 
   *  @return true if element is removed
   */
  void erase ( const Key& key ) 
  { m_map.erase  ( key ) ; } ;
  
  /** erase the sequence from the map
   *  @param it the "begin" iterator for the sequence 
   *  @param last  the "end"   iterator for the sequence 
   */
  void erase ( iterator it )
  { m_map.erase  ( it ); };
  
  /** erase the sequence from the map
   *  @param first the "begin" iterator for the sequence 
   *  @param last  the "end"   iterator for the sequence 
   */
  void erase ( iterator first , 
               iterator last  )
  { m_map.erase ( first , last ); };
  
  /// clear the content of all containers 
  void clear() 
  { m_map.clear() ; };
  
  // empty map ??
  bool empty() const { return 0 == m_map.size() ; }
  
private:
  
  mutable MAP m_map        ; // the underlying  MAP container
  
}; // end of class CaloMap 

// ============================================================================
// The End 
// ============================================================================
#endif // KERNEL_CALOMAP_H
// ============================================================================
