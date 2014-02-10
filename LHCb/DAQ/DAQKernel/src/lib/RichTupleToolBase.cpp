
//-----------------------------------------------------------------------------
/** @file RichTupleToolBase.cpp
 *
 *  Implementation file for class : RichTupleToolBase
 *
 *  CVS Log :-
 *  $Id: RichTupleToolBase.cpp,v 1.1 2009-08-10 12:12:06 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2002-04-05
 */
//-----------------------------------------------------------------------------

// local
#include "RichKernel/RichTupleToolBase.h"

// ============================================================================
// Force creation of templated class
#include "RichHistoBase.icpp"
template class Rich::CommonBase < GaudiTupleTool > ;
template class Rich::HistoBase  < GaudiTupleTool > ;
// ============================================================================
