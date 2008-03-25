
//-----------------------------------------------------------------------------
/** @file RichGlobalPIDAlgBase.cpp
 *
 *  Implementation file for RICH Global PID algorithm base class : Rich::Rec::GlobalPID::AlgBase
 *
 *  CVS Log :-
 *  $Id: RichGlobalPIDAlgBase.cpp,v 1.13 2008-03-25 16:26:43 jonrob Exp $
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
template class Rich::Rec::GlobalPID::CommonBase<Rich::Rec::AlgBase>;
//=============================================================================

//-----------------------------------------------------------------------------

StatusCode Rich::Rec::GlobalPID::AlgBase::sysExecute()
{
  this->deleteGPIDEvent();
  return Rich::Rec::AlgBase::sysExecute();
}
