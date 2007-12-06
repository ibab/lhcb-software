#include "MaterialLocatorBase.h"
#include "Event/CubicStateVectorInterpolationTraj.h"
#include "Event/TrackParameters.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "DetDesc/Material.h"

MaterialLocatorBase::MaterialLocatorBase(const std::string& type,
					 const std::string& name,
					 const IInterface* parent ) 
  : GaudiTool(type, name, parent),
    m_maxNumIntervals( 20 ),
    m_maxDeviation( 5*Gaudi::Units::cm),
    m_scatteringtool("StateThickMSCorrectionTool"),
    m_dedxtool("StateSimpleBetheBlochEnergyCorrectionTool"),
    m_elecdedxtool("StateElectronEnergyCorrectionTool")
{
  declareInterface<IMaterialLocator>(this);
}

StatusCode MaterialLocatorBase::initialize()
{
  return 
    GaudiTool::initialize() &&
    m_scatteringtool.retrieve() && 
    m_dedxtool.retrieve() && 
    m_elecdedxtool.retrieve() ;
}

StatusCode MaterialLocatorBase::finalize()
{
  return 
    m_scatteringtool.release() && 
    m_dedxtool.release() && 
    m_elecdedxtool.release() && 
    GaudiTool::finalize() ;
}

size_t MaterialLocatorBase::intersect( const Gaudi::XYZPoint& p, const Gaudi::XYZVector& v, 
				       IMaterialLocator::Intersections& intersepts ) const 
  
{
  static ILVolume::Intersections origintersepts ;
  origintersepts.clear() ;
  size_t rc = intersect(p,v,origintersepts) ;
  double dz = v.z() ;
  double z1 = p.z() ;
  double tx = v.x()/v.z() ;
  double ty = v.y()/v.z() ;
  intersepts.resize(origintersepts.size()) ;
  int i(0) ;
  for( ILVolume::Intersections::const_iterator it = origintersepts.begin() ;
       it != origintersepts.end() ; ++it, ++i ) {
    intersepts[i].z1 = z1 + dz * it->first.first ;
    intersepts[i].z2 = z1 + dz * it->first.second ;
    intersepts[i].tx = tx ;
    intersepts[i].ty = ty ;
    intersepts[i].material  = it->second ;
  }
  return rc ;
}

static double midpointdeviation( const LHCb::ZTrajectory& traj, double z1, double z2)
{
  Gaudi::XYZPoint p1 = traj.position(z1) ;
  Gaudi::XYZPoint p2 = traj.position(z2) ;
  Gaudi::XYZPoint ptrue  = traj.position(0.5*(z1+z2)) ;
  Gaudi::XYZPoint pguess = p1 + 0.5*(p2-p1) ;
  return (ptrue-pguess).R() ;
}

size_t MaterialLocatorBase::intersect( const LHCb::ZTrajectory& traj,
				       IMaterialLocator::Intersections& intersepts ) const 
{
  intersepts.clear() ;
  if( std::abs(traj.endRange() - traj.beginRange())  > TrackParameters::propagationTolerance) {
    // The material locators can only use straight lines. So, we
    // approximate the trajectory by straight lines. We determine the
    // number of trajectories from the maximum allowed deviation.
    std::vector<double> zpositions ;
    const size_t maxnumnodes = m_maxNumIntervals+1 ;
    zpositions.reserve(maxnumnodes) ;
    zpositions.push_back(traj.beginRange()) ;
    zpositions.push_back(traj.endRange()) ;
    double deviation = midpointdeviation(traj, zpositions[0], zpositions[1]) ;
    size_t worstinterval(0) ;
    while( deviation > m_maxDeviation && zpositions.size() < maxnumnodes ) {
      // add a node
      zpositions.insert( zpositions.begin() + worstinterval+1, 
			 0.5*(zpositions[worstinterval] + zpositions[worstinterval+1]) ) ;
      if( zpositions.size() < maxnumnodes ) {
	// find the worst node. this could be done much faster if we
	// would 'cache' the deviations. since the trajectories are
	// TrackTraj's, that's actually not such a strange idea ...
	deviation = midpointdeviation(traj, zpositions[0], zpositions[1]) ;
	worstinterval=0 ;
	for(size_t interval=1; interval<zpositions.size()-1; ++interval) {
	  double thisdeviation = midpointdeviation(traj, zpositions[interval], zpositions[interval+1]) ;
	  if( deviation < thisdeviation) {
	    deviation = thisdeviation ;
	    worstinterval = interval ;
	  }
	}
      }
    }

    if( zpositions.size()==maxnumnodes) {
      warning() << "Trajectory approximation did not reach desired accuracy." << endreq ;
    } else if(msgLevel( MSG::DEBUG )) {
      debug() << "Trajectory approximation: numnodes=" << zpositions.size() 
	      << ", maxdeviation=" << deviation 
	      << " at z= " << 0.5*(zpositions[worstinterval]+zpositions[worstinterval+1]) 
	      << endreq ;
    }
    
    // Now create intersections for each of the intervals.
    static IMaterialLocator::Intersections tmpintersepts ;
    Gaudi::XYZPoint p1 = traj.position(zpositions[0]) ;
    for(size_t interval=0; interval<zpositions.size()-1; ++interval) {
      Gaudi::XYZPoint p2 = traj.position(zpositions[interval+1]) ;
      MaterialLocatorBase::intersect(p1,p2-p1,tmpintersepts) ;
      intersepts.insert(intersepts.end(),tmpintersepts.begin(),tmpintersepts.end()) ;
      p1 = p2 ;
    }
  }
  
  return intersepts.size() ;
} ;

size_t MaterialLocatorBase::intersect( const LHCb::StateVector& origin, 
				       const LHCb::StateVector& target,
				       IMaterialLocator::Intersections& intersepts ) const 
{
  intersepts.clear() ;
  if( std::abs(origin.z() - target.z()) > TrackParameters::propagationTolerance) {
    // create a trajectory for interpolation
    LHCb::CubicStateVectorInterpolationTraj traj(origin,target) ;
    intersect(traj,intersepts) ;
  }
  return intersepts.size() ;
} ;


void
MaterialLocatorBase::computeMaterialCorrection(Gaudi::TrackSymMatrix& noise,
                                               Gaudi::TrackVector& delta,
                                               const IMaterialLocator::Intersections& intersepts,
                                               double zorigin,
                                               double ztarget,
                                               double momentum,
                                               LHCb::ParticleID pid) const
{
  double qop = 1/momentum ;
  bool upstream = zorigin > ztarget ;
  // reset whatever comes in
  delta = Gaudi::TrackVector() ;
  noise = Gaudi::TrackSymMatrix() ;
  // loop over all intersections and do the work. note how we
  // explicitely keep the momentum constant. note that the way we
  // write this down, we rely on the fact that it is totally
  // irrelevant how the intersepts are sorted (because the propagation
  // is assumed to be linear.) 


  // the only thing that is tricky is dealing with the fact that z1
  // and z2 need not be in increasing value, nor intersept.z1 and
  // intersept.z2. that makes calculating the overlap ('thickness') a
  // bit difficult. that's why we just reorder them.
  double zmin(zorigin),zmax(ztarget) ;
  if(upstream) std::swap(zmin,zmax) ;
  const IStateCorrectionTool* dedxtool = 11 == pid.abspid() ? &(*m_elecdedxtool) : &(*m_dedxtool) ;
  
  //static Gaudi::TrackMatrix F = Gaudi::TrackMatrix( ROOT::Math::SMatrixIdentity() );
  for( Intersections::const_iterator it = intersepts.begin() ; it != intersepts.end(); ++it) {
    double z1 = std::max( zmin, std::min(it->z1,it->z2)) ;
    double z2 = std::min( zmax, std::max(it->z1,it->z2)) ;
    double thickness = z2 - z1 ; // negative thickness means no overlap
    if( thickness > TrackParameters::propagationTolerance ) {
      double thickness = z2-z1 ;
      
      // create a state. probably it is faster not to create it. but then we need to reset the noise every time.
      LHCb::State state ;
      state.setQOverP( qop ) ;
      state.setTx( it->tx ) ;
      state.setTy( it->ty ) ;
      
      // now add the wall
      m_scatteringtool->correctState( state, it->material, thickness, upstream );
      dedxtool->correctState( state, it->material, thickness, upstream );
      
      // extract the energy loss
      delta(4) += state.qOverP() - qop ;

      // propagate the noise to the target. linear propagation, only
      // non-zero contributions
      double dz = upstream ? ztarget - z1 : ztarget - z2 ; 
      state.covariance()(0,0) += 2*dz*state.covariance()(2,0) + dz*dz*state.covariance()(2,2) ;
      state.covariance()(2,0) += dz*state.covariance()(2,2) ;
      state.covariance()(1,1) += 2*dz*state.covariance()(3,1) + dz*dz*state.covariance()(3,3) ;
      state.covariance()(3,1) += dz*state.covariance()(3,3) ;
      noise +=  state.covariance() ;
    }
  }
}
