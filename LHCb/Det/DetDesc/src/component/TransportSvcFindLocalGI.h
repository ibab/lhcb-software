// $Id: TransportSvcFindLocalGI.h,v 1.11 2005-12-02 18:36:56 jpalac Exp $
// ===========================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ===========================================================================
// $Log: not supported by cvs2svn $
// Revision 1.10  2003/01/17 14:03:40  sponce
// support for gcc 3.2
//
// Revision 1.9  2002/07/11 07:15:05  ibelyaev
//  fix the problems with virtual calls inside constructors
// 
// ===========================================================================
#ifndef DETDESC_TRANSPORTSVC_TRANSPORTSVCFINDLOCALGI_H
#define DETDESC_TRANSPORTSVC_TRANSPORTSVCFINDLOCALGI_H 1

#if defined (__GNUC__) && ( __GNUC__ <= 2 )
#include <strstream> 
#else
#include <sstream>
#endif

// local
#include "TransportSvc.h" 

/** @file
 *
 *  simple implementation of TransportSvc::findLocalGI method 
 *  @see TranpsortSvc 
 *  @see ITransportSvc 
 * 
 *  @author: Vanya Belyaev
 */

IGeometryInfo*  TransportSvc::findLocalGI ( const Gaudi::XYZPoint& point1 , 
                                            const Gaudi::XYZPoint& point2 , 
                                            IGeometryInfo*    gi     ,
                                            IGeometryInfo*    topGi  ) const  {
  if( 0 == gi ) { return 0 ; } 

  MsgStream log( msgSvc() , name() );

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
#if defined (__GNUC__) && ( __GNUC__ <= 2 )
        const unsigned int buflen = 1024;
        char buffer[buflen] = {0,0};
        std::ostrstream ost( buffer , buflen );
#else
        std::ostringstream ost;
#endif
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
#if defined (__GNUC__) && ( __GNUC__ <= 2 )
        const unsigned int buflen = 1024;
        char buffer[buflen] = {0,0}; 
        std::ostrstream ost( buffer , buflen ); 
#else
        std::ostringstream ost;
#endif
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

