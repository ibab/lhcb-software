/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $ 
/// ===========================================================================
/// Gaudi Kernel
#include "GaudiKernel/TimePoint.h"
/// DetDesc 
#include "DetDesc/LVolume.h"

/// ===========================================================================
/** @file LVolumeValidity.cpp
 * 
 * implementationf of "IValidity" interface for class LVolume 
 * 
 * @author Vanya Belyaev  Ivan.Belyaev@itep.ru  
 * @date xx/xx/xxxx
 */
/// ===========================================================================

bool LVolume::isValid ()                 
{ return validSince() <           validTill() ;};

bool LVolume::isValid ( const ITime& t ) 
{ return validSince() < t &&  t < validTill() ;};

const ITime&  LVolume::validSince ()
{
  if ( 0 != m_lv_validSince ){  setValiditySince( time_absolutepast ); }
  return *m_lv_validSince; 
};

const ITime&  LVolume::validTill  () 
{
  if ( 0 != m_lv_validTill ) { setValidityTill( time_absolutefuture ); }
  return *m_lv_validTill; 
};

void LVolume::setValidity ( const ITime& Since , 
                            const ITime& Till )
{
  setValiditySince( Since );
  setValidityTill ( Till  );
};

void LVolume::setValiditySince  ( const ITime& Since ) 
{
  if( 0 != m_lv_validSince ) { delete m_lv_validSince; m_lv_validSince = 0 ;} 
  m_lv_validSince = new  TimePoint( Since );   
};

void LVolume::setValidityTill  ( const ITime& Till ) 
{
  if( 0 != m_lv_validTill ){ delete m_lv_validTill; m_lv_validTill = 0 ;} 
  m_lv_validTill = new  TimePoint( Till );   
};


StatusCode LVolume::updateValidity    ()
{
  validSince();
  validTill ();
  //
  //  not yet implemented in a proper way 
  //
  return StatusCode::SUCCESS;
};



