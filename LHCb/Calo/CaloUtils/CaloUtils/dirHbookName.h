// $Id: dirHbookName.h,v 1.1 2002-04-05 09:32:17 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef CALOUTILS_DIRHBOOKNAME_H 
#define CALOUTILS_DIRHBOOKNAME_H 1
/// ===========================================================================
// Include files
#include <cstdlib>
#include <string>
#include <algorithm> 
// CaloUtils 
#include "CaloUtils/histoHbookName.h"

/** @file dirHbookName.h
 *  
 *  file with definition of 2 functions for handling Hbook 
 *  names for histograms and directories 
 *
 *  @see histoHbookName 
 *  @see dirHbookName 
 *
 *  @authjor Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 04/04/2002
 */

/** @fn dirHbookName
 *  
 *  simple function to convert any valid Gaudi address(name in Transient Store)
 *  to address, which is simultaneously valid for Hbook directory
 *
 *    examples:  
 *   "verylongname"  -->  "verylong/name"
 *
 *  @param   old    old address 
 *  @param   maxLen maximum allowed length of directory name (8 for Hbook) 
 *  @return  new  address 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   06/07/2001
 */
inline std::string dirHbookName 
( const std::string& addr       , 
  const int          maxLen = 8 )
{
  // ignore empty locations 
  if( addr.empty() ) { return std::string(); }
  //
  std::string old( addr );
  // remove long names
  if( 0 < maxLen &&  maxLen < (int) old.size() ) 
    { 
      std::string::iterator p1,p2;
      p1 = old.begin() ;
      while( old.end() != p1 )
        {
          p2 = std::find( p1 + 1 , old.end() , '/' ) ;
          if( ( p2 - p1 ) <= (int) maxLen ) { p1 = p2 ; continue ; }
          old.insert( p1 + maxLen , '/' ) ;  
          p1 = old.begin() ; 
        }
    }
  ///
  return old;
};

// ============================================================================
// The End 
// ============================================================================
#endif ///< CALOUTILS_DIRHBOOKNAME_H
// ============================================================================
