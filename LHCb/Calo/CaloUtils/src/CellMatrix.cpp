// $Id: CellMatrix.cpp,v 1.2 2001-11-22 16:02:34 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2001/11/07 17:24:49  ibelyaev
//  new utilities for determination of relative positions of cells
// 
// ============================================================================
// Include files
#include "CaloGen/CaloException.h"
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
