// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Event/PhysEvent/src/AxPartCandidate.cpp,v 1.1 2002-04-15 11:37:31 gcorti Exp $


// Include files
#include "Event/AxPartCandidate.h"
#include <math.h>


//------------------------------------------------------------------------------
//
// AxPartCandidate Class
// Implement not accessors methods
//
// Author:      G.Corti
//
//------------------------------------------------------------------------------


/// ThreeMomentum
Hep3Vector AxPartCandidate::threeMomentum () {
  // calculate the Momentum vector (Px,Py,Pz) from measured fit
  // parameters slopeX, slopeY and total Momentum
  double pz = this->fitMomentum() / 
            ( sqrt ( 1.0 + (this->fitSlopeX() * this->fitSlopeX()) + 
                           (this->fitSlopeY() * this->fitSlopeY()) ) );
  double px = this->fitSlopeX() * pz;
  double py = this->fitSlopeY() * pz;
  Hep3Vector p3(px,py,pz);
  return p3;
}


/// FourMomentum
HepLorentzVector AxPartCandidate::fourMomentum (double massValue) {
  // calculate the Momentum 4vector (Px,Py,Pz,E) from 3 Momentum vector
  // (Px,Py,Pz) and provided mass
  Hep3Vector p3 = this->threeMomentum();
  double energy = sqrt( massValue*massValue + p3.mag2() );
  HepLorentzVector p4( p3.x(), p3.y(), p3.z(), energy);
  return p4;
}
