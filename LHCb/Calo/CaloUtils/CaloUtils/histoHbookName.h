/// ===========================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.2  2001/07/12 21:54:23  ibelyaev
/// adaptation for Win2K
///
/// Revision 1.1  2001/07/06 11:18:16  ibelyaev
/// update to use histoHbookName function
///
/// ===========================================================================
#ifndef HISTOHBOOKNAME_H 
#define HISTstriOHBOOKNAME_H 1
/// ===========================================================================

// Include files
#include <cstdlib>
#include <string>
#include <algorithm> 

/** 
 *  simple function to convert any valid Gaudi address(name in Transient Store)
 *  to address, which is simultaneously valid for Hbook
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
 *  @author Ivan Belyaev
 *  @date   06/07/2001
 */
static inline std::string 
histoHbookName ( const std::string& addr , const int maxLen = 8 )
{
  /// ignore empty locations 
  if( addr.empty() ) { return std::string(); }
  ///
  std::string old( addr );
  { /// make valid histogram ID (integer)  
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
  /// remove long names
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

/// ===========================================================================
#endif ///< HISTOHBOOKNAME_H
/// ===========================================================================
