// $Id: IIDICaloClusterTool.h,v 1.3 2005-11-07 12:08:25 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2005/01/25 14:12:18  cattanem
// updates for CLHEP 1.9
//
// Revision 1.1  2004/02/17 11:51:56  ibelyaev
//  move IID* from CaloInterfaces to src directory
//
// Revision 1.3  2002/04/02 11:08:13  ibelyaev
//  new interfaces
//
// Revision 1.2  2001/12/09 14:09:13  ibelyaev
//  update for newer version of Gaudi
//
// Revision 1.1.1.1  2001/11/01 13:17:37  ibelyaev
// New package: the first release
// 
// ============================================================================
#ifndef CALOINTERFACES_IIDICALOCLUSTERTOOL_H 
#define CALOINTERFACES_IIDICALOCLUSTERTOOL_H 1
// include 
// GaudiKernel 
#include "GaudiKernel/IInterface.h"

/** @file IIDICaloClusterTool.h
 *  
 *  The definition of unique interface identifier 
 *  for interface ICaloClusterTool
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date   30/10/2001
 */

/** @var IID_ICaloClusterTool( "ICaloClusterTool" , 3 , 0 )
 *  
 *  The definition of unique interface identifier 
 *  for interface ICaloClusterTool
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date   30/10/2001
 */
static const InterfaceID IID_ICaloClusterTool( "ICaloClusterTool" , 3 , 0 );

// ============================================================================
// The End 
// ============================================================================
#endif // CALOINTERFACES_IIDICALOCLUSTERTOOL_H
// ============================================================================
