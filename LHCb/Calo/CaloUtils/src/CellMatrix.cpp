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
CellMatrix::~CellMatrix() { m_det = 0 ; }

// ============================================================================
/** throw the exception 
 *  @exception CaloException 
 *  @param message exception message 
 */
// ============================================================================
void CellMatrix::Exception
( const std::string& message ) const 
{ throw CaloException( " CellMatrix:: " + message ); }

// ============================================================================
// The End 
// ============================================================================
