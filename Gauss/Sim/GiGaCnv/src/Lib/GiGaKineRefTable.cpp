// $Id: GiGaKineRefTable.cpp,v 1.3 2002-03-12 15:14:08 ibelyaev Exp $
// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.2  2001/08/12 17:24:51  ibelyaev
/// improvements with Doxygen comments
///
/// Revision 1.1  2001/07/24 11:13:55  ibelyaev
/// package restructurization(III) and update for newer GiGa
/// 
// ============================================================================
/// GiGaCnv 
#include "GiGaCnv/GiGaKineRefTable.h"

// ============================================================================
/** @file GiGaKineRefTable.cpp 
 *  
 * Implementation file for class : GiGaKineRefTable
 * @warning the implementation is platform-dependent! 
 * 
 * 21/07/2001 : Vanya Belyaev Ivan.Belyaev@itep.ru
 */
// ============================================================================

// ============================================================================
/// creator
// ============================================================================
GiGaKineRefTable::GiGaKineRefTable () : m_table() {};

// ============================================================================
/// destructor 
// ============================================================================
GiGaKineRefTable::~GiGaKineRefTable() { m_table.clear(); };

// ============================================================================
// The End 
// ============================================================================
