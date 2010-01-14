// $Id: CaloUtilsDict.h,v 1.4 2010-01-14 17:39:40 vegorych Exp $
// ============================================================================
// CVS Tag $Name: not supported by cvs2svn $ , version $Revision: 1.4 $
// ============================================================================
#ifndef CALOUTILS_CALOUTILSDICT_H 
#define CALOUTILS_CALOUTILSDICT_H 1
// ============================================================================
// Include files
// ============================================================================
// CaloUtils 
// ============================================================================
#include "CaloUtils/CaloMomentum.h"
#include "CaloUtils/CaloParticle.h"
#include "CaloUtils/CaloCellIDAsProperty.h"
#include "CaloUtils/CaloUtils.h"
#include "CaloUtils/ICaloElectron.h"
// ============================================================================
// Stefan Roiser's trick to allow compilation on Win32
// ============================================================================
#ifdef WIN32
#ifdef CONST
#undef CONST
#endif 
#endif
// ============================================================================
namespace
{
  // ==========================================================================
  struct _Instantiations 
  {
    LHCb::CaloCellID::Vector m_1 ;
    LHCb::CaloCellID::Set    m_2 ;
    LHCb::CaloDigit::Vector  m_3 ;
    LHCb::CaloDigit::Set     m_4 ;    
  } ;
  //===========================================================================
}
// ============================================================================
// The END 
// ============================================================================
#endif // CALOUTILS_CALOUTILSDICT_H
// ============================================================================
