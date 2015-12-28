// $Id: THitExpectation.cpp,v 1.5 2010-04-07 21:51:06 wouter Exp $

// from GaudiKernel
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/IMagneticFieldSvc.h"

// Event
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/StateVector.h"
#include "Event/StateParameters.h"

// Tsa
#include "TsaKernel/Line.h"
#include "TsaKernel/Parabola.h"
#include "LHCbMath/GeomFun.h"

#include "TrackInterfaces/ITrackExtrapolator.h"

#include "THitExpectation.h"

//=============================================================================
// 
//=============================================================================
THitExpectation::THitExpectation(const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent):
  GaudiTool(type, name, parent)
{ 
  // constructor

  declareProperty("extrapolatorName", m_extrapolatorName = "TrackParabolicExtrapolator");
  declareInterface<IHitExpectation>(this);
}


//=============================================================================
// 
//=============================================================================
StatusCode THitExpectation::initialize()
{

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }


  m_extrapolator = tool<ITrackExtrapolator>(m_extrapolatorName);

  m_pIMF = svc<IMagneticFieldSvc>("MagneticFieldSvc", true);

  return StatusCode::SUCCESS;
}

Tf::Tsa::Parabola THitExpectation::xParabola(const LHCb::Track& aTrack, const double z) const{
  
  // find the closest state
  const LHCb::State& aState = aTrack.closestState(z);
  LHCb::StateVector stateVector(aState.position(),aState.slopes());
  m_extrapolator->propagate(stateVector,StateParameters::ZMidT); 

  // make a parabola from this...
  const double zS = aState.z();
  const double a = curvature(aState);
  const double b = aState.tx() -2*zS*a;  
  const double c = aState.x() - zS*(b + a*zS); 
  return Tf::Tsa::Parabola(a,b,c);
}

double THitExpectation::curvature(const LHCb::State& aState) const{

  Gaudi::XYZPoint P = aState.position();
  static Gaudi::XYZVector B;
  m_pIMF->fieldVector( P, B ).ignore();
  const double tx = aState.tx();
  const double ty = aState.ty();
  auto tmp = 1.0+std::pow(tx,2);
  const double nTx = sqrt(tmp);
  const double norm = sqrt(tmp+std::pow(ty,2));

  return -0.5*norm * nTx *Gaudi::Units::c_light* B.y() * aState.qOverP();
}

Tf::Tsa::Line THitExpectation::yLine(const LHCb::Track& aTrack, const double z)const{

  // find the closest state
  LHCb::State aState = aTrack.closestState(z);
  LHCb::StateVector stateVector(aState.position(),aState.slopes());
  m_extrapolator->propagate(stateVector,StateParameters::ZMidT); 

  const double m = aState.ty();
  const double c = aState.y() - m*aState.z();
  return Tf::Tsa::Line(m,c);
}
