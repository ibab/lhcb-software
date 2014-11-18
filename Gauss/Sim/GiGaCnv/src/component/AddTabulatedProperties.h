// $Id: AddTabulatedProperties.h,v 1.3 2002-11-06 16:56:28 sponce Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2001/11/19 18:27:00  ibelyaev
//  bux fix and the new converter for catalogs
//
// ============================================================================
#ifndef     GIGACNV_ADDTABULATEDPROPERTIES_H
#define     GIGACNV_ADDTABULATEDPROPERTIES_H
/// GaudiKernel
#include "GaudiKernel/StatusCode.h"
/// DetDesc
#include "DetDesc/Material.h"
#include "DetDesc/TabulatedProperty.h"
/// Geant4 
#include "G4MaterialPropertiesTable.hh"

/** @function AddTabulatedProperty AddTabnulatedProperties.h 
 *  
 *  The simple helpful function for convertion of tabulated properties 
 *  into Geant4 
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date  xx/xx/xxxx
 *
 *  @param prop   pointer to tabulated property
 *  @param tabTo  Geant4 material property table 
 *  @return status code  
 */
inline StatusCode AddTabulatedProperty 
( const TabulatedProperty*   prop , 
  G4MaterialPropertiesTable* tabTo   )
{
  if ( 0 == prop || 0 == tabTo    ) { return StatusCode::FAILURE; }
  else if ( prop->table().empty() ) { return StatusCode::SUCCESS; }
  ///
  typedef TabulatedProperty::Table Table;
  const   Table& table    = prop->table();
  ///
  const Table::size_type size = table.size (); 
  double* vx = new double[ size ] ;
  double* vy = new double[ size ] ;
  for( unsigned int index = 0 ; index < size ; ++index )
    {
      vx[ index ] = table[ index ].first  ; 
      vy[ index ] = table[ index ].second ;
    }
  /// 
  tabTo->AddProperty( prop->type().c_str() , vx , vy , size );
  ///
  delete [] vx ;
  delete [] vy ;
  ///
  return StatusCode::SUCCESS;
}


/** @function AddTabulatedProperties AddTabnulatedProperties.h 
 *  
 *  The simple helpful function for convertion of tabulated properties 
 *  into Geant4 
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date  xx/xx/xxxx
 *
 *  @param tavFrom  pointer to tabulated property
 *  @param tabTo    Geant4 material property table 
 *  @return status code  
 */
template<class TYPE>
inline StatusCode AddTabulatedProperties
( const TYPE&                tabFrom , 
  G4MaterialPropertiesTable* tabTo   )
{
  if( 0 == tabTo ) { return StatusCode::FAILURE; }
  /// 
  typedef typename TYPE::const_iterator iterator; 
  ///
  for( iterator it= tabFrom.begin() ; tabFrom.end() != it ; ++it ) 
    {
      StatusCode sc = AddTabulatedProperty( *it , tabTo ); 
      if( sc.isFailure() ) { return sc ; } 
    }
  ///
  return StatusCode::SUCCESS;
}

// ============================================================================
// The End 
// ============================================================================
#endif  //  GIGACNV_ADDTABULATEDPROPERTIES_H
// ============================================================================
