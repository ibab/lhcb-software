// $Id: TransportSvcGoodLocalGI.h,v 1.5 2002-06-03 09:52:36 ocallot Exp $
// ============================================================================
// CVS Tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2002/04/03 11:01:45  ibelyaev
//  fix the problems with Assemblies for TransportSvc
//
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
  bool debug = false;
  
  ///
  if( 0 == gi )                    { 
    if ( debug ) std::cout << " goodLocalGI : no gi" << std::endl;
    return false ; } 
  ///
  const ILVolume* lv = gi->lvolume(); 
  ///
  if( 0 == lv )                    { 
    if ( debug ) std::cout << " goodLocalGI : gi has no lv" << std::endl;
    return false ; } 
  ///
  if( lv->isAssembly() )           { 
    if ( debug ) std::cout << " goodLocalGI : lv " << lv->name() 
                           << " is Assembly " << std::endl;
    return false ; 
  }
  
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
  if ( debug ) std::cout << " goodLocalGI : nInt " << nInt << std::endl;
  return ( 2 == nInt && tickMin == *(m_local_ticks.begin() ) 
           && tickMax == *(m_local_ticks.rbegin()) ) ? true : false ; 
  ///
};

// ============================================================================
// The End 
// ============================================================================
#endif        __DETDESC_TRANSPORTSVC_TRANSPORTSVCGOODLOCALGI_H__
// ============================================================================
