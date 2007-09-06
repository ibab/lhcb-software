// $Id: StateVector.cpp,v 1.2 2007-09-06 11:44:07 mneedham Exp $

#include <math.h>
#include <gsl/gsl_math.h>

// from GaudiKernel
#include "GaudiKernel/GenericMatrixTypes.h"
#include "GaudiKernel/GenericVectorTypes.h"
#include "GaudiKernel/SymmetricMatrixTypes.h"

// local
#include "Event/TrackTypes.h"
#include "Event/StateVector.h"
#include "Event/TrackParameters.h"

using namespace LHCb;
using namespace Gaudi;

//-----------------------------------------------------------------------------
// Implementation file for class : StateVector
//
// 2004-12-14 : Jose Hernando, Eduardo Rodrigues
//-----------------------------------------------------------------------------

//=============================================================================
// Constructor from a position and direction
//=============================================================================
StateVector::StateVector(const Gaudi::XYZPoint& pos, 
			 const Gaudi::XYZVector& dir,
			 double qop)
  : m_parameters(pos.x(),pos.y(),fabs(dir.z()) > TrackParameters::lowTolerance  ? dir.x()/dir.z() :0.0,
		  fabs(dir.z()) > TrackParameters::lowTolerance ? dir.y()/dir.z() : 0,qop),
    m_z(pos.z()) {}

//=============================================================================
// Retrieve the momentum of the state
//=============================================================================
double StateVector::p() const
{
  return ( fabs(qOverP()) > TrackParameters::lowTolerance ? fabs( 1./qOverP()) : HUGE_VAL );
};

//=============================================================================
// Retrieve the transverse momentum of the state
//=============================================================================
double StateVector::pt() const
{
  double rc(HUGE_VAL) ;
  if ( fabs(m_parameters[4]) > TrackParameters::lowTolerance ) {
    const double txy2 =   m_parameters[2]*m_parameters[2]
      + m_parameters[3]*m_parameters[3];
    rc = sqrt( txy2/(1.+txy2) ) / fabs( m_parameters[4] );
  }
  return rc ;
};

//=============================================================================
