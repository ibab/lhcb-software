// $Id: IIDIPhotonParams.h,v 1.3 2005-06-09 13:11:30 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2005/06/09 13:10:40  pkoppenb
// Joined with v3r3b1 branch
//
// Revision 1.1.6.1  2005/06/02 16:45:41  pkoppenb
// CheckOverlap - another implementation
//
// Revision 1.1  2005/01/06 10:30:43  pkoppenb
// *.h moved from DaVinciTools to Kernel
//
// Revision 1.2  2004/10/27 14:09:58  pkoppenb
// IPVLocator
//
// Revision 1.1.1.1  2004/08/24 06:59:45  pkoppenb
// DaVinci Kerel and Interfaces extracted from DaVinciTools
//
// Revision 1.5  2004/07/28 14:15:23  pkoppenb
// untag
//
// Revision 1.4  2004/05/11 16:01:24  pkoppenb
// DVAlgorithm.cpp
//
// Revision 1.3  2004/03/11 16:12:34  pkoppenb
// Restored DaVinciTools. Better to have two libraries here than a DaVinciKernel
//
// Revision 1.1  2003/01/22 16:43:21  ibelyaev
//  new tools for Photons
// 
// ============================================================================
#ifndef DAVINCIKERNEL_IIDIPHOTONPARAMS_H 
#define DAVINCIKERNEL_IIDIPHOTONPARAMS_H 1
// Include files
// GaudiKernel 
#include "GaudiKernel/IInterface.h"
/** @file
 *
 *  The definition of unique interface identifier 
 *  for class IPhotonParams
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2003-01-19
 */

/** @var IID_IPhotonParams
 *  
 *  The definition of unique interface identifier 
 *  for class IPhotonParams
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date   2003-01-19
 */
static const InterfaceID IID_IPhotonParams( "IPhotonParams" , 1 , 0 );

// ============================================================================
// The END 
// ============================================================================
#endif // DAVINCIKERNEL_IIDIPHOTONPARAMS_H
// ============================================================================
