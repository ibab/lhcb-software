// $Id: TransportSvcFindLocalGI.h,v 1.2 2005-12-07 17:15:50 cattanem Exp $
// ===========================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ===========================================================================
#ifndef DETDESC_TRANSPORTSVC_TRANSPORTSVCFINDLOCALGI_H
#define DETDESC_TRANSPORTSVC_TRANSPORTSVCFINDLOCALGI_H 1

#include <sstream>

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

 
  /// output :-))
  IGeometryInfo* goodGI = 0  ; 
  
  try {  
    /// find the nearest "upper" volume, which contains the  first point  
    if ( !gi->isInside( point1 ) ) {
      return 0;
    }
    IGeometryInfo* gi1   = gi->belongsTo( point1, -1 ) ;   
  
    /// find the nearest "upper" volume, which contains the second point  
    IGeometryInfo* gi2   = gi1 ; 
    {  
      bool loc = false ; 
      for( loc = gi2->isInside( point2 ) ; !loc && 0 != gi2 ; 
           gi2 = gi2->parentIGeometryInfo() ) { 
        loc = gi2->isInside( point2 ); 
        if ( loc ) break;
      }  
      if( 0 == gi2 ) { 
        return 0; 
      }
    }
    
    // Here both points are located, gi2 is a parent of gi1. Get the first 
    // parent of gi2 which is a good GI.

    for( IGeometryInfo* gl = gi2 ; 0 != gl ; 
         gl = gl->parentIGeometryInfo() ) { 
      if ( 0 == gl ) return 0;
      if( goodLocalGI( point1 , point2 , gl ) ) { 
        return gl; 
      } 
      if ( topGi == gl ) return 0;
    }
    /// we have failed to find "good" element 
    return 0;                      /// RETURN !!! 

  }
  catch( const GaudiException& Exception ) 
    {
      /// throw new exception:
      std::string message
        ("TransportSvc::findLocalGI(...), exception caught; Params: ");
      {
        std::ostringstream ost;
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
        std::ostringstream ost;
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

