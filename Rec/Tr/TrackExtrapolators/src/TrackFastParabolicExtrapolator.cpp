// $Id: TrackFastParabolicExtrapolator.cpp,v 1.11 2007-06-25 09:56:50 cattanem Exp $

// Include files
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/PhysicalConstants.h"
using namespace Gaudi::Units;

// local
#include "TrackFastParabolicExtrapolator.h"

DECLARE_TOOL_FACTORY( TrackFastParabolicExtrapolator );

/** @file TrackFastParabolicExtrapolator.cpp
 *
 *  A TrackFastParabolicExtrapolator is a TrackExtrapolator which does a 
 *  'parabolic' extrapolation of a State. 
 *  Fast means the transport matrix is calculated approximately.
 *  It doesn't take into account Multiple Scattering.
 *
 *  @author Matt Needham
 *  @date   22-04-2000
 */

using namespace LHCb;
using namespace Gaudi;

// TrackFastParabolicExtrapolator constructor.
TrackFastParabolicExtrapolator::TrackFastParabolicExtrapolator
(const std::string& type,const std::string& name,const IInterface* parent):
  TrackParabolicExtrapolator(type, name, parent)
{
  declareInterface<ITrackExtrapolator>( this );
}

// TrackFastParabolicExtrapolator destructor.
TrackFastParabolicExtrapolator::~TrackFastParabolicExtrapolator()
{
  
}

//=============================================================================
// Update the transport matrix
//=============================================================================
void TrackFastParabolicExtrapolator::updateTransportMatrix( const double dz, 
                                                            Gaudi::TrackVector&,
                                                            Gaudi::TrackMatrix& transMat )
{
  // Reset the transport matrix
  transMat = TrackMatrix( ROOT::Math::SMatrixIdentity() );

  transMat(0,2) = dz;
  transMat(0,4) = 0.5 * m_ax * eplus * c_light * dz * dz;
  transMat(1,3) = dz;
  transMat(2,4) = m_ax * eplus * c_light * dz;

  return;
}
