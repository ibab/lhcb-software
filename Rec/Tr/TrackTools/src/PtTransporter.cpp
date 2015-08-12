// Include files 

#include "Event/State.h"
#include "Event/Track.h"
#include "GaudiKernel/SystemOfUnits.h"
#include <cmath>

// local
#include "PtTransporter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PtTransporter
//
// 2008-05-08 : Johannes Albrecht
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( PtTransporter )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PtTransporter::PtTransporter( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IPtTransporter>(this);
  declareInterface<ITrackFunctionTool>(this);
  declareProperty("zMagnet", m_zMagnet = 5300. * Gaudi::Units::mm);
}
//=============================================================================
// Destructor
//=============================================================================
PtTransporter::~PtTransporter() {} 

//=============================================================================

double PtTransporter::ptAtOrigin(double zref, double xref, double /* yref */,
                                 double tx, double ty, double p) const
{
  // assume B field conserves magnitude of p, and assume that py is
  // not altered; model effect of magnetic field using kick in center
  // of magnet plane
  const double xmag = xref + tx * (m_zMagnet - zref);
  const double r = std::sqrt(xmag * xmag + m_zMagnet * m_zMagnet);
  double py = p * ty / std::sqrt(1. + tx * tx + ty * ty);
  p *= xmag / r; p *= p;
  py *= m_zMagnet / r; py *= py;
  return std::sqrt(p + py);
}

double PtTransporter::ptAtOrigin(const LHCb::State& state) const
{
  // protect against division by zero
  if (std::abs(state.qOverP()) < 1e-42) return HUGE_VAL;
  return ptAtOrigin(state.z(), state.x(), state.y(), state.tx(), state.ty(),
            1. / std::abs(state.qOverP()));
}

double PtTransporter::function(const LHCb::Track& track)
{
  if( track.nStates() == 0 ) return 0.;
  LHCb::State state = track.closestState(7800.);
  return ptAtOrigin(state);
}
  
