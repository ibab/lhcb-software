#ifndef     GIGA_ADDTABULATEDPROPERTIES_H
#define     GIGA_ADDTABULATEDPROPERTIES_H
/// GaudiKernel
#include "GaudiKernel/StatusCode.h"
/// DetDesc
#include "DetDesc/Material.h"
#include "DetDesc/TabulatedProperty.h"
/// Geant4 
#include "G4MaterialPropertiesTable.hh"
///

///
inline StatusCode AddTabulatedProperty ( const TabulatedProperty*   prop , 
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
      vx[ index ] = table[ index ].second ;
    }
  /// 
  tabTo->AddProperty( prop->type().c_str() , vx , vy , size );
  ///
  delete vx ;
  delete vy ;
  ///
  return StatusCode::SUCCESS;
};
///
template<class TYPE>
inline StatusCode AddTabulatedProperties( const TYPE&                tabFrom , 
                                          G4MaterialPropertiesTable* tabTo    )
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
};
///
                                                           
#endif  //  GIGA_ADDTABULATEDPROPERTIES_H
