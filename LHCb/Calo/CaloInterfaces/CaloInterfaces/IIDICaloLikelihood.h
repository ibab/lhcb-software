// $Id: IIDICaloLikelihood.h,v 1.1 2004-02-17 11:42:29 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/04/02 11:08:13  ibelyaev
//  new interfaces
//
// ============================================================================
#ifndef CALOINTERFACES_IIDICALOLIKELIHOOD_H 
#define CALOINTERFACES_IIDICALOLIKELIHOOD_H 1
// Include files
// GaudiKernel
#include "GaudiKernel/IInterface.h"

/** @file IIDICaloLikelihood.h 
 *  
 *  Definition of the uniqie identifier for abstract interface ICaloLikelyhood
 *  @see IInterface
 *  @see IInterfaceID
 *  @see ICaloLikelyhood 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date   31/03/2002
 */

/** @var IID_ICaloLikelihood  IIDICaloLikelihood.h 
 *  
 *  Definition of the uniqie identifier for abstract interface ICaloLikelihood
 *  @see IInterface
 *  @see IInterfaceID
 *  @see ICaloLikelyhood 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date   31/03/2002
 */
static const InterfaceID IID_ICaloLikelihood( "ICaloLikelihood" , 1 , 0 );

// ============================================================================
// The End 
// ============================================================================
#endif // CALOINTERFACES_IIDICALOLIKELIHOOD_H
// ============================================================================
