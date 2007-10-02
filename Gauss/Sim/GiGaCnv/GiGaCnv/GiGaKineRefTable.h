// $Id: GiGaKineRefTable.h,v 1.7 2007-10-02 13:14:50 gcorti Exp $ 
#ifndef GIGACNV_GIGAKINEREFTABLE_H
#define GIGACNV_GIGAKINEREFTABLE_H 1

// Include files

// STD & STL
#include "GiGa/GiGaHashMap.h"
#include <functional>

// GiGaCnv
#include "GiGaCnv/GiGaKineRefTableEntry.h"

/** @class GiGaKineRefTable GiGaKineRefTable.h GiGaCnv/GiGaKineRefTable.h
 *
 *  An auxillary class to keep the relations between
 *  Geant4 track/trajectory TrackID identifier and
 *  the converted MCParticle object
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   21/07/2001
 */

class GiGaKineRefTable:
  public std::unary_function<const int,GiGaKineRefTableEntry>
{
public:
  ///
  typedef int                                    Index ;
  typedef GiGaKineRefTableEntry                  Entry ;
  ///
  typedef GiGaHashMap<int,GiGaKineRefTableEntry> Table ;

public:

  /// constructor
  GiGaKineRefTable();

  /// destructor(virtual)
  virtual ~GiGaKineRefTable();

  /** access to the elemements  (non-const version)
   *  @param index index of the element
   *  @return reference to the element
   */
  inline       Entry& operator()       ( const Index& index )
  { return m_table[ index ] ; };
  
  /** access to the elemements  (fictive "const" version)
   *  @param index index of the element
   *  @return reference to the element
   */
  inline const Entry& operator()       ( const Index& index ) const
  { return m_table[ index ] ; };
  
  /** access to the elemements  (non-const version)
   *  @param index index of the element
   *  @return reference to the element
   */
  inline       Entry& operator[]       ( const Index& index )
  { return (*this)( index ); };
  
  /** access to the elemements  (fictive "const" version)
   *  @param index index of the element
   *  @return reference to the element
   */
  inline const Entry& operator[]        ( const Index& index ) const
  { return (*this)( index ); } ;
  
  /// clear the container
  inline void clear() { m_table.clear() ; }
  
  /// resize the container
  inline void resize ( const unsigned long /* size */  )
  { /* m_table.reserve  ( size ) */ ; };
  
  /// resize the container
  inline void reserve ( const unsigned long /* size */ )
  { /* m_table.reserve ( size ); */ };
  
private:

  /// copy constructor is private
  GiGaKineRefTable( const GiGaKineRefTable& );

  /// assignement is private
  GiGaKineRefTable& operator=( const GiGaKineRefTable& );

private:

  mutable Table   m_table;

};

#endif ///< GIGACNV_GIGAKINEREFTABLE_H
