// Include files

// STL
#include <cmath>

// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"
#include "TrackInterfaces/ITrackExtrapolator.h"

// local
#include "PVSeed3DOfflineTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PVSeed3DOfflineTool
//
// 2005-11-19 : Mariusz Witek
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( PVSeed3DOfflineTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PVSeed3DOfflineTool::PVSeed3DOfflineTool( const std::string& type,
                    const std::string& name,
                    const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_fullExtrapolator(0)
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
PVSeed3DOfflineTool::~PVSeed3DOfflineTool() {}

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
void PVSeed3DOfflineTool::getSeeds(std::vector<const LHCb::Track*>& inputTracks, 
				   const Gaudi::XYZPoint& beamspot,
				   std::vector<Gaudi::XYZPoint>& seeds) {

  if(inputTracks.size() < 3 ) return;

  if(msgLevel(MSG::DEBUG))  {
     debug() << " This is 3D PV seeding. Beam spot is ignored. BS: " << beamspot << endmsg;
  }

  std::vector<seedState> seed_states;
  seed_states.reserve(inputTracks.size());

  std::vector<const LHCb::Track*>::iterator it;
  //  std::cout <<  "nr of tracks " << inputTracks.size() << std::endl;
  for ( it = inputTracks.begin(); it != inputTracks.end(); it++ ) {
//     const LHCb::Track* ptr = (*it);
    //    std::cout <<  "z of track " <<  ptr->firstState().z() << std::endl;
    //    std::cout << *ptr << std::endl;
  }

  for ( it = inputTracks.begin(); it != inputTracks.end(); it++ ) {
    const LHCb::Track* pvtr = (*it);

    LHCb::State mstate;
    mstate =  pvtr->firstState();
    if ( mstate.checkLocation(LHCb::State::ClosestToBeam) != true ) {
      // extrapolate
      if(fabs(mstate.qOverP()) > 0) {
        StatusCode sc = m_fullExtrapolator->propagate(mstate,0.0);
        if(!sc.isSuccess()) continue;
      }
    }
    seedState seedtr;
    seedtr.used=0;
    seedtr.nclose=1; // count the track itself
    seedtr.dclose=0.0;
    seedtr.lbstate = mstate;
    seed_states.push_back(seedtr);
  }


  if(msgLevel(MSG::DEBUG))  {
     debug() << " seed_states.size  " << seed_states.size() << endmsg;
     debug() << " inputTracks.size  " << inputTracks.size() << endmsg;;
  }

  std::vector<seedState>::iterator its1,its2;
  for(its1 = seed_states.begin(); its1 != seed_states.end()--; its1++) {
     for(its2 = its1+1; its2 != seed_states.end(); its2++) {

        EPoint closestPoint;
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

  std::stable_sort(seed_states.begin(), seed_states.end(), statecomp);

  if(msgLevel(MSG::DEBUG))  {
    debug() << " close nodes (pairs of tracks wrt one track): " << endmsg;
  }

  for(its1 = seed_states.begin(); its1 != seed_states.end(); its1++) {

    if(its1->nclose < m_MinCloseTracks) break;
    if ( its1->used > 0 ) continue;

    std::vector<closeNode> close_nodes;

    if(msgLevel(MSG::DEBUG)) {
      debug() << " close nodes - next track, nclose: " << its1->nclose << endmsg;
    }

    for(its2 = seed_states.begin(); its2 != seed_states.end(); its2++) {
      if ( its2->used > 0 || its1 == its2 ) continue;
       EPoint closest_point;
       double distance;
       double distanceChi2;
       //       const State lbst1 = its1->lbstate;
       //       const State lbst2 = its2->lbstate;
       bool ok = xPointParameters(its1->lbstate, its2->lbstate, distance, distanceChi2, closest_point);
       double costh = thetaTracks(its1->lbstate, its2->lbstate);
       //if (ok && distance < m_TrackPairMaxDistance && costh<0.999) {
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
      std::vector<closeNode>::iterator it;
      for ( it = close_nodes.begin(); it != close_nodes.end(); it++ ) {
        if ( it->take > 0 ) {
          it->seed_state->used = 1;
          multi++;
        }
      }
      if ( multi < m_MinCloseTracks ) continue;
      seedState* base_state = &(*its1);
      wMean(close_nodes, base_state, mean_point_w);
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



}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PVSeed3DOfflineTool::finalize() {

  debug() << "==> Finalize" << endmsg;

  return StatusCode::SUCCESS;

}
//=============================================================================
// closestPoints
//=============================================================================
bool PVSeed3DOfflineTool::closestPoints(const EPoint& ori1, const EVector& dir1,
                             const EPoint& ori2, const EVector& dir2,
                             EPoint& close1, EPoint& close2) {

  // Calculate the point between two tracks
  // (closest distance to both tracks)
  // code from Paul Bourke,
  // http://astronomy.swin.edu.au/~pbourke/geometry/lineline3d/

  double eps(1.e-6);

  close1 = EPoint(0.,0.,0.);
  close2 = EPoint(0.,0.,0.);

  EVector udir1 = dir1.unit();
  EVector udir2 = dir2.unit();

  EPoint t1b = ori1;
  EPoint t2b = ori2;

  EVector v0 = ori1 - ori2;
  EVector v1 = udir1;
  if (fabs(v1.x())  < eps && fabs(v1.y())  < eps && fabs(v1.z())  < eps)
    return false;
  EVector v2 = udir2;
  if (fabs(v2.x())  < eps && fabs(v2.y())  < eps && fabs(v2.z())  < eps)
    return false;

  double d02 = v0.Dot(v2);
  double d21 = v2.Dot(v1);
  double d01 = v0.Dot(v1);
  double d22 = v2.Dot(v2);
  double d11 = v1.Dot(v1);

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
// thetaTracks
//=============================================================================
double PVSeed3DOfflineTool::thetaTracks(const State& state1,
                                 const State& state2) {
  EVector dir1(state1.tx(),state1.ty(),1.);
  EVector dir2(state2.tx(),state2.ty(),1.);
  EVector udir1 = dir1.unit();
  EVector udir2 = dir2.unit();
  double ct = udir1.Dot(udir2);
  return ct;
}

//=============================================================================
// closestDistance
//=============================================================================
bool PVSeed3DOfflineTool::xPointParameters(const State& state1, const State& state2,
                       double & distance, double & distanceChi2, EPoint & closestPoint) {

  distance = 1.e10;
  EVector dis(0.,0.,1.e6);
  EPoint pos1(0.,0.,0.);
  EPoint pos2(0.,0.,0.);
  EVector dir1(state1.tx(),state1.ty(),1.);
  EVector dir2(state2.tx(),state2.ty(),1.);
  EPoint  ori1(state1.x(),state1.y(),state1.z());
  EPoint  ori2(state2.x(),state2.y(),state2.z());
  bool ok = closestPoints(ori1,dir1,ori2,dir2,pos1,pos2);
  if (!ok) return ok;
  closestPoint.SetXYZ(0.5*(pos1.x()+pos2.x()), 0.5*(pos1.y()+pos2.y()), 0.5*(pos1.z()+pos2.z()));
  distance = sqrt((pos1 - pos2).Mag2());
  double errxy2 = state1.errX2() + state1.errY2() + state2.errX2() + state2.errY2();
  distanceChi2 = 1.e9;
  if ( errxy2 > 0. ) distanceChi2 = distance*distance/errxy2;
  return ok;;
}

//=============================================================================
// weighedMean
//=============================================================================
void PVSeed3DOfflineTool::wMean(std::vector<closeNode> & close_nodes, seedState* base_state,
                               seedPoint & pseed) {

   pseed.position.SetXYZ(0., 0., 0.);
   pseed.error.SetXYZ(0., 0., 0.);
   pseed.multiplicity = close_nodes.size();
   if ( close_nodes.size() < 2 ) return;

   double sum_wx = 0.;
   double sum_wy = 0.;
   double sum_wz = 0.;
   double sum_wxx =0.;
   double sum_wyy =0.;
   double sum_wzz =0.;

   std::vector<closeNode>::iterator it;
   for ( it = close_nodes.begin(); it != close_nodes.end(); it++ ) {
     if ( it->take == 0 ) continue;

     State& state1 = base_state->lbstate;
     State& state2 = it->seed_state->lbstate;

     //     double errxy2 = 0.1*0.1;
     double err1xy2 = (state1.errX2() + state1.errY2());
     double err2xy2 = (state2.errX2() + state2.errY2());
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

   double ex = sqrt(1./sum_wx);
   double ey = sqrt(1./sum_wy);
   double ez = sqrt(1./sum_wz);

   pseed.position.SetXYZ(x,y,z);
   pseed.error.SetXYZ(ex,ey,ez);
   pseed.multiplicity = close_nodes.size();
   /*

      cout << " www mean  "
          << x  << " "
          << y  << " "
          << z  << "  err "
          << ex  << " "
          << ey  << " "
          << ez  << " "
          << endl;
   */
   return;
}

//=============================================================================
// simpleMean
//=============================================================================
bool PVSeed3DOfflineTool::simpleMean(std::vector<closeNode> & close_nodes, seedPoint & pseed) {

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

   EPoint pmean;
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
