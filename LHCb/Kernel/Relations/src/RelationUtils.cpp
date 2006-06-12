// $Id: RelationUtils.cpp,v 1.6 2006-06-12 15:27:28 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.6 $
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <string>
#include <cstdio>
#include <iostream>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IInterface.h"   
#include "GaudiKernel/ClassID.h"      
// ============================================================================
// Relations
// ============================================================================
#include "Relations/RelationUtils.h"
// ============================================================================

// ============================================================================
/** @file
 *  
 *  Implementation file for method from namespace Relation
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 28/01/2002 
 */
// ============================================================================

// ============================================================================
/**
 *  funtion to create the uniqie relation 
 *  interfaceID from interface name, CLIDs and types 
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date   28/01/2002
 * 
 *  @param id       interface name
 */
// ============================================================================
InterfaceID Relations::interfaceID 
( const std::string&  name     ) 
{ return InterfaceID( name.c_str() , 1 , 0  ); };

// ============================================================================
/**
 *  function to create the uniqie relation 
 *  clid from object name, CLIDs and types 
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date   28/01/2002
 * 
 *  @param id       object name
  */
// ============================================================================
CLID        Relations::clid        
( const std::string&  name     )
{
  CLID cl = Relations::interfaceID( name ).id() ;
  // set ObjectVector bit to NULL 
  cl = ~CLID_ObjectVector & cl ;
  // set ObjectList   bit to NULL 
  cl = ~CLID_ObjectList   & cl ;
  return cl ;
};
// ============================================================================

// ============================================================================
/** @fn guid 
 *  simple funtion to convert CLID (Gaudi unique class identifier) 
 *  to GUID (POOL unique class identifier)
 *
 *  The actual code is stollen from Markus Frank's lines 
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-01-30
 *
 *  @param clid Gaudi unique class identifier 
 *  @return POOL unique class identifier 
 */
// ============================================================================
std::string Relations::guid ( const CLID& clID ) 
{
  char clid_txt[64];
  sprintf ( clid_txt ,"%08X-0000-0000-0000-000000000000", (int) clID );
  return std::string ( clid_txt , 64  ) ;
};
// ============================================================================
/** @fn guid 
 *  simple funtion to convers CLID (Gaudi unique class identifier) 
 *  to GUID (POOL unique class identifier)
 *
 *  The actual code is stollen from Markus Frank's lines 
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-01-30
 *
 *  @param clid class name 
 *  @return POOL unique class identifier 
 */
// ============================================================================
std::string Relations::guid ( const std::string& name  )
{ return Relations::guid( Relations::clid( name ) ) ; } ;


// ============================================================================
// The End 
// ============================================================================
