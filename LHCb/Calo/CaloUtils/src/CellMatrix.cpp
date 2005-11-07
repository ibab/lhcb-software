// $Id: CellMatrix.cpp,v 1.4 2005-11-07 11:57:13 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2001/11/25 15:26:20  ibelyaev
//  update for newer CaloKernel package
//
// Revision 1.2  2001/11/22 16:02:34  ibelyaev
//  new utilities
//
// Revision 1.1  2001/11/07 17:24:49  ibelyaev
//  new utilities for determination of relative positions of cells
// 
// ============================================================================
// Include files
// CaloKernel
#include "CaloKernel/CaloException.h"
// local
#include "CaloUtils/CellMatrix.h"

// ============================================================================
/** @file CellMatrix.cpp
 *
 *  Implementation file for class : CellMatrix
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 07/11/2001 
 */
// ============================================================================

// ============================================================================
/** destructor 
 */
// ============================================================================
CellMatrix::~CellMatrix() { m_det = 0 ; };

// ============================================================================
/** throw the exception 
 *  @exception CaloException 
 *  @param message exception message 
 */
// ============================================================================
void CellMatrix::Exception
( const std::string& message ) const 
{ throw CaloException( " CellMatrix:: " + message ); };

// ============================================================================
// The End 
// ============================================================================
