// $Id: TransportSvcFindLocalGI.h,v 1.3 2002-04-03 11:01:45 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
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

IGeometryInfo*  TransportSvc::findLocalGI
( const HepPoint3D& point1 , 
  const HepPoint3D& point2 , 
  IGeometryInfo*    gi     ) const  
{
  ///
  if( 0 == gi ) { return 0 ; } 
  
  /// output :-))
  IGeometryInfo* goodGI = 0  ; 
  
  try {  
    /// find the nearest "upper" volume, which contains the  first point  
    IGeometryInfo* gi1   = gi ;   
    {  
      bool loc = false ;  
      for( loc = gi1->isInside( point1 ) ; 
           !loc && 0 != gi1 ; gi1 = gi1->parentIGeometryInfo() ) 
        { 
          std::cout << " inside loop" <<  
            (loc = gi1->isInside( point1 ) ) 
                    << " pointr=" << gi1 << std::endl;  }  
      ///
      if( !loc || 0 == gi1 ) { return 0; }   
      ///
    }
  
    /// find the nearest "upper" volume, which contains the second point  
    IGeometryInfo* gi2   = gi ; 
    {  
      bool loc = false ; 
      for( loc = gi1->isInside( point2 ) ; 
           !loc && 0 != gi2 ; gi2 = gi2->parentIGeometryInfo() ) 
        { loc = gi1->isInside( point2 ); }  
      ///
      if( !loc || 0 == gi2 ) { return 0; } /// RETURN !!! 
      ///
    }
    
    /// here both points are located 
    ///  try fo find the "common" location 

    /// if both points are located in the same geometry element
    if( gi1 == gi2 )  
      {  
        ///  try to find a "good" geometry element in geometry hierarhy
        for( IGeometryInfo* gl = gi1 ; 0 != gl ; 
             gl = gl->parentIGeometryInfo() ) 
          { if( goodLocalGI( point1 , point2 , gl ) ) 
            { return gl; } }                                /// RETURN !!!    
        /// we have failed to find "good" element 
        return 0;                      /// RETURN !!! 
      }
    
    
    /// points are located in different geometry elements, 
    /// or geometry element is not good!
    
    /// "top" geometry tree for the first point 
    IGeometryInfo* vgi;

    m_vGi1.clear();   
    for( vgi = gi1 ; 0 != vgi ; 
         vgi = vgi->parentIGeometryInfo()  ) { m_vGi1.push_back( vgi ); }
    
    /// "top" geometry tree for the second point 
    m_vGi2.clear(); 
    for( vgi = gi2 ; 0 != vgi ; 
         vgi = vgi->parentIGeometryInfo()  ) { m_vGi2.push_back( vgi ); }  
    
    /// "common" top tree for both points 
    m_vGi.clear(); 
    {
      rGeoIt it1 = m_vGi1.rbegin(); 
      rGeoIt it2 = m_vGi2.rbegin(); 
      while( ( m_vGi1.rend() !=  it1 ) && 
             ( m_vGi2.rend() !=  it2 ) && 
             (      0        != *it1 ) && 
             (      0        != *it2 ) &&  
             (   *it1        == *it2 )    ) 
        { m_vGi.push_back( *it1 ) ; ++it1 ; ++it2 ; }
    }
    
    /// no common elements found !!!
    if( m_vGi.empty() ) { return 0 ; } 

    /// common "top" tree is found! 
    {
      /// extend it to lower level!   ???? DO WE NEED IT ????  YES!!! 
      IGeometryInfo*  gl = *(m_vGi.rbegin()); 
      while( 0 != gl ) 
        {
          IGeometryInfo* gl1 = gl->belongsTo( point1 ); 
          if( 0 == gl1 ) { gl = 0 ; }  
          else 
            {
              IGeometryInfo* gl2 = gl->belongsTo( point2 ); 
              if   ( gl1 == gl2 ) { m_vGi.push_back( gl2 ); gl = gl2 ; } 
              else                {                         gl = 0   ; }  
            }
        }
    } 
    
    /// try to find a "good" geometry element in this tree 
    for( rGeoIt it = m_vGi.rbegin() ; 
         m_vGi.rend() != it && 0 == goodGI ; ++it ) 
      { goodGI = goodLocalGI( point1 , point1 , *it ) ? (*it) : 0 ; } 
    
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

