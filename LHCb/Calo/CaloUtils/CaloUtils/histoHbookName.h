// $Id: histoHbookName.h,v 1.4 2002-04-07 18:05:40 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2002/04/05 09:32:18  ibelyaev
//  split histoHbookName.h into histoHbookName.h and dirHbookName.h
//
// Revision 1.2  2002/04/04 13:58:05  ibelyaev
//  add 'dirHbookName(...)' function
//
// Revision 1.1.1.1  2001/11/02 14:39:53  ibelyaev
// New package: The first commit into CVS
//
// Revision 1.2  2001/07/12 21:54:23  ibelyaev
// adaptation for Win2K
//
// Revision 1.1  2001/07/06 11:18:16  ibelyaev
// update to use histoHbookName function
//
// ============================================================================
#ifndef CALOUTILS_HISTOHBOOKNAME_H 
#define CALOUTILS_HISTOHBOOKNAME_H 1
/// ===========================================================================
// Include files
#include <cstdlib>
#include <string>
#include <algorithm> 
#include <functional> 
// CaloUtils 
#include "CaloUtils/dirHbookName.h"

/** @file histoHbookName.h
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

/** @fn histoHbookName
 *  
 *  simple function to convert any valid Gaudi address(name in Transient Store)
 *  to address, which is simultaneously valid for Hbook histogram
 *
 *    examples:  
 *   "verylongname/116"  -->  "verylong/name/116"
 *   "verylongname"      -->  "verylong/name/1"
 *   "somename/here/"    -->  "somename/here/1"
 *   "directory/object"  -->  "director/y/object/1"
 *
 *  @param   old    old address 
 *  @param   maxLen maximum allowed length of directory name (8 for Hbook) 
 *  @return  new  address 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   06/07/2001
 */
inline std::string histoHbookName 
( const std::string& addr       , 
  const int          maxLen = 8 )
{
  // ignore empty locations 
  if( addr.empty() ) { return std::string(); }
  //
  std::string old( addr );
  { // make valid histogram ID (integer)  
    std::string::size_type pos 
      = old.find_last_of( '/' );
    if      ( std::string::npos == pos ) { old += "/1" ; }
    else if ( old.size() - 1    == pos ) { old += '1'  ; }
    else
      {
        const int id = 
			atoi( std::string( old , pos + 1 , std::string::npos ).c_str() );
        if( 0 == id ) { old+="/1"; }
      }
  }
  // remove long names
  if( 0 < maxLen &&  maxLen < (int) old.size() ) 
    { 
      std::string::iterator p1,p2;
      p1 = old.begin();
      const char sep('/');
      while( old.end() != p1 )
        {
          p1 = 
            std::find_if( p1        , 
                          old.end() , 
                          std::bind2nd(std::not_equal_to<char>(),sep));
          p2 = std::find( p1 , old.end() , sep ) ;
          if( ( p2 - p1 ) <= (int) maxLen  ) { p1 = p2 ; continue ; }
          old.insert( p1 + maxLen , sep ) ;  
          p1 = old.begin() ; 
        }
    }
  //
  return old;
};

// ============================================================================
// The End 
// ============================================================================
#endif ///< CALOUTILS_HISTOHBOOKNAME_H
// ============================================================================
