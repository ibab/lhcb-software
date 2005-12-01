// Include files 
// -------------
// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// from Event/TrackEvent
#include "Event/TrackParameters.h"

// from CLHEP
#include "CLHEP/Geometry/Point3D.h"

// local
#include "TrajPoca.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrajPoca
//
// 2005-11-23 : Edwin Bos, Jeroen van Tilburg, Eduardo Rodrigues
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<TrajPoca>          s_factory ;
const        IToolFactory& TrajPocaFactory = s_factory ; 

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrajPoca::TrajPoca( const std::string& type,
                    const std::string& name,
                    const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<ITrajPoca>(this);

  declareProperty( "Tolerance", m_tolerance = 20*micrometer );
  
}

//=============================================================================
// Destructor
//=============================================================================
TrajPoca::~TrajPoca() {};

//=============================================================================
//  Find the arclengths corresponding to a distance within tolerance
//=============================================================================
StatusCode TrajPoca::minimize( const Trajectory& traj1, const Trajectory& traj2,
                               double& arclength1, double& arclength2,
                               HepVector3D& distance )
{
  HepPoint3D  p1, p2;
  HepVector3D dp1, dp2, ddp1, ddp2;
  
  while( distance.mag() > m_tolerance ) {
    traj1.expansion( arclength1, p1, dp1, ddp1 );
    traj2.expansion( arclength2, p2, dp2, ddp2 );

    double a = (p1 - p2).dot(ddp1) + 1.;
    double b = dp2.dot(dp1);
    double c = - dp1.dot(dp2);
    double d = (p2 - p1).dot(ddp2) + 1.;
    double nom = a*d - b*c;

    if( fabs(nom) < TrackParameters::lowTolerance ) {
      return StatusCode::FAILURE;
    }

    double c1 = (p1 - p2).dot(dp1);
    double c2 = (p2 - p1).dot(dp2);

    double frac = 1. / nom;

    // This acts as if arclength along parabolas can be readily used
    // to increase arclengh of trajectories 
    arclength1 += - frac*d*c1 + frac*b*c2;
    arclength2 +=   frac*c*c1 - frac*a*c2;

    distance = HepVector3D(
                   p1 + dp1*arclength1 + 0.5*ddp1*arclength1*arclength1
                 - p2 + dp2*arclength2 + 0.5*ddp2*arclength2*arclength2 );
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode TrajPoca::initialize() {

  // Mandatory initialization of GaudiAlgorithm
  StatusCode sc = GaudiTool::initialize();
  if( sc.isFailure() ) { return sc; }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TrajPoca::finalize() {
  return GaudiTool::finalize();
};

//=============================================================================
