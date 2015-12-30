// Include files

// STL
#include <cmath>

// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"
#include "TrackInterfaces/ITrackExtrapolator.h"

// local
#include "PVSeed3DOfflineTool.h"

using namespace SimplePVSpace;
using namespace ROOT::Math;
using namespace Gaudi;
using namespace LHCb;
using namespace std;


//-----------------------------------------------------------------------------
// Implementation file for class : PVSeed3DOfflineTool
//
// 2005-11-19 : Mariusz Witek
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( PVSeed3DOfflineTool )

namespace {
//=============================================================================
// closestPoints
//=============================================================================
bool closestPoints(const Gaudi::XYZPoint& ori1, const Gaudi::XYZVector& dir1,
                             const Gaudi::XYZPoint& ori2, const Gaudi::XYZVector& dir2,
                             Gaudi::XYZPoint& close1, Gaudi::XYZPoint& close2) {

  // Calculate the point between two tracks
  // (closest distance to both tracks)
  // code from Paul Bourke,
  // http://astronomy.swin.edu.au/~pbourke/geometry/lineline3d/

  double eps(1.e-6);

  close1 = Gaudi::XYZPoint(0.,0.,0.);
  close2 = Gaudi::XYZPoint(0.,0.,0.);

  Gaudi::XYZVector udir1 = dir1.unit();
  Gaudi::XYZVector udir2 = dir2.unit();

  Gaudi::XYZPoint t1b = ori1;
  Gaudi::XYZPoint t2b = ori2;

  Gaudi::XYZVector v0 = ori1 - ori2;
  Gaudi::XYZVector v1 = udir1;
  if (fabs(v1.x())  < eps && fabs(v1.y())  < eps && fabs(v1.z())  < eps)
    return false;
  Gaudi::XYZVector v2 = udir2;
  if (fabs(v2.x())  < eps && fabs(v2.y())  < eps && fabs(v2.z())  < eps)
    return false;

  double d01 = v0.Dot(v1);
  double d02 = v0.Dot(v2);
  double d11 = v1.Dot(v1);
  double d21 = v2.Dot(v1);
  double d22 = v2.Dot(v2);

  double denom = d11 * d22 - d21 * d21;
  if (fabs(denom) < eps) return false;
  double numer = d02 * d21 - d01 * d22;

  double mu1 = numer / denom;
  double mu2 = (d02 + d21 * mu1) / d22;

  close1 = t1b + mu1 * v1;
  close2 = t2b + mu2 * v2;

  return true;
}
//=============================================================================
// closestDistance
//=============================================================================
bool xPointParameters(const State& state1, const State& state2,
                      double & distance, double & distanceChi2, 
                      Gaudi::XYZPoint & closestPoint) {

  distance = 1.e10;
  Gaudi::XYZVector dis(0.,0.,1.e6);
  Gaudi::XYZPoint pos1(0.,0.,0.);
  Gaudi::XYZPoint pos2(0.,0.,0.);
  Gaudi::XYZVector dir1 = state1.slopes();
  Gaudi::XYZVector dir2 = state2.slopes();
  Gaudi::XYZPoint  ori1 = state1.position();
  Gaudi::XYZPoint  ori2 = state2.position();
  bool ok = closestPoints(ori1,dir1,ori2,dir2,pos1,pos2);
  if (!ok) return ok;
  closestPoint.SetXYZ(0.5*(pos1.x()+pos2.x()), 0.5*(pos1.y()+pos2.y()), 0.5*(pos1.z()+pos2.z()));
  distance = (pos1 - pos2).R();
  double errxy2 = state1.errX2() + state1.errY2() + state2.errX2() + state2.errY2();
  distanceChi2 = ( errxy2>0 ?distance*distance/errxy2  :1.e9 );
  return ok;;
}
//=============================================================================
// thetaTracks
//=============================================================================
double thetaTracks(const LHCb::State& state1,
                   const LHCb::State& state2) {
  auto dir1 = state1.slopes();
  auto dir2 = state2.slopes();
  return dir1.Dot(dir2)/std::sqrt(dir1.Mag2()*dir2.Mag2());
}
}
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PVSeed3DOfflineTool::PVSeed3DOfflineTool( const std::string& type,
                    const std::string& name,
                    const IInterface* parent )
  : base_class ( type, name , parent )
{
  declareInterface<IPVSeeding>(this);
  declareProperty("TrackPairMaxDistance", m_TrackPairMaxDistance = 5. * Gaudi::Units::mm );
  declareProperty("TrackPairMaxDistanceChi2", m_TrackPairMaxDistanceChi2 = 25. );
  declareProperty("MinCloseTracks",  m_MinCloseTracks  = 3);
  declareProperty("zMaxSpread",   m_zMaxSpread   = 1000. * Gaudi::Units::mm);
}
//=============================================================================
// Destructor
//=============================================================================
PVSeed3DOfflineTool::~PVSeed3DOfflineTool() = default;

StatusCode PVSeed3DOfflineTool::initialize() {
  // Full track extrapolator
  m_fullExtrapolator = tool<ITrackExtrapolator>("TrackMasterExtrapolator",this);
  if(!m_fullExtrapolator) {
    err() << "Unable to get TrackMasterExtrapolator" << endmsg;
    return  StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// getSeeds
//=============================================================================
std::vector<Gaudi::XYZPoint> 
PVSeed3DOfflineTool::getSeeds(const std::vector<const LHCb::Track*>& inputTracks,
				              const Gaudi::XYZPoint& beamspot) const {

  std::vector<Gaudi::XYZPoint> seeds;
  if(inputTracks.size() < 3 ) return seeds;


  if(msgLevel(MSG::DEBUG))  {
     debug() << " This is 3D PV seeding. Beam spot is ignored. BS: " << beamspot << endmsg;
  }

  std::vector<seedState> seed_states;
  seed_states.reserve(inputTracks.size());

  for (const auto& pvtr : inputTracks) {
    LHCb::State mstate =  pvtr->firstState();
    if ( !mstate.checkLocation(LHCb::State::ClosestToBeam) ) {
      // extrapolate
      if(fabs(mstate.qOverP()) > 0) {
        StatusCode sc = m_fullExtrapolator->propagate(mstate,0.0);
        if(!sc.isSuccess()) continue;
      }
    }
    seed_states.emplace_back();
    seed_states.back().used=0;
    seed_states.back().nclose=1; // count the track itself
    seed_states.back().dclose=0.0;
    seed_states.back().lbstate = mstate;
  }


  if(msgLevel(MSG::DEBUG))  {
     debug() << " seed_states.size  " << seed_states.size() << endmsg;
     debug() << " inputTracks.size  " << inputTracks.size() << endmsg;;
  }

  for(auto its1 = seed_states.begin(); its1 != seed_states.end()--; its1++) {
     for(auto its2 = its1+1; its2 != seed_states.end(); its2++) {

        Gaudi::XYZPoint closestPoint;
        double distance;
        double distanceChi2;
        bool ok = xPointParameters(its1->lbstate, its2->lbstate, distance, distanceChi2, closestPoint);
	if (ok && distance < m_TrackPairMaxDistance && distanceChi2 < m_TrackPairMaxDistanceChi2 ) {
          double err2xy = its1->lbstate.errX2() + its1->lbstate.errY2() + its2->lbstate.errX2() + its2->lbstate.errY2();
	  its1->nclose++;
	  its2->nclose++;
	  its1->dclose += 1./err2xy;
	  its2->dclose += 1./err2xy;
        }
     }
  }

  std::stable_sort(seed_states.begin(), seed_states.end(), 
                   [](const seedState& lhs, const seedState& rhs) 
                   { return lhs.dclose > rhs.dclose; } );

  if(msgLevel(MSG::DEBUG))  {
    debug() << " close nodes (pairs of tracks wrt one track): " << endmsg;
  }

  for(auto its1 = seed_states.begin(); its1 != seed_states.end(); its1++) {

    if(its1->nclose < m_MinCloseTracks) break;
    if ( its1->used > 0 ) continue;

    std::vector<closeNode> close_nodes;

    if(msgLevel(MSG::DEBUG)) {
      debug() << " close nodes - next track, nclose: " << its1->nclose << endmsg;
    }

    for(auto its2 = seed_states.begin(); its2 != seed_states.end(); its2++) {
      if ( its2->used > 0 || its1 == its2 ) continue;
       Gaudi::XYZPoint closest_point;
       double distance;
       double distanceChi2;
       //       const State lbst1 = its1->lbstate;
       //       const State lbst2 = its2->lbstate;
       bool ok = xPointParameters(its1->lbstate, its2->lbstate, distance, distanceChi2, closest_point);
       double costh = thetaTracks(its1->lbstate, its2->lbstate);
       //if (ok && distance < m_TrackPairMaxDistance && costh<0.999) 
       if (ok && distance < m_TrackPairMaxDistance && distanceChi2 < m_TrackPairMaxDistanceChi2 && costh<0.999) {
         closeNode closetr;
         closetr.take           = 1;
         closetr.seed_state    =  &(*its2);
         closetr.distance      = distance;
         closetr.closest_point = closest_point;
         close_nodes.push_back( closetr );

         if(msgLevel(MSG::DEBUG))  {
           double err_s1 = sqrt(its1->lbstate.errX2()+its1->lbstate.errY2());
           double err_s2 = sqrt(its2->lbstate.errX2()+its2->lbstate.errY2());
           debug() << format(" xyz %7.3f %7.3f %7.3f distance %7.3f %7.3f errors: %7.3f %7.3f ",
     		      closest_point.X(),  closest_point.Y(), closest_point.Z(),
                      distance, distanceChi2, err_s1, err_s2) << endmsg;
         } // end debug
       }
    }  // its2

    seedPoint mean_point;
    seedPoint mean_point_w;
    bool OK = simpleMean(close_nodes, mean_point);
    if ( OK ) {
      if(msgLevel(MSG::DEBUG)) {
        debug() << "simple mean OK " << endmsg;
      }
      its1->used=1; // base track
      int multi = 1;
      for (auto it = close_nodes.begin(); it != close_nodes.end(); it++ ) {
        if ( it->take > 0 ) {
          it->seed_state->used = 1;
          multi++;
        }
      }
      if ( multi < m_MinCloseTracks ) continue;
      mean_point_w = wMean(close_nodes, *its1);
      seeds.push_back(mean_point_w.position);

      if(msgLevel(MSG::DEBUG)) {

        debug() << " ===> xyz seed multi  "
                << mean_point_w.position.X()  << " "
                << mean_point_w.position.Y()  << " "
                << mean_point_w.position.Z()  << " | "
                << mean_point_w.error.X()  << " "
                << mean_point_w.error.Y()  << " "
                << mean_point_w.error.Z()  << " | "
                << mean_point_w.multiplicity  << " "
                << endmsg;
      }
    }
  } // its1
  return seeds;
}




//=============================================================================
// weighedMean
//=============================================================================
seedPoint PVSeed3DOfflineTool::wMean(const std::vector<closeNode> & close_nodes, 
                                     const seedState& base_state) const {
   seedPoint pseed;
   pseed.position.SetXYZ(0., 0., 0.);
   pseed.error.SetXYZ(0., 0., 0.);
   pseed.multiplicity = close_nodes.size();
   if ( close_nodes.size() < 2 ) return pseed;

   double sum_wx = 0.;
   double sum_wy = 0.;
   double sum_wz = 0.;
   double sum_wxx =0.;
   double sum_wyy =0.;
   double sum_wzz =0.;

   for (auto it = close_nodes.begin(); it != close_nodes.end(); it++ ) {
     if ( it->take == 0 ) continue;

     const State& state1 = base_state.lbstate;
     const State& state2 = it->seed_state->lbstate;

     //     double errxy2 = 0.1*0.1;
     double err1xy2 = state1.errX2() + state1.errY2();
     double err2xy2 = state2.errX2() + state2.errY2();
     double errxy2 = err1xy2;
     if ( err2xy2 > err1xy2 ) errxy2 = err2xy2;
     double costh = thetaTracks(state1, state2);
     double c2 = costh*costh;
     double ctanth2 = c2/(1.-c2);
     double errz2 = 2.*ctanth2*errxy2;

     double wx = 1./ errxy2;
     double wy = 1./ errxy2;
     double wz = 1./ errz2;
     sum_wx += wx;
     sum_wy += wy;
     sum_wz += wz;
     sum_wxx += wx*it->closest_point.X();
     sum_wyy += wy*it->closest_point.Y();
     sum_wzz += wz*it->closest_point.Z();
   }

   double x = sum_wxx/sum_wx;
   double y = sum_wyy/sum_wy;
   double z = sum_wzz/sum_wz;

   double ex = std::sqrt(1./sum_wx);
   double ey = std::sqrt(1./sum_wy);
   double ez = std::sqrt(1./sum_wz);

   pseed.position.SetXYZ(x,y,z);
   pseed.error.SetXYZ(ex,ey,ez);
   pseed.multiplicity = close_nodes.size();
   return pseed;
}

//=============================================================================
// simpleMean
//=============================================================================
bool PVSeed3DOfflineTool::simpleMean(std::vector<closeNode> & close_nodes, seedPoint & pseed) const {

   pseed.position.SetXYZ(0., 0., 0.);
   pseed.error.SetXYZ(0., 0., 0.);
   pseed.multiplicity = close_nodes.size();

   if(msgLevel(MSG::DEBUG)) {
     debug() << "close node size: " << close_nodes.size() << endmsg;
   }
   if ( close_nodes.size() < 2 ) return false;

   double x = 0.;
   double y = 0.;
   double z = 0.;

   double  spread2_max = m_zMaxSpread*m_zMaxSpread;

   Gaudi::XYZPoint pmean;
   std::vector<closeNode>::iterator it;
   std::vector<closeNode>::iterator itmax;

   double dist2_max = 1000.*1000.;
   int ngood = 1000;
   while (dist2_max>spread2_max && ngood>1) {
     int ng = 0;
     x = 0.;
     y = 0.;
     z = 0.;
     for ( it = close_nodes.begin(); it != close_nodes.end(); it++ ) {
        if ( it->take == 0 ) continue;
        ng++;
        x += it->closest_point.X();
        y += it->closest_point.Y();
        z += it->closest_point.Z();
     }
     if ( ng < m_MinCloseTracks ) return false;
     x /= ng;
     y /= ng;
     z /= ng;
     pmean.SetXYZ(x,y,z);

     double d2max=0.;
     for ( it = close_nodes.begin(); it != close_nodes.end(); it++ ) {
       if ( it->take == 0 ) continue;
       double dist2 = (pmean - it->closest_point).Mag2();
       if ( dist2>d2max ) {
         d2max = dist2;
         itmax = it;
       }
     }

     ngood = ng;
     if ( d2max > spread2_max ) {
       itmax->take = 0;
       ngood--;
       if(msgLevel(MSG::DEBUG)) {
         debug() << "spread too large => throwing out node" << endmsg;
       }
     }
     dist2_max = d2max;

   } // end while

   if ( ngood < m_MinCloseTracks ) return false;
   pseed.position = pmean;
   pseed.multiplicity = ngood;
   return true;
}

//=============================================================================
