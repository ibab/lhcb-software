// $Id: TransportSvcFindLocalGI.h,v 1.5 2002-06-21 13:43:44 ocallot Exp $ 
// ============================================================================
#ifndef        __DETDESC_TRANSPORTSVC_TRANSPORTSVCFINDLOCALGI_H__
#define        __DETDESC_TRANSPORTSVC_TRANSPORTSVCFINDLOCALGI_H__ 1

#include <strstream> 

// local
#include "TransportSvc.h" 

/** @file TransportSvcFindLocalGI.cpp 
 *  
 * 
 * simple implementation of TransportSvc::findLocalGI method 
 *  @see TranpsortSvc 
 *  @see ITransportSvc 
 *
 * 
 * @author: Vanya Belyaev
 */

IGeometryInfo*  TransportSvc::findLocalGI ( const HepPoint3D& point1 , 
                                            const HepPoint3D& point2 , 
                                            IGeometryInfo*    gi     ,
                                            IGeometryInfo*    topGi  ) const  {
  if( 0 == gi ) { return 0 ; } 

  bool debug = false;

  /// output :-))
  IGeometryInfo* goodGI = 0  ; 
  
  try {  
    /// find the nearest "upper" volume, which contains the  first point  
    IGeometryInfo* gi1   = gi ;   
    if ( debug ) std::cout << "Original geom " << gi1->lvolumeName() 
                           << " point z = " << point1.z()
                           << std::endl;
    gi1   = gi1->belongsTo( point1, -1 ) ;   
    if ( debug ) std::cout << "  belongsTo geom " << gi1->lvolumeName() 
                           << std::endl;
    {  
      bool loc = false ;  
      for( loc = gi1->isInside( point1 ) ; 
           !loc && (0 != gi1) && (gi != gi1) ; 
           gi1 = gi1->parentIGeometryInfo() ) {
        loc = gi1->isInside( point1 ); 
        if ( debug ) std::cout << " inside loop " <<  loc 
                               << " pointr=" << gi1->lvolumeName() 
                               << std::endl;  
        if ( loc ) break;
      }  
      if( !loc || 0 == gi1 ) { 
        if ( debug ) std::cout << " .. point1 not inside" << std::endl;
        return 0; 
      }   
    }
   if ( debug )  std::cout << "  p1 isInside geom " << gi1->lvolumeName() 
                           << std::endl;
  
    /// find the nearest "upper" volume, which contains the second point  
    IGeometryInfo* gi2   = gi1 ; 
    {  
      bool loc = false ; 
      for( loc = gi2->isInside( point2 ) ; !loc && 0 != gi2 ; 
           gi2 = gi2->parentIGeometryInfo() ) { 
        loc = gi2->isInside( point2 ); 
        if ( debug ) std::cout << "  p2 : " << loc << " vol " 
                               << gi2->lvolumeName() << std::endl;
        if ( loc ) break;
      }  
      if( 0 == gi2 ) { 
        if ( debug ) std::cout << " .. point2 not inside" << std::endl;
        return 0; 
      }
    }
    if ( debug ) std::cout << "  p2 isInside geom " << gi2->lvolumeName() 
                           << std::endl;
    
    /// here both points are located 
    ///  try fo find the "common" location 

    /// if both points are located in the same geometry element
    if( gi1 == gi2 ) {  
      ///  try to find a "good" geometry element in geometry hierarhy
      for( IGeometryInfo* gl = gi1 ; 0 != gl ; 
           gl = gl->parentIGeometryInfo() ) { 
        if ( 0 == gl ) return 0;
        if( goodLocalGI( point1 , point2 , gl ) ) { 
          if ( debug ) std::cout << "  goodLocalGI geom " << gl->lvolumeName()
                                 << std::endl;
          return gl; 
        } 
      }
      /// we have failed to find "good" element 
     if ( debug )  std::cout << "  failed to find good element" << std::endl;
      return 0;                      /// RETURN !!! 
    }
    
    
    /// points are located in different geometry elements, 
    /// or geometry element is not good!
    
    /// "top" geometry tree for the first point 
    IGeometryInfo* vgi;

    m_vGi1.clear();   
    for( vgi = gi1 ; (0 != vgi); vgi = vgi->parentIGeometryInfo() ) { 
      if ( debug ) std::cout << "  store parent 1 " << vgi->lvolumeName() 
                             << std::endl;
      m_vGi1.push_back( vgi ); 
      if ( topGi == vgi ) break;
    }
    
    /// "top" geometry tree for the second point 
    m_vGi2.clear(); 
    for( vgi = gi2 ; (0 != vgi); vgi = vgi->parentIGeometryInfo()  ) { 
      if ( debug ) std::cout << "  store parent 2 " << vgi->lvolumeName() 
                             << std::endl;
      m_vGi2.push_back( vgi ); 
      if ( topGi == vgi ) break;
    }  
    
    /// "common" top tree for both points 
    m_vGi.clear(); 
    {
      rGeoIt it1 = m_vGi1.rbegin(); 
      rGeoIt it2 = m_vGi2.rbegin(); 
      while( ( m_vGi1.rend() !=  it1 ) && 
             ( m_vGi2.rend() !=  it2 ) && 
             (      0        != *it1 ) && 
             (      0        != *it2 ) &&  
             (   *it1        == *it2 )    ) { 
       if ( debug )  std::cout << "  store common " << (*it1)->lvolumeName()
                               << std::endl;
        m_vGi.push_back( *it1 ) ; 
        ++it1 ; 
        ++it2 ; 
      }
    }
    
    /// no common elements found !!!
    if( m_vGi.empty() ) { 
      if ( debug ) std::cout << " .. no common element" << std::endl;
      return 0 ; 
    } 

    /// common "top" tree is found! 
    {
      if ( debug ) std::cout << "  extend tree" << std::endl;
      
      /// extend it to lower level!   ???? DO WE NEED IT ????  YES!!! 
      IGeometryInfo*  gl = *(m_vGi.rbegin()); 
      while( 0 != gl ) {
        if ( debug ) std::cout << "     test " << gl->lvolumeName() 
                               << std::endl;
        IGeometryInfo* gl1 = gl->belongsTo( point1 ); 
        if( 0 == gl1 ) { 
          gl = 0 ; 
        } else {
          IGeometryInfo* gl2 = gl->belongsTo( point2 ); 
          if   ( gl1 == gl2 ) {
            std::cout << "   extended to " << gl2->lvolumeName() << std::endl;
            m_vGi.push_back( gl2 ); gl = gl2 ; 
          } else {
            gl = 0; 
          }  
        }
      }
    } 
    
    goodGI =gi;
    
    /// try to find a "good" geometry element in this tree 
    for( rGeoIt it = m_vGi.rbegin() ; m_vGi.rend() != it ; ++it ) { 
      if ( debug ) std::cout << "   test goodLocalGI " <<  (*it)->lvolumeName()
                             << std::endl;
      if ( goodLocalGI( point1 , point2 , *it ) ) {
        goodGI = (*it); 
        if ( debug ) std::cout << " geometry " << (*it)->lvolumeName() 
                               << std::endl;
        break;
      }
    } 
    if ( debug ) std::cout << " +++ exit with goodGI = " 
                           << goodGI->lvolumeName() 
                           << std::endl;
    
    ///  "good" element if found , or not found ;-))) 
  }
  catch( const GaudiException& Exception ) 
    {
      /// throw new exception:
      std::string message
        ("TransportSvc::findLocalGI(...), exception caught; Params: ");
      {
        const unsigned int buflen = 1024;
        char buffer[buflen] = {0,0}; 
        std::ostrstream ost( buffer , buflen ); 
        ost << "Point1=" << point1
            << "Point2=" << point2 ; 
        message += ost.str();             
        Assert( false , message , Exception );
      }
    }
  catch( ... ) 
    {
      /// throw new exception:
      std::string message
        ("TransportSvc::findLocalGI(...), unknown exception caught; Params: ");
      {
        const unsigned int buflen = 1024;
        char buffer[buflen] = {0,0}; 
        std::ostrstream ost( buffer , buflen ); 
        ost << "Point1=" << point1
            << "Point2=" << point2 ; 
        message += ost.str();             
        Assert( false , message );
      }      
    }
  
  ///
  return  goodGI; 

};

// ============================================================================
// The End 
// ============================================================================
#endif    //   __DETDESC_TRANSPORTSVC_TRANSPORTSVCFINDLOCALGI_H__
// ============================================================================

