// $Id: DirHbookName.h,v 1.1.1.1 2002-05-26 12:47:06 ibelyaev Exp $
// =============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// =============================================================================
// $Log: not supported by cvs2svn $
// =============================================================================
#ifndef DETDESCCHECKS_DIRHBOOKNAME_H 
#define DETDESCCHECKS_DIRHBOOKNAME_H 1

namespace Local
{
  /** Simple function to convert any valid Gaudi 
   *  address(name in Transient Store)
   *  to address, which is simultaneously valid for Hbook directory
   *
   *    examples:  
   *   "verylongname"  -->  "verylong/name"
   *
   *  The function is imported from Calo/CaloUtils package 
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
    ///
    return old;
  }; 
  
}; // end of namespace Local 
// =============================================================================


// =============================================================================
// The END 
// =============================================================================
#endif // DETDESCCHECKS_DIRHBOOKNAME_H
// =============================================================================
