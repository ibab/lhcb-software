// $Id: GiGaKineRefTable.h,v 1.4 2002-03-13 15:37:57 ibelyaev Exp $ 
// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.2  2001/08/12 17:24:50  ibelyaev
/// improvements with Doxygen comments
///
/// Revision 1.1  2001/07/24 11:13:54  ibelyaev
/// package restructurization(III) and update for newer GiGa
///
// ============================================================================
#ifndef GIGACNV_GIGAKINEREFTABLE_H 
#define GIGACNV_GIGAKINEREFTABLE_H 1
// ============================================================================
// STD & STL 
#include <functional>

/// hash table! 
#ifdef WIN32
#include "GaudiKernel/HashTable"  // use hash map from Markus 
template<> class HashTableHasher<int> : 
  public std::unary_function<int,size_t>  {
public:
  inline size_t operator()(const int key) 
  { return 0 < key ? (size_t) key : (size_t) ( -1 * key ) ; }
};
#else  
#include <hash_map>  // use standard hash map 
#endif

/// GiGaCnv 
#include "GiGaCnv/GiGaKineRefTableEntry.h"

/** @class GiGaKineRefTable GiGaKineRefTable.h GiGaCnv/GiGaKineRefTable.h
 *  
 *  An auxillary class to keep the relations between 
 *  Geant4 track/trajectory TrackID identifier and 
 *  the converted MCParticle object 
 *
 *  @warning the implementation of the class is platform-dependent! 
 * 
 *  @author Ivan Belyaev
 *  @date   21/07/2001
 */

class GiGaKineRefTable:
  public std::unary_function<const int,GiGaKineRefTableEntry&>
{
public:
  ///
  typedef int                   Index ;
  typedef GiGaKineRefTableEntry Entry ;
  ///
#ifdef WIN32 
  typedef HashTable<Index,Entry>     Table ;
#else 
  typedef std::hash_map<Index,Entry> Table ;
#endif
  
public:
  
  /// constructor
  GiGaKineRefTable();
  
  /// destructor(virtual)
  virtual ~GiGaKineRefTable();
  
  /** access to the elemements ( non-const version) 
   *  @param index index of the element 
   *  @return reference to the element 
   */
  inline       Entry& operator() ( const Index& index ) 
  { 
#ifdef WIN32
    Table::value_type* value = m_table.find( index );
    if( 0 != qvalue ) { return value->second;}
    /// insert the element 
    m_table.insert( index , Entry() );
    value = m_table.find( index );
    return value->second ;
#else
    return m_table[ index ] ;
#endif 
  };
  
  /** access to the elemements ( non-const version) 
   *  @param index index of the element 
   *  @return reference to the element 
   */
  inline       Entry& operator[] ( const Index& index ) 
  { return (*this)( index ); };
  
  /** clear the container
   */
  inline void clear() { m_table.clear() ; }
  
  /** resize the container
   */
  inline void resize ( const unsigned long size ) 
  {
#ifdef WIN32 
    m_table.reserve ( size ) ;
#else 
    m_table.resize  ( size ) ;
#endif 
  };
  
  /** resize the container
   */
  inline void reserve ( const unsigned long size ) { resize( size ); };
  
private:

  /// copy constructor is private
  GiGaKineRefTable( const GiGaKineRefTable& );

  /// assignement is private
  GiGaKineRefTable& operator=( const GiGaKineRefTable& );

private:
  
  mutable Table   m_table;

};

// ============================================================================
#endif ///< GIGACNV_GIGAKINEREFTABLE_H
// ============================================================================
