
//-----------------------------------------------------------------------------
/** @file RichTupleAlgBase.cpp
 *
 *  Implementation file for class : RichTupleAlgBase
 *
 *  CVS Log :-
 *  $Id: RichTupleAlgBase.cpp,v 1.3 2009-07-27 20:29:02 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2002-04-05
 */
//-----------------------------------------------------------------------------

// local
#include "RichKernel/RichTupleAlgBase.h"

// ============================================================================
// Force creation of templated class
#include "RichHistoBase.icpp"
template class Rich::CommonBase < GaudiTupleAlg > ;
template class Rich::HistoBase  < GaudiTupleAlg > ;
// ============================================================================
