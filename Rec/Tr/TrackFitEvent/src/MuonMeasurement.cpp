// $Id: MuonMeasurement.cpp,v 1.1 2007-04-27 20:38:51 polye Exp $
// Include files 

// local
#include "Event/MuonMeasurement.h"
#include "Kernel/LineTraj.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : MuonMeasurement
//
// Author: Jose Hernando, Miriam Gandelman, Erica Polycarpo
// Created: 27-04-2007
//-----------------------------------------------------------------------------

/// Standard constructor, initializes variables
MuonMeasurement::MuonMeasurement( const LHCbID& lhcbID,
                              const Gaudi::XYZPoint& position,
                              double dx, double dy ): 
  m_position(position), m_dx(dx), m_dy(dy) 
{
  setLhcbID(lhcbID);
  setZ(position.z());
  setType(Measurement::Muon);
  setMeasure(position.x());
  setErrMeasure(2.*dx/sqrt(12.));
  Gaudi::XYZVector x_dir(1,0,0); 
  Gaudi::XYZVector y_dir(0,1,0); 
  std::pair<double,double> x_range(position.x()-dx,position.x()+dx);
  std::pair<double,double> y_range(position.y()-dy,position.y()+dy);
  m_trajectory = std::auto_ptr<Trajectory>(new LineTraj(position, x_dir, x_range, true));
  m_trajectoryY = std::auto_ptr<Trajectory>(new LineTraj(position, y_dir, y_range, true));
}

/// Copy constructor
MuonMeasurement::MuonMeasurement( const MuonMeasurement& other ) 
  : Measurement(other) {

}
