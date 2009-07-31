
//-----------------------------------------------------------------------------
/** @file STHistoAlgBase.cpp
 *
 *  Implementation file for class : STHistoToolBase
 *
 *  CVS Log :-
 *  $Id: STHistoToolBase.cpp,v 1.2 2009-07-31 14:58:10 mneedham Exp $
 *
 *  @author Matthew Needham    
 *  @date   2008-10-11
 */
//-----------------------------------------------------------------------------

// ============================================================================
// Disable warning on windows about using 'this' in constructors
#ifdef _WIN32
#pragma warning ( disable:4355 )
#endif
// ============================================================================


// local
#include "Kernel/STHistoToolBase.h"

// ============================================================================
// Force creation of templated class
#include "STCommonBase.icpp"
template class ST::CommonBase<GaudiHistoTool> ;
// ============================================================================
