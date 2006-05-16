// $Id: TrackFastParabolicExtrapolator.cpp,v 1.7 2006-05-16 07:49:21 cattanem Exp $

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

// Create the transport matrix dX/dX_0 for pState's
void TrackFastParabolicExtrapolator::updateTransportMatrix( const double dz, 
                                                            State& )
{
  m_F(0,2) = dz;
  m_F(0,4) = 0.5 * m_ax * eplus * c_light * dz * dz;
  m_F(1,3) = dz;
  m_F(2,4) = m_ax * eplus * c_light * dz;

  return;
}
