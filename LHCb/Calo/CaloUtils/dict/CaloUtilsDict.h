// $Id: CaloUtilsDict.h,v 1.2 2009-09-30 13:18:43 ibelyaev Exp $
// ============================================================================
// CVS Tag $Name: not supported by cvs2svn $ , version $Revision: 1.2 $
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
// ============================================================================
// Stefan Roiser's trick to allow compilation on Win32
// ============================================================================
#ifdef WIN32
#ifdef CONST
#undef CONST
#endif 
#endif
// ============================================================================
// The END 
// ============================================================================
#endif // CALOUTILS_CALOUTILSDICT_H
// ============================================================================
