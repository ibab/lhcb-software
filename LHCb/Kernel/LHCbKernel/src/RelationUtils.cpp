// $Id: RelationUtils.cpp,v 1.3 2002-05-24 18:36:33 ibelyaev Exp $
// ============================================================================
// CVS tag $name:$
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2002/04/03 15:35:19  ibelyaev
// essential update and redesing of all 'Relations' stuff
// 
// ============================================================================
// Include files
// STD & STL
#include <string>
#include <stdio.h>
// GaudiKernel
#include "GaudiKernel/IInterface.h"   
#include "GaudiKernel/ClassID.h"      
// Relations
#include "Relations/RelationUtils.h"

// ============================================================================
/** @file RelationUtils.cpp
 *  
 *  Implementation file for method from namespace Relation
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @adte 28/01/2002 
 */
// ============================================================================

// ============================================================================
/** @fn interfaceID 
 *   
 *  funtion to create the uniqie relation 
 *  interfaceID from interface name, CLIDs and types 
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date   28/01/2002
 * 
 *  @param name     interface name
 *  @param idFrom   ID for the first ("FROM") object  
 *  @param idTo     ID for teh second("TO")   object
 *  @param weight   ID of weight class 
 *  @param major    interface major version 
 *  @param minor    interface minor version 
 */
// ============================================================================
InterfaceID Relations::interfaceID 
( const std::string&  id       ,
  const unsigned long idFrom   ,
  const unsigned long idTo     ,
  const unsigned long weight   ,
  const unsigned long major    , 
  const unsigned long minor    )
{
  std::string name( id ) ;
  char a[128];
  name     += "<" + std::string( a , a + sprintf( a , "%d" , (int) idFrom ) ) ;
  name     += "," + std::string( a , a + sprintf( a , "%d" , (int) idTo   ) ) ;
  if( 0 != weight  ) 
    { name += "," + std::string( a , a + sprintf( a , "%d" , (int) weight ) ) ;}
  name += ">" ;
  ///
  return InterfaceID( name.c_str() , major , minor );
};

// ============================================================================
/** @fn clid 
 *   
 *  function to create the uniqie relation 
 *  clid from object name, CLIDs and types 
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date   28/01/2002
 * 
 *  @param name     object name
 *  @param idFrom   ID for the first ("FROM") object  
 *  @param idTo     ID for the second("TO")   object
 *  @param weight   ID of weight class 
 *  @param major    major version 
 *  @param minor    minor version 
 */
// ============================================================================
CLID        Relations::clid        
( const std::string&  id       ,
  const unsigned long idFrom   ,
  const unsigned long idTo     ,
  const unsigned long weight   , 
  const unsigned long major    , 
  const unsigned long minor    ) 
{
  std::string name( id ) ;
  char a[128];
  name     += "<" + std::string( a , a + sprintf( a , "%d" , (int) idFrom ) ) ;
  name     += "," + std::string( a , a + sprintf( a , "%d" , (int) idTo   ) ) ;
  if( 0 != weight  ) 
    { name += "," + std::string( a , a + sprintf( a , "%d" , (int) weight ) ) ;}
  name += ">" ;
  //
  CLID cl = InterfaceID( name.c_str() , major , minor ).id() ;
  // set ObjectVector bit to NULL 
  cl = ~CLID_ObjectVector & cl ;
  // set ObjectList   bit to NULL 
  cl = ~CLID_ObjectList   & cl ;
  //
  return cl ;
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
