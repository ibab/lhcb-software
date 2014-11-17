// $Id: GiGaUtil.cpp,v 1.7 2007-01-12 15:45:03 ranjard Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2002/05/07 12:21:34  ibelyaev
//  see $GIGAROOT/doc/release.notes  7 May 2002
//
// ============================================================================
/// STD & STL 
#include <algorithm> 
/// GaudiKernel
//#include "GaudiKernel/ISvcLocator.h"
//#include "GaudiKernel/IObjManager.h"
/// GiGa 
#include "GiGa/GiGaUtil.h"

// ============================================================================
/** @file 
 *  Implementation file for functions from  GiGaUtil namespace 
 * 
 *  @author Vanya  Belyaev
 *  @date 23/07/2001 
 */
// ============================================================================

// ============================================================================
/** split a "Type/Name" string into "Type" and "Name"
 *  
 *  rules:
 * -  "Type/Name"  =  "Type" , "Name"
 * -  "Type"       =  "Type" , "Type"
 * -  "Type/"      =  "Type" , "Type"
 * 
 *  error conditions 
 *
 *  -  empty input string 
 *  -  more than 1 separator ('/') 
 *  -  empty type ("/Name")
 *
 *  @param TypeAndName   string to be splitted 
 *  @param Type  returned "Type" 
 *  @param Name  returned "Name" 
 *  @return status code
 */  
// ============================================================================
StatusCode GiGaUtil::SplitTypeAndName
( const std::string& TypeAndName ,
  std::string      & Type        , 
  std::string      & Name        )
  
{
  /// clear output strings
  Type.erase();
  Name.erase();
  ///
  if( TypeAndName.empty() ) { return StatusCode::FAILURE ; } ///< RETURN
  /// 
  const unsigned int numSep = 
    std::count( TypeAndName.begin() ,
                TypeAndName.end  () , '/' );
  if      ( 1  <  numSep  ) { return StatusCode::FAILURE ; } ///< RETURN 
  else if ( 0 ==  numSep  ) 
    {
      Type = TypeAndName ;
      Name = TypeAndName ;
      return StatusCode::SUCCESS ;                           ///< RETURN
    }
  else 
    {      
      /// look for '/' in the string 
      std::string::size_type pos = TypeAndName.find('/');
      if( std::string::npos  == pos ) { return StatusCode::FAILURE ; }
      /// construct the name and the type
      Type = std::string( TypeAndName , 0   , pos );
      if( Type.empty()    ) { return StatusCode::FAILURE ; }  ///< RETURN 
      Name = std::string( TypeAndName , pos + 1 , std::string::npos ) ;
      if( Name.empty()    ) { Name = Type ; }
    }  
  ///
  return StatusCode::SUCCESS; 
  ///
}
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
