#include "MaterialLocatorBase.h"
#include "TrackKernel/CubicStateVectorInterpolationTraj.h"
#include "Event/TrackParameters.h"
#include "Event/StateParameters.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "DetDesc/Material.h"

MaterialLocatorBase::MaterialLocatorBase(const std::string& type,
					 const std::string& name,
					 const IInterface* parent )
  : GaudiTool(type, name, parent),
    m_maxNumIntervals( 20 ),
    m_maxDeviation( 5*Gaudi::Units::cm),
    m_maxDeviationAtRefstates( 2*Gaudi::Units::mm),
    m_maxDeviationAtVeloRefstates( 0.5*Gaudi::Units::mm),
    m_elecdedxtool("StateElectronEnergyCorrectionTool")
{
  declareInterface<IMaterialLocator>(this);
  declareProperty( "GeneralDedxToolName" ,       m_dedxtoolname="StateDetailedBetheBlochEnergyCorrectionTool");
  declareProperty( "StateMSCorrectionToolName" , m_msCorrectionToolName="StateThickMSCorrectionTool");
}

StatusCode MaterialLocatorBase::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if( sc.isFailure() ) return sc;

  m_dedxtool       = tool<IStateCorrectionTool>( m_dedxtoolname, "GeneralDedxTool", this);
  m_scatteringTool = tool<IStateCorrectionTool>( m_msCorrectionToolName, "StateMSCorrectionTool", this );

  return m_elecdedxtool.retrieve() ;
}

StatusCode MaterialLocatorBase::finalize()
{
  m_elecdedxtool.release().ignore() ;
  return GaudiTool::finalize() ;
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
  intersepts.clear();
  intersepts.reserve(origintersepts.size()) ;
  for( const auto& i : origintersepts ) {
    intersepts.push_back(  IMaterialLocator::Intersection{ z1 + dz * i.first.first,
                              z1 + dz * i.first.second,
                              tx, ty, i.second  } );
  }
  return rc ;
}

inline double pointerror(const LHCb::StateVector& begin,
                         const LHCb::StateVector& end,
                         const LHCb::StateVector& mid )
{
  double rc(0) ;
  double dz = end.z() - begin.z();
  if(fabs(dz)> TrackParameters::propagationTolerance) {
    double tx = (end.x() - begin.x())/dz ;
    double ty = (end.y() - begin.y())/dz ;
    dz = mid.z() - begin.z() ;
    double dx = begin.x() + dz*tx - mid.x() ;
    double dy = begin.y() + dz*ty - mid.y() ;
    rc = std::sqrt(dx*dx+dy*dy) ;
  }
  return rc ;
}

inline double linearerror(const LHCb::StateVector& origin,
                          const LHCb::StateVector& destination)
{
  // calculate deviation from a parabola
  double dz = destination.z() - origin.z() ;
  double dx = origin.x() + origin.tx()*dz - destination.x() ;
  double dy = origin.y() + origin.ty()*dz - destination.y() ;
  return 0.25*std::sqrt(dx*dx+dy*dy) ;
}


size_t MaterialLocatorBase::intersect( const LHCb::ZTrajectory& traj,
                                       IMaterialLocator::Intersections& intersepts ) const
{
  intersepts.clear() ;
  if( std::abs(traj.endRange() - traj.beginRange())  > TrackParameters::propagationTolerance) {
    // The material locators can only use straight lines, so we
    // approximate the trajectory by straight lines. The less
    // intervals the better. We determine the number of intervals from
    // the maximum allowed deviation. Because 'queuering' the
    // trajectory for a state is potentially expensive (e.g. for the
    // tracktraj), we also limit the number of calls to the trajectory
    // as much as possible. There are two simple ways of calculating
    // the error: Either we can queuery the trajectory halfway
    // ('midpointerror'), or we can estimate the halfway deviation
    // from extrapolating to the end ('linearerror'). The latter is
    // cheaper and more conservative; the former is more optimal
    // if tracks aren't very quadratic.

    // The first two ndoes are just the endpoints. We sort the in z to
    // make things easier.
    const size_t maxnumnodes = m_maxNumIntervals+1 ;
    std::list<LHCb::StateVector> nodes = {
                traj.stateVector(std::min(traj.beginRange(),traj.endRange())),
                traj.stateVector(std::max(traj.beginRange(),traj.endRange()))};
    std::list<LHCb::StateVector>::iterator inode(nodes.begin()), nextnode ;

    // reference states for this trajectory. may be empty.
    if( m_maxDeviationAtRefstates > 0) {
      const std::vector<LHCb::StateVector> refstates = traj.refStateVectors() ;
      // First insert nodes at the reference positions, if required
      if( !refstates.empty()) {
        while(  (nextnode = std::next(inode)) != nodes.end() && nodes.size() < maxnumnodes ) {
          auto worstref = refstates.end() ;
          double reldeviation(0) ;
          for(auto iref = refstates.begin() ; iref != refstates.end(); ++iref)
            if( inode->z() < iref->z() && iref->z() < nextnode->z() ) {
              double thisdeviation    = pointerror( *inode, *nextnode, *iref ) ;
              double thisreldeviation =  thisdeviation /
                ( iref->z() < StateParameters::ZEndVelo ?
                  m_maxDeviationAtVeloRefstates : m_maxDeviationAtRefstates ) ;
              if( thisreldeviation > reldeviation ) {
                reldeviation = thisreldeviation ;
                worstref = iref ;
              }
            }
          if( reldeviation > 1 && worstref!=refstates.end()) {
            nodes.insert(nextnode,*worstref) ;
          }
          else
            ++inode ;
        }
      }
    }

    // now the usual procedure
    inode = nodes.begin() ;
    double worstdeviation(0) ;
    auto worstnode = inode ;
    while(  (nextnode = std::next(inode)) != nodes.end() && nodes.size() < maxnumnodes ) {
      // make sure we are fine at the midpoint
      LHCb::StateVector midpoint = traj.stateVector( 0.5*(inode->z()+nextnode->z())) ;
      double deviation = pointerror( *inode, *nextnode, midpoint  ) ;
      if( deviation > m_maxDeviation ) {
        nodes.insert(nextnode,midpoint ) ;
      } else {
        if( deviation > worstdeviation ) {
          worstdeviation = deviation ;
          worstnode = inode ;
        }
        ++inode ;
      }
    }

    // issue a warning if we didn't make it
    if( nodes.size()==maxnumnodes )
      Warning("Trajectory approximation did not reach desired accuracy. ", StatusCode::SUCCESS,0).ignore();

    // debug output
    if(msgLevel( MSG::VERBOSE ) ||
       (msgLevel( MSG::DEBUG) && nodes.size()==maxnumnodes ) ) {
      debug() << "Trajectory approximation: numnodes=" << nodes.size()
              << ", deviation=" << worstdeviation
              << " at z= " << 0.5*(worstnode->z()+next(worstnode)->z())
              << endmsg ;
      for( inode = nodes.begin(); (nextnode = std::next(inode)) != nodes.end(); ++inode) {
        LHCb::StateVector midpoint = traj.stateVector( 0.5*(inode->z()+nextnode->z() ) ) ;
        if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
          debug() << "interval: "
                  << "(" << inode->z() << ", " << nextnode->z() << ")"
                  << " ---> midpoint deviation: "
                  << pointerror(*inode,*nextnode,midpoint) << endmsg ;
      }
    }

    // Now create intersections for each of the intervals.
    static IMaterialLocator::Intersections tmpintersepts ;
    Gaudi::XYZPoint p1 = nodes.front().position() ;
    for( inode = nodes.begin(); (nextnode = std::next(inode)) != nodes.end(); ++inode) {
      Gaudi::XYZPoint p2 = nextnode->position() ;
      try {
        MaterialLocatorBase::intersect(p1,p2-p1,tmpintersepts) ;
      }
      catch (GaudiException& exception) {
        error() << "propagating pos1, pos2: "
                << p1 << " " << p2 << " "
                << traj.beginPoint() << " " << traj.endPoint() << endmsg ;
        throw exception ;
      }
      intersepts.insert(intersepts.end(),tmpintersepts.begin(),tmpintersepts.end()) ;
      p1 = p2 ;
    }
  }

  return intersepts.size() ;
}

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
}

void
MaterialLocatorBase::applyMaterialCorrections(LHCb::State& stateAtTarget,
                                              const IMaterialLocator::Intersections& intersepts,
                                              double zorigin,
                                              LHCb::ParticleID pid,
                                              bool applyScatteringCorrection,
                                              bool applyEnergyLossCorrection) const
{
  double ztarget = stateAtTarget.z() ;
  bool upstream = zorigin > ztarget ;
  double qop = stateAtTarget.qOverP() ;

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

  //Gaudi::TrackMatrix F = ROOT::Math::SMatrixIdentity();
  for( auto isept : intersepts ) {
    double z1 = std::max( zmin, std::min(isept.z1,isept.z2)) ;
    double z2 = std::min( zmax, std::max(isept.z1,isept.z2)) ;
    double thickness = z2 - z1 ; // negative thickness means no overlap
    if( thickness > TrackParameters::propagationTolerance ) {
      double thickness = z2 - z1 ; // Why this? Was something else intended?

      // create a state. probably it is faster not to create it. but then we need to reset the noise every time.
      LHCb::State state ;
      state.setQOverP( qop ) ;
      state.setTx( isept.tx ) ;
      state.setTy( isept.ty ) ;

      // now add the wall
      if( applyScatteringCorrection ) {
        m_scatteringTool->correctState( state, isept.material, thickness, upstream, pid );
      }
      if( applyEnergyLossCorrection) {
        dedxtool->correctState( state, isept.material, thickness, upstream, pid );
      }

      // add the change in qOverP
      stateAtTarget.setQOverP( stateAtTarget.qOverP() + state.qOverP() - qop ) ;

      // propagate the noise to the target. linear propagation, only
      // non-zero contributions
      double dz = upstream ? ztarget - z1 : ztarget - z2 ;
      state.covariance()(0,0) += 2*dz*state.covariance()(2,0) + dz*dz*state.covariance()(2,2) ;
      state.covariance()(2,0) += dz*state.covariance()(2,2) ;
      state.covariance()(1,1) += 2*dz*state.covariance()(3,1) + dz*dz*state.covariance()(3,3) ;
      state.covariance()(3,1) += dz*state.covariance()(3,3) ;
      stateAtTarget.covariance() += state.covariance() ;
    }
  }
}

