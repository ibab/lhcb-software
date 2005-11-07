// $Id: IIDICaloTrackMatch.h,v 1.4 2005-11-07 12:08:25 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2005/01/25 14:12:18  cattanem
// updates for CLHEP 1.9
//
// Revision 1.2  2004/09/02 18:52:52  ibelyaev
//  make preparation for HLT/Trigger
//
// Revision 1.1  2004/02/17 11:51:57  ibelyaev
//  move IID* from CaloInterfaces to src directory
//
// Revision 1.4  2002/05/23 09:41:37  ibelyaev
//  add 'state' method to ICaloTrackMatch interface
//
// ============================================================================
#ifndef CALOINTERFACES_IIDICALOTRACKMATCH_H 
#define CALOINTERFACES_IIDICALOTRACKMATCH_H 1
// include 
// GaudiKernel 
#include "GaudiKernel/IInterface.h"

/** @file IIDICaloTrackMatch.h
 *  
 *  The definition of unique interface identifier 
 *  for interface ICaloTrackMatch
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @author Dima  Rusinov Dmitri.Roussinov@cern.ch
 *  @date   30/10/2001
 */

/** @var IID_ICaloTrackMatch( "ICaloTrackMatch" , 4 , 0 )
 *  
 *  The definition of unique interface identifier 
 *  for interface ICaloTrackMatch
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @author Dima  Rusinov Dmitri.Roussinov@cern.ch
 *  @date   30/10/2001
 */
static const InterfaceID IID_ICaloTrackMatch( "ICaloTrackMatch" , 4 , 0 );

// ============================================================================
// The End 
// ============================================================================
#endif // CALOINTERFACES_IIDICALOTRACKMATCH_H
// ============================================================================
