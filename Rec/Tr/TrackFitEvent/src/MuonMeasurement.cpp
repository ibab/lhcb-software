// $Id: MuonMeasurement.cpp,v 1.7 2009-04-06 13:52:11 hernando Exp $
// Include files

// local
#include "Event/MuonMeasurement.h"
#include "MuonDet/DeMuonDetector.h"
#include "Kernel/LineTraj.h"
#include "LHCbMath/LHCbMath.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : MuonMeasurement
//
// Author: Jose Hernando, Miriam Gandelman, Erica Polycarpo, Silvia Pozzi
// Created: 27-04-2007
//-----------------------------------------------------------------------------

/// Standard constructor, initializes variables
MuonMeasurement::MuonMeasurement( const LHCbID& lhcbID,
				  const DeMuonDetector& muondet,
                                  const Gaudi::XYZPoint& position,
                                  double dx, double dy,
                                  MuonMeasurement::MuonMeasurementType& XY)
  : Measurement(Measurement::Muon,lhcbID,0),
    m_position(position),  m_muonProjection(XY)
{
  LHCb::MuonTileID muid = lhcbID.muonID();
  // they have promised to fix the const
  const DeMuonChamber* chamber = muondet.getChmbPtr(muid.station(),muid.region(),
						    const_cast<DeMuonDetector&>(muondet).Tile2Chamber(muid).front()->chamberNumber()) ;
  m_detectorElement = chamber ;
  setZ(position.z());
  if(m_muonProjection == X){
    setMeasure(position.x());
    setErrMeasure(2.*dx*LHCb::Math::inv_sqrt_12);
    Gaudi::XYZVector y_dir(0,1,0);
    std::pair<double,double> y_range(-dy,dy);
    m_trajectory.reset( new LineTraj(position, y_dir, y_range, true) );

  }else{
    setMeasure(position.y());
    setErrMeasure(2.*dy*LHCb::Math::inv_sqrt_12);
    Gaudi::XYZVector x_dir(1,0,0);
    std::pair<double,double> x_range(-dx,dx);
    m_trajectory.reset( new LineTraj(position, x_dir, x_range, true) );

  }
}

/// Copy constructor
MuonMeasurement::MuonMeasurement( const MuonMeasurement& other )
  : Measurement(other),m_position(other.m_position),m_muonProjection(other.m_muonProjection) {

}
