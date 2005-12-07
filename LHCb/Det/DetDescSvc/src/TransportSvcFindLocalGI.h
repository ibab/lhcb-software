// $Id: TransportSvcFindLocalGI.h,v 1.1.1.1 2005-12-07 16:01:45 mneedham Exp $
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

IGeometryInfo*  TransportSvc::findLocalGI ( const HepPoint3D& point1 , 
                                            const HepPoint3D& point2 , 
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

