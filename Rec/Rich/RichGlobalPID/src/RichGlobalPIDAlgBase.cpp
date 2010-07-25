
//-----------------------------------------------------------------------------
/** @file RichGlobalPIDAlgBase.cpp
 *
 *  Implementation file for RICH Global PID algorithm base class : Rich::Rec::GlobalPID::AlgBase
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-05-10
 */
//-----------------------------------------------------------------------------

// local
#include "RichGlobalPIDAlgBase.h"

//=============================================================================
// Force creation of templated class
#include "RichGlobalPIDCommonBase.icpp"
template class Rich::Rec::GlobalPID::CommonBase<Rich::Rec::HistoAlgBase>;
//=============================================================================

//-----------------------------------------------------------------------------

StatusCode Rich::Rec::GlobalPID::AlgBase::sysExecute()
{
  this->deleteGPIDEvent();
  return Rich::Rec::HistoAlgBase::sysExecute();
}
