// $Id: GiGaAssemblyStore.cpp,v 1.2 2002-12-07 14:36:25 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/01/22 18:24:42  ibelyaev
//  Vanya: update for newer versions of Geant4 and Gaudi
// 
// ============================================================================
// Include files
// STD & STL 
#include <algorithm>
// GiGaCnv
#include "GiGaCnv/GiGaAssembly.h"
#include "GiGaCnv/GiGaAssemblyStore.h"

// ============================================================================
/** @file GiGaAssemblyStore
 * 
 * Implementation file for class : GiGaAssemblyStore
 * 
 * @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 * @date   19/01/2002 
 */
// ============================================================================

// ============================================================================
/** get the instance of the store
 *  return pointer to the store 
 */
// ============================================================================
GiGaAssemblyStore* GiGaAssemblyStore::store() 
{
  static GiGaAssemblyStore worldStore;
  return &worldStore ;
}

// ============================================================================
/** constructor
 */
// ============================================================================
GiGaAssemblyStore::GiGaAssemblyStore() 
  : m_assemblies()
{}

// ============================================================================
/** destructor
 */
// ============================================================================
GiGaAssemblyStore::~GiGaAssemblyStore() { 
  clear(); 
}

//=============================================================================
/** get the assembly by name 
 *  @param name name of the assembly
 */
//=============================================================================
GiGaAssembly*  GiGaAssemblyStore::assembly
( const std::string& Name ) 
{  
  ///
  for( Assemblies::reverse_iterator a = m_assemblies.rbegin() ; 
       m_assemblies.rend() != a ; ++a )
    { if( 0 != *a && (*a)->name() == Name ) { return *a ; } }
  ///
  return 0 ;
}

// ============================================================================
/** add assembly to the store 
 *  @param  a assembly to be added 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaAssemblyStore::addAssembly 
( GiGaAssembly* a  ) 
{
  if( 0 == a ) { return StatusCode::FAILURE ; }
  m_assemblies.push_back( a ) ;
  return StatusCode::SUCCESS ;
}

// ============================================================================
/** remove assembly from the store 
 *  @param a assembly to be removed 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaAssemblyStore::removeAssembly ( GiGaAssembly* a )
{
  ///
  Assemblies::iterator it = 
    std::find( m_assemblies.begin () , m_assemblies.end   () , a );
  ///
  if( m_assemblies.end() != it ) { m_assemblies.erase( it ) ; }
  //
  return StatusCode::SUCCESS;  
}

// ============================================================================
/** clear the store
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaAssemblyStore::clear() 
{
  while( !m_assemblies.empty() )
    {
      Assemblies::iterator it = m_assemblies.begin() ;      
      GiGaAssembly*        a  = *it                  ;
      m_assemblies.erase( it ) ;      
      if( 0 != a ) { delete a  ; }
    }
  return StatusCode::SUCCESS ;
}

// ============================================================================
// The End 
// ============================================================================
