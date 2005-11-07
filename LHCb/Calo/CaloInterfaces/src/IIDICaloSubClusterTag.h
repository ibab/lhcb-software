// $Id: IIDICaloSubClusterTag.h,v 1.3 2005-11-07 12:08:25 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2005/01/25 14:12:18  cattanem
// updates for CLHEP 1.9
//
// Revision 1.1  2004/02/17 11:51:57  ibelyaev
//  move IID* from CaloInterfaces to src directory
//
// Revision 1.1  2002/04/02 11:08:13  ibelyaev
//  new interfaces
// 
// ============================================================================
#ifndef CALOINTERFACES_IIDICALOSUBCLUSTERTAG_H 
#define CALOINTERFACES_IIDICALOSUBCLUSTERTAG_H 1
// Include files
#include "GaudiKernel/IInterface.h"

/** @file IIDICaloSubClusterTag.h 
 *  
 *  The definition of unique interface identifier 
 *  for interface ICaloSubClusterTag
 *
 *  @see ICaloClusterTool
 *  @see ICaloSubClusterTag
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date   30/10/2001
 */

/** @var IID_ICaloSubClusterTag( "ICaloSubClusterTag" , 1 , 0 )
 *  
 *  The definition of unique interface identifier 
 *  for interface ICaloSubClusterTag( "ICaloSubClusterTag" , 1 , 0 )
 *
 *  @see ICaloClusterTool
 *  @see ICaloSubClusterTag
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date   30/10/2001
 */
static const InterfaceID IID_ICaloSubClusterTag( "ICaloSubClusterTag" , 1 , 0 );

// ============================================================================
// The End 
// ============================================================================
#endif // CALOINTERFACES_IIDICALOSUBCLUSTERTAG_H
// ============================================================================
