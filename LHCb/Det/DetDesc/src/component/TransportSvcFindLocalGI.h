// $Id: TransportSvcFindLocalGI.h,v 1.7 2002-06-22 15:58:36 ocallot Exp $ 
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

  MsgStream log( DetDesc::msgSvc() , "TransportSvc" );

  /// output :-))
  IGeometryInfo* goodGI = 0  ; 
  
  try {  
    /// find the nearest "upper" volume, which contains the  first point  
    IGeometryInfo* gi1   = gi ;   
    log << MSG::VERBOSE << "Original geom " << gi1->lvolumeName() 
        << " point z = " << point1.z() << endreq;
    gi1   = gi1->belongsTo( point1, -1 ) ;   
    log << MSG::VERBOSE << "  belongsTo geom " << gi1->lvolumeName() 
        << endreq;
    {  
      bool loc = false ;  
      for( loc = gi1->isInside( point1 ) ; 
           !loc && (0 != gi1) && (gi != gi1) ; 
           gi1 = gi1->parentIGeometryInfo() ) {
        loc = gi1->isInside( point1 ); 
        log << MSG::VERBOSE << " inside loop " <<  loc 
            << " pointr=" << gi1->lvolumeName() << endreq;  
        if ( loc ) break;
      }  
      if( !loc || 0 == gi1 ) { 
        log << MSG::VERBOSE << " .. point1 not inside" << endreq;
        return 0; 
      }   
    }
    log << MSG::VERBOSE << "  p1 isInside geom " << gi1->lvolumeName() 
        << endreq;
  
    /// find the nearest "upper" volume, which contains the second point  
    IGeometryInfo* gi2   = gi1 ; 
    {  
      bool loc = false ; 
      for( loc = gi2->isInside( point2 ) ; !loc && 0 != gi2 ; 
           gi2 = gi2->parentIGeometryInfo() ) { 
        loc = gi2->isInside( point2 ); 
        log << MSG::VERBOSE << "  p2 : " << loc << " vol " 
            << gi2->lvolumeName() << endreq;
        if ( loc ) break;
      }  
      if( 0 == gi2 ) { 
        log << MSG::VERBOSE << " .. point2 not inside" << endreq;
        return 0; 
      }
    }
    log << MSG::VERBOSE << "  p2 isInside geom " << gi2->lvolumeName() 
        << endreq;
    
    /// here both points are located 
    ///  try fo find the "common" location 

    /// if both points are located in the same geometry element
    if( gi1 == gi2 ) {  
      ///  try to find a "good" geometry element in geometry hierarhy
      for( IGeometryInfo* gl = gi1 ; 0 != gl ; 
           gl = gl->parentIGeometryInfo() ) { 
        if ( 0 == gl ) return 0;
        if( goodLocalGI( point1 , point2 , gl ) ) { 
          log << MSG::VERBOSE << "  goodLocalGI geom " << gl->lvolumeName()
              << endreq;
          return gl; 
        } 
      }
      /// we have failed to find "good" element 
      log << MSG::VERBOSE << "  failed to find good element" << endreq;
      return 0;                      /// RETURN !!! 
    }
    
    
    /// points are located in different geometry elements, 
    /// or geometry element is not good!
    
    /// "top" geometry tree for the first point 
    IGeometryInfo* vgi;

    m_vGi1.clear();   
    for( vgi = gi1 ; (0 != vgi); vgi = vgi->parentIGeometryInfo() ) { 
      log << MSG::VERBOSE << "  store parent 1 " << vgi->lvolumeName() 
          << endreq;
      m_vGi1.push_back( vgi ); 
      if ( topGi == vgi ) break;
    }
    
    /// "top" geometry tree for the second point 
    m_vGi2.clear(); 
    for( vgi = gi2 ; (0 != vgi); vgi = vgi->parentIGeometryInfo()  ) { 
      log << MSG::VERBOSE << "  store parent 2 " << vgi->lvolumeName() 
          << endreq;
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
        log << MSG::VERBOSE << "  store common " << (*it1)->lvolumeName()
            << endreq;
        m_vGi.push_back( *it1 ) ; 
        ++it1 ; 
        ++it2 ; 
      }
    }
    
    /// no common elements found !!!
    if( m_vGi.empty() ) { 
      log << MSG::VERBOSE << " .. no common element" << endreq;
      return 0 ; 
    } 

    /// common "top" tree is found! 
    {
      log << MSG::VERBOSE << "  extend tree" << endreq;
      
      /// extend it to lower level!   ???? DO WE NEED IT ????  YES!!! 
      IGeometryInfo*  gl = *(m_vGi.rbegin()); 
      while( 0 != gl ) {
        log << MSG::VERBOSE << "     test " << gl->lvolumeName() 
            << endreq;
        IGeometryInfo* gl1 = gl->belongsTo( point1 ); 
        if( 0 == gl1 ) { 
          gl = 0 ; 
        } else {
          IGeometryInfo* gl2 = gl->belongsTo( point2 ); 
          if   ( gl1 == gl2 ) {
            log << MSG::VERBOSE << "   extended to " 
                << gl2->lvolumeName() << endreq;
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
      log << MSG::VERBOSE << "   test goodLocalGI " <<  (*it)->lvolumeName()
          << endreq;
      if ( goodLocalGI( point1 , point2 , *it ) ) {
        goodGI = (*it); 
        log << MSG::VERBOSE << " geometry " << (*it)->lvolumeName() 
                               << endreq;
        break;
      }
    } 
    log << MSG::VERBOSE << " +++ exit with goodGI = " 
        << goodGI->lvolumeName() << endreq;
    
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

