// $Id: CaloInterfaces.cpp,v 1.2 2004-02-17 11:51:56 ibelyaev Exp $
// ============================================================================
// CVS tag $Name:R
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2004/02/17 11:42:29  ibelyaev
//  fix in Likel(i,y)hoods
// 
// ============================================================================
// CaloInterfaces 
// ============================================================================
#include "CaloInterfaces/ICaloClusterSelector.h"
#include "CaloInterfaces/ICaloClusterTool.h"
#include "CaloInterfaces/ICaloCorrection.h"
#include "CaloInterfaces/ICaloDigitTool.h"
#include "CaloInterfaces/ICaloHypoLikelihood.h"
#include "CaloInterfaces/ICaloHypoTool.h"
#include "CaloInterfaces/ICaloLikelihood.h"
#include "CaloInterfaces/ICaloMomentumTool.h"
#include "CaloInterfaces/ICaloParticleTool.h"
#include "CaloInterfaces/ICaloPosition2MomentumTool.h"
#include "CaloInterfaces/ICaloSCorrectionFunction.h"
#include "CaloInterfaces/ICaloSplitTool.h"
#include "CaloInterfaces/ICaloSubClusterTag.h"
#include "CaloInterfaces/ICaloTrackIdEval.h"
#include "CaloInterfaces/ICaloTrackMatch.h"
// ============================================================================
// Local 
// ============================================================================
#include "IIDICaloClusterSelector.h"
#include "IIDICaloClusterTool.h"
#include "IIDICaloCorrection.h"
#include "IIDICaloDigitTool.h"
#include "IIDICaloHypoLikelihood.h"
#include "IIDICaloHypoTool.h"
#include "IIDICaloLikelihood.h"
#include "IIDICaloMomentumTool.h"
#include "IIDICaloParticleTool.h"
#include "IIDICaloPosition2MomentumTool.h"
#include "IIDICaloSplitTool.h"
#include "IIDICaloSubClusterTag.h"
#include "IIDICaloTrackIdEval.h"
#include "IIDICaloTrackMatch.h"
// ============================================================================

/** @file 
 * 
 *  Implementation file for package Calo/CaloInterfaces 
 * 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2004-02-15 
 */

// ============================================================================
ICaloClusterSelector       :: ~ICaloClusterSelector       () {} ;
// ============================================================================
ICaloClusterTool           :: ~ICaloClusterTool           () {} ;
// ============================================================================
ICaloCorrection            :: ~ICaloCorrection            () {} ;
// ============================================================================
ICaloDigitTool             :: ~ICaloDigitTool             () {} ;
// ============================================================================
ICaloHypoLikelihood        :: ~ICaloHypoLikelihood        () {} ;
// ============================================================================
ICaloHypoTool              :: ~ICaloHypoTool              () {} ;
// ============================================================================
ICaloLikelihood            :: ~ICaloLikelihood            () {} ;
// ============================================================================
ICaloMomentumTool          :: ~ICaloMomentumTool          () {} ;
// ============================================================================
ICaloParticleTool          :: ~ICaloParticleTool          () {} ;
// ============================================================================
ICaloPosition2MomentumTool :: ~ICaloPosition2MomentumTool () {} ;
// ============================================================================
ICaloSCorrectionFunction   :: ~ICaloSCorrectionFunction   () {} ;
// ============================================================================
ICaloSplitTool             :: ~ICaloSplitTool             () {} ;
// ============================================================================
ICaloSubClusterTag         :: ~ICaloSubClusterTag         () {} ;
// ============================================================================
ICaloTrackIdEval           :: ~ICaloTrackIdEval           () {} ;
// ============================================================================
ICaloTrackMatch            :: ~ICaloTrackMatch            () {} ;
// ============================================================================

// ============================================================================
const InterfaceID& 
ICaloClusterSelector       ::  interfaceID() 
{ return                   IID_ICaloClusterSelector       ; }
// ============================================================================
const InterfaceID& 
ICaloClusterTool           ::  interfaceID() 
{ return                   IID_ICaloClusterTool           ; }
// ============================================================================
const InterfaceID& 
ICaloCorrection            ::  interfaceID() 
{ return                   IID_ICaloCorrection            ; }
// ============================================================================
const InterfaceID& 
ICaloDigitTool             ::  interfaceID() 
{ return                   IID_ICaloDigitTool             ; }
// ============================================================================
const InterfaceID& 
ICaloHypoTool              ::  interfaceID() 
{ return                   IID_ICaloHypoTool              ; }
// ============================================================================
const InterfaceID& 
ICaloLikelihood            ::  interfaceID() 
{ return                   IID_ICaloLikelihood            ; }
// ============================================================================
const InterfaceID& 
ICaloHypoLikelihood        ::  interfaceID() 
{ return                   IID_ICaloHypoLikelihood        ; }
// ============================================================================
const InterfaceID& 
ICaloMomentumTool          ::  interfaceID() 
{ return                   IID_ICaloMomentumTool          ; }
// ============================================================================
const InterfaceID& 
ICaloParticleTool          ::  interfaceID() 
{ return                   IID_ICaloParticleTool          ; }
// ============================================================================
const InterfaceID& 
ICaloPosition2MomentumTool ::  interfaceID() 
{ return                   IID_ICaloPosition2MomentumTool ; }
// ============================================================================
const InterfaceID& 
ICaloSCorrectionFunction   ::  interfaceID() 
{ return                   IID_ICaloSCorrectionFunction   ; }
// ============================================================================
const InterfaceID& 
ICaloSubClusterTag         ::  interfaceID() 
{ return                   IID_ICaloSubClusterTag         ; }
// ============================================================================
const InterfaceID& 
ICaloSplitTool             ::  interfaceID() 
{ return                   IID_ICaloSplitTool             ; }
// ============================================================================
const InterfaceID& 
ICaloTrackIdEval           ::  interfaceID() 
{ return                   IID_ICaloTrackIdEval            ; }
// ============================================================================
const InterfaceID& 
ICaloTrackMatch            ::  interfaceID() 
{ return                   IID_ICaloTrackMatch             ; }
// ============================================================================

 
// ============================================================================
// The END 
// ============================================================================
