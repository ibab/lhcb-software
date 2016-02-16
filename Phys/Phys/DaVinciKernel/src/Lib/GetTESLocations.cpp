// $Id:$ 
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IRegistry.h"
// ============================================================================
// Local
// ============================================================================
#include "Kernel/GetTESLocations.h"
// ============================================================================
/** @file 
 *  Implementation file for class DaVinci::Utils::GetTESLocations
 *
 *  @date 2016-02-16 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 * 
 *  Version     $Revision:$
 *  Last update $Date:$
 *           by $Author:$
 */
// ============================================================================
// destructor 
// ============================================================================
DaVinci::Utils::GetTESLocations::~GetTESLocations()
{
  m_mapping  .clear() ;
  m_locations.clear() ;
}
// ============================================================================
// add TES-location 
// ============================================================================
std::size_t DaVinci::Utils::GetTESLocations::_collect 
( const DataObject* o )
{
  const IRegistry* iregistry = nullptr == o ? nullptr : o->registry() ;
  if ( nullptr == iregistry ) { return 0 ; }
  //
  const std::string& n =  iregistry->identifier() ;
  if ( m_mapping.insert ( { o , n } ).second ) 
  {
    m_locations.push_back ( n ) ;
    return 1 ;
  }
  return 0 ;                  
}
// ============================================================================
//  The END 
// ============================================================================
