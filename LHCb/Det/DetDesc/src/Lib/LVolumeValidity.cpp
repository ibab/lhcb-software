#ifndef     __DETDESC_VOLUMES_LVOLUME_VALIDITY_H__
#define     __DETDESC_VOLUMES_LVOLUME_VALIDITY_H__ 1 


#include "GaudiKernel/TimePoint.h"
#include "DetDesc/LVolume.h"


///
/// implementationf of "IValidity" interface for LVolume 
///
/// Author Vanya Belyaev 
///


/// functions from IValidity
///
const ITime&  LVolume::validSince ()
{
  if ( 0 != m_lv_validSince ){  setValiditySince( time_absolutepast ); }
  return *m_lv_validSince; 
};
///
const ITime&  LVolume::validTill  () 
{
  if ( 0 != m_lv_validTill ) { setValidityTill( time_absolutefuture ); }
  return *m_lv_validTill; 
};
///
void        LVolume::setValidity       ( const ITime& Since , 
                                         const ITime& Till )
{
  setValiditySince( Since );
  setValidityTill ( Till  );
};
///
void         LVolume::setValiditySince  ( const ITime& Since ) 
{
  if( 0 != m_lv_validSince ) { delete m_lv_validSince; m_lv_validSince = 0 ;} 
  m_lv_validSince = new  TimePoint( Since );   
};
///
void         LVolume::setValidityTill  ( const ITime& Till ) 
{
  if( 0 != m_lv_validTill ){ delete m_lv_validTill; m_lv_validTill = 0 ;} 
  m_lv_validTill = new  TimePoint( Till );   
};
///
StatusCode   LVolume::updateValidity    ()
{
  validSince();
  validTill ();
  //
  //  not yet implemented in a proper way 
  //
  return StatusCode::SUCCESS;
};
///

#endif  //  __DETDESC_VOLUMES_LVOLUME_VALIDITY_H__



