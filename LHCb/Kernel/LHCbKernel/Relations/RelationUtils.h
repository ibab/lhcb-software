// $Id: RelationUtils.h,v 1.2 2002-04-03 15:35:18 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef RELATIONS_RELATIONUTILS_H 
#define RELATIONS_RELATIONUTILS_H 1
// Include files
// STD & STL
#include <string>
// GaudiKernel
#include "GaudiKernel/IInterface.h"    // definition of type InterfaceID 
#include "GaudiKernel/ClassID.h"       // definition of type CLID 

namespace Relations
{
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
   *  @param idTo     ID for the second("TO")   object
   *  @param weight   ID of weight class 
   *  @param major    interface major version 
   *  @param minor    interface minor version 
   */
  InterfaceID interfaceID ( const std::string&  id           ,
                            const unsigned long idFrom       ,
                            const unsigned long idTo         ,
                            const unsigned long weight  = 0  ,
                            const unsigned long major   = 0  , 
                            const unsigned long minor   = 0  ) ;
  
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
   *  @param idTo     ID for teh second("TO")   object
   *  @param weight   ID of weight class 
   *  @param major    major version 
   *  @param minor    minor version 
   */
  CLID        clid        ( const std::string&  id           ,
                            const unsigned long idFrom       ,
                            const unsigned long idTo         ,
                            const unsigned long weight   = 0 , 
                            const unsigned long major    = 0 ,
                            const unsigned long minor    = 0 ) ;
  
}; // end of namespace RelationUtils

// ============================================================================
// The End 
// ============================================================================
#endif // RELATIONS_RELATIONUTILS_H
// ============================================================================
