// $Id: THitExpectation.cpp,v 1.2 2007-10-10 18:32:17 smenzeme Exp $

// from GaudiKernel
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/IMagneticFieldSvc.h"

// Event
#include "Event/Track.h"
#include "Event/State.h"

// Tsa
#include "TsaKernel/Line.h"
#include "TsaKernel/Parabola.h"
#include "LHCbMath/GeomFun.h"

#include "TrackInterfaces/ITrackExtrapolator.h"

#include "THitExpectation.h"

using namespace LHCb;
using namespace Gaudi;

//=============================================================================
// 
//=============================================================================
THitExpectation::THitExpectation(const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent):
  GaudiTool(type, name, parent)
{ 
  // constructer

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


  m_extrapolator = tool<ITrackExtrapolator>("TrackFastParabolicExtrapolator");

  m_pIMF = svc<IMagneticFieldSvc>("MagneticFieldSvc", true);

  return StatusCode::SUCCESS;
}

Tf::Tsa::Parabola THitExpectation::xParabola(const Track& aTrack, const double z) const{
  
  // find the closest state
  const State& aState = aTrack.closestState(z);

  //  double m_curvFactor = 42.0;

  // make a parabola from this...
  const double zS = aState.z();
  const double a = curvature(aState);
  //const double a = aState.qOverP()/m_curvFactor;
  const double b = aState.tx() -2*zS*a;  
  const double c = aState.x() - zS*(b + a*zS); 
  return Tf::Tsa::Parabola(a,b,c);
}

double THitExpectation::curvature(const State& aState) const{

  XYZPoint P = aState.position();
  static XYZVector B;
  m_pIMF->fieldVector( P, B ).ignore();
  const double tx = aState.tx();
  const double ty = aState.ty();
  const double nTx = sqrt(1.0+gsl_pow_2(tx));
  const double norm = sqrt(1.0+gsl_pow_2(tx)+gsl_pow_2(ty));

  double a = -0.5*norm * nTx *Gaudi::Units::c_light* B.y() * aState.qOverP();
  return a;
}

Tf::Tsa::Line THitExpectation::yLine(const LHCb::Track& aTrack, const double z)const{

  // find the closest state
  State aState = aTrack.closestState(z);

  const double m = aState.ty();
  const double c = aState.y() - m*aState.z();
  return Tf::Tsa::Line(m,c);
}
