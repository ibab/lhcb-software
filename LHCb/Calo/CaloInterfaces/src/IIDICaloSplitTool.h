// $Id: IIDICaloSplitTool.h,v 1.2 2005-11-07 12:08:25 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2004/02/17 11:51:57  ibelyaev
//  move IID* from CaloInterfaces to src directory
//
// Revision 1.2  2002/04/02 11:08:13  ibelyaev
//  new interfaces
//
// Revision 1.1  2002/03/19 17:31:27  ibelyaev
//  add new abstract interface for Cluster Splitting Tools
// 
// ============================================================================
#ifndef CALOINTERFACES_IIDICALOSPLITTOOL_H 
#define CALOINTERFACES_IIDICALOSPLITTOOL_H 1
// Include files
// GaudiKernel 
#include "GaudiKernel/IInterface.h"

/** @file IIDICaloSplitTool.h
 *  
 *  The definition of unique interface identifier 
 *  for interface ICaloSplitTool
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date   30/10/2001
 */
static const InterfaceID IID_ICaloSplitTool( "ICaloSplitTool" , 2 , 0 );

// ============================================================================
// The End 
// ============================================================================
#endif // CALOINTERFACES_IIDICALOSPLITTOOL_H
