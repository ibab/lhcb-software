// $Id: TransportSvcFindLocalGI.h,v 1.8 2002-07-03 08:15:37 ocallot Exp $ 
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
    log << MSG::VERBOSE << "Original geom " << gi->lvolumeName() 
        << " point z = " << point1.z() << endreq;
    if ( !gi->isInside( point1 ) ) {
      log << MSG::VERBOSE << " point is not inside. Return 0 " << endreq;
      return 0;
    }
    IGeometryInfo* gi1   = gi->belongsTo( point1, -1 ) ;   
    log << MSG::VERBOSE << "  p1 " << point1.x() << " " 
        << point1.y() << " " << point1.z()
        << " isInside geom " << gi1->lvolumeName() 
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
    log << MSG::VERBOSE << "  p2 " << point2.x() << " " 
        << point2.y() << " " << point2.z()
        << " isInside geom " << gi2->lvolumeName() 
        << endreq;
    
    // Here both points are located, gi2 is a parent of gi1. Get the first 
    // parent of gi2 which is a good GI.

    for( IGeometryInfo* gl = gi2 ; 0 != gl ; 
         gl = gl->parentIGeometryInfo() ) { 
      if ( 0 == gl ) return 0;
      if( goodLocalGI( point1 , point2 , gl ) ) { 
        log << MSG::VERBOSE << "+++ Exit with goodLocalGI geom " 
            << gl->lvolumeName() << endreq;
        return gl; 
      } 
      if ( topGi == gl ) return 0;
    }
    /// we have failed to find "good" element 
    log << MSG::INFO << "--- failed to find good GI element" << endreq;
    return 0;                      /// RETURN !!! 

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

