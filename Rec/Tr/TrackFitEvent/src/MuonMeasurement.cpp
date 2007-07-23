// $Id: MuonMeasurement.cpp,v 1.2 2007-07-23 11:12:35 spozzi Exp $
// Include files 

// local
#include "Event/MuonMeasurement.h"
#include "Kernel/LineTraj.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : MuonMeasurement
//
// Author: Jose Hernando, Miriam Gandelman, Erica Polycarpo, Silvia Pozzi
// Created: 27-04-2007
//-----------------------------------------------------------------------------

/// Standard constructor, initializes variables
MuonMeasurement::MuonMeasurement( const LHCbID& lhcbID,
                                  const Gaudi::XYZPoint& position,
                                  double dx, double dy,
                                  MuonMeasurement::MuonMeasurementType& XY): 
  m_position(position),  m_muonProjection(XY) 
{
  setLhcbID(lhcbID);
  setZ(position.z());
  setType(Measurement::Muon);
  if(m_muonProjection == Y){
    setMeasure(position.x());
    setErrMeasure(2.*dy/sqrt(12.));
    Gaudi::XYZVector x_dir(1,0,0); 
    std::pair<double,double> x_range(-dx,dx);
    m_trajectory = std::auto_ptr<Trajectory>(new LineTraj(position, x_dir, x_range, true));

  }else{
    setMeasure(position.y());
    setErrMeasure(2.*dx/sqrt(12.));
    Gaudi::XYZVector y_dir(0,1,0); 
    std::pair<double,double> y_range(-dy,dy);
    m_trajectory = std::auto_ptr<Trajectory>(new LineTraj(position, y_dir, y_range, true));  

  }
}

/// Copy constructor
MuonMeasurement::MuonMeasurement( const MuonMeasurement& other ) 
  : Measurement(other) {

}
