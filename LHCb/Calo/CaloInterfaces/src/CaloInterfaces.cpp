// $Id: CaloInterfaces.cpp,v 1.4 2006-12-21 18:01:56 odescham Exp $
// ============================================================================
// CVS tag $Name:R
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2006/03/22 18:21:51  odescham
// Migration to new Event Model
//
// Revision 1.2  2004/02/17 11:51:56  ibelyaev
//  move IID* from CaloInterfaces to src directory
//
// Revision 1.1  2004/02/17 11:42:29  ibelyaev
//  fix in Likel(i,y)hoods
// 
// ============================================================================
// CaloInterfaces 
// ============================================================================
#include "CaloInterfaces/ICaloElectron.h"
#include "CaloInterfaces/ICaloClusterSelector.h"
#include "CaloInterfaces/ICaloClusterTool.h"
#include "CaloInterfaces/ICaloCorrection.h"
#include "CaloInterfaces/ICaloDigitTool.h"
#include "CaloInterfaces/ICaloHypoLikelihood.h"
#include "CaloInterfaces/ICaloHypoTool.h"
#include "CaloInterfaces/ICaloLikelihood.h"
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
//ICaloElectron              :: ~ICaloElectron              () {} ;
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
