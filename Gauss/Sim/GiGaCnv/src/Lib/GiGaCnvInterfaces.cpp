// $Id: GiGaCnvInterfaces.cpp,v 1.1 2004-02-20 19:27:27 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files
// ============================================================================
// GiGaCnv
// ============================================================================
#include "GiGaCnv/IGiGaCnvSvc.h"
#include "GiGaCnv/IGiGaGeomCnvSvc.h"
#include "GiGaCnv/IGiGaHitsCnvSvc.h"
#include "GiGaCnv/IGiGaKineCnvSvc.h"
// ============================================================================
// local 
// ============================================================================
#include "IIDIGiGaCnvSvc.h"
#include "IIDIGiGaGeomCnvSvc.h"
#include "IIDIGiGaHitsCnvSvc.h"
#include "IIDIGiGaKineCnvSvc.h"
// ============================================================================

/** @file 
 *  Implementation file for GiGaCnv interfaces 
 *  @date 2004-02-20 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 */

// ============================================================================
IGiGaCnvSvc       ::   ~IGiGaCnvSvc        ()  {}
IGiGaGeomCnvSvc   ::   ~IGiGaGeomCnvSvc    ()  {}
IGiGaHitsCnvSvc   ::   ~IGiGaHitsCnvSvc    ()  {}
IGiGaKineCnvSvc   ::   ~IGiGaKineCnvSvc    ()  {}
// ============================================================================

// ============================================================================
const InterfaceID&  IGiGaCnvSvc          :: interfaceID () 
{ return        IID_IGiGaCnvSvc          ; }
// ============================================================================
const InterfaceID&  IGiGaGeomCnvSvc      :: interfaceID () 
{ return        IID_IGiGaGeomCnvSvc      ; }
// ============================================================================
const InterfaceID&  IGiGaHitsCnvSvc      :: interfaceID () 
{ return        IID_IGiGaHitsCnvSvc      ; }
// ============================================================================
const InterfaceID&  IGiGaKineCnvSvc      :: interfaceID () 
{ return        IID_IGiGaKineCnvSvc      ; }
// ============================================================================



// ============================================================================
// The END 
// ============================================================================
