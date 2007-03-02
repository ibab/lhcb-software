
//-----------------------------------------------------------------------------
/** @file IMCReconstructible.cpp
 *
 *  Implementation file for class : IMCReconstructible
 *
 *  CVS Log :-
 *  $Id: IMCReconstructible.cpp,v 1.2 2007-03-02 09:19:31 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date 28/02/2007
 */
//-----------------------------------------------------------------------------

// local
#include "Kernel/IMCReconstructible.h"

//-----------------------------------------------------------------------------

std::string IMCReconstructible::text( const RecCategory cat )
{
  switch ( cat )
  {
  case IMCReconstructible::NotReconstructible : return "Not Reconstructible";
  case IMCReconstructible::ChargedLong        : return "Charged Long Particle";
  case IMCReconstructible::ChargedDownstream  : return "Charged Downstream Particle";
  case IMCReconstructible::ChargedUpstream    : return "Charged Upstream Particle";
  case IMCReconstructible::ChargedTtrack      : return "Charged Ttrack Particle";
  case IMCReconstructible::ChargedVelo        : return "Charged VELO Particle";
  case IMCReconstructible::Neutral            : return "Neutral Particle";
  case IMCReconstructible::NoClassification   : return "No MC Classification";
  default                                     : return "UNKNOWN TYPE";
  }
}
