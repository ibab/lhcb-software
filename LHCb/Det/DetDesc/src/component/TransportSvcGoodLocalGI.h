// $Id: TransportSvcGoodLocalGI.h,v 1.4 2002-04-03 11:01:45 ibelyaev Exp $
// ============================================================================
// CVS Tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef       __DETDESC_TRANSPORTSVC_TRANSPORTSVCGOODLOCALGI_H__
#define       __DETDESC_TRANSPORTSVC_TRANSPORTSVCGOODLOCALGI_H__ 1

#include "TransportSvc.h"

/** @file TransportSvcGoogLocalGI.h
 *  
 * simple implementation of TransportSvc::goodLocalGI method 
 *
 * @attention mandatory that at least one point is INSIDE volume! 
 */
inline bool  TransportSvc::goodLocalGI
( const HepPoint3D& point1 , 
  const HepPoint3D& point2 , 
  IGeometryInfo*    gi     ) const  
{
  ///
  if( 0 == gi )                    { return false ; } 
  ///
  const ILVolume* lv = gi->lvolume(); 
  ///
  if( 0 == lv )                    { return false ; } 
  ///
  if( lv->isAssembly() )           { return false ; }
  
  /// 
  ISolid::Tick  tickMin = 0; 
  ISolid::Tick  tickMax = 1; 
  ///
  unsigned int nInt = 
    lv->solid()->intersectionTicks
    ( gi->matrix() * point1                         , 
      gi->matrix() * HepVector3D( point2 - point1 ) , 
      tickMin   ,     tickMax  ,  m_local_ticks     ) ; 
  /// 
  return ( 2 == nInt && tickMin == *(m_local_ticks.begin() ) 
           && tickMax == *(m_local_ticks.rbegin()) ) ? true : false ; 
  ///
};

// ============================================================================
// The End 
// ============================================================================
#endif        __DETDESC_TRANSPORTSVC_TRANSPORTSVCGOODLOCALGI_H__
// ============================================================================
