// Include files

// local
#include "PVSeed3DTool.h"

namespace {
double cos2Theta( const Gaudi::XYZVector& v1, const Gaudi::XYZVector& v2 ) {
  return std::pow(v1.Dot(v2),2) / (v1.Mag2() * v2.Mag2() );
}

//=============================================================================
// closestPointParameters
//=============================================================================
bool xPointParameters
    ( const LHCb::Track* track1, const LHCb::Track* track2,
      double& distanceSq,
      double& cos2Theta,
      Gaudi::XYZPoint& closestPoint )
{
  auto o1 = track1->position();
  auto o2 = track2->position();
  auto v1 = track1->slopes();
  auto v2 = track2->slopes();
  auto v0 = o1 - o2;

  double d11 = v1.Dot(v1);
  double d12 = v1.Dot(v2);
  double d22 = v2.Dot(v2);
  double d10 = v1.Dot(v0);
  double d20 = v2.Dot(v0);

  double det = d11 * d22 - d12 * d12;
  if ( fabs(det) < 1.0e-6 ) {
    return false; // reject parallel track pairs
  }
  double l1 = ( d12 * d20 - d22 * d10 ) / det;
  double l2 = ( d20 + d12 * l1 ) / d22;
  auto close1 = o1 + l1 * v1;
  auto close2 = o2 + l2 * v2;

  cos2Theta = d12 * d12 / d11 / d22;

  distanceSq = (close1-close2).Mag2(); // could also be evaluated as distancesq = v0.Mag2() + l1 * d10 - l2 * d20;

  closestPoint.SetCoordinates( 0.5 * ( close1.x() + close2.x() ),
                               0.5 * ( close1.y() + close2.y() ),
                               0.5 * ( close1.z() + close2.z() ) );
  return true;
}

  // helper types
  struct seedPoint {
    Gaudi::XYZPoint position;
    Gaudi::XYZPoint error;
    int multiplicity;
  };

//=============================================================================
// seedcomp
//=============================================================================
bool seedcomp( const seedPoint& first, const seedPoint& second )
{ return ( first.multiplicity > second.multiplicity ); }


  struct seedTrack {
    const LHCb::Track* lbtrack;
    bool used;
  };

  struct closeNode {
    seedTrack* seed_track;
    Gaudi::XYZPoint closest_point;
    bool take;
  };

//=============================================================================
// simpleMean
//=============================================================================
bool simpleMean(std::vector<closeNode> & close_nodes, seedPoint & pseed, double spread2_max, int minCloseTracks) {

   pseed.position.SetXYZ(0., 0., 0.);
   pseed.error.SetXYZ(0., 0., 0.);
   pseed.multiplicity = close_nodes.size();

   if ( close_nodes.size() < 2 ) return false;

   double x = 0.;
   double y = 0.;
   double z = 0.;


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
     if ( ng < minCloseTracks ) return false;
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
     }
     dist2_max = d2max;

   } // end while

   if ( ngood < minCloseTracks ) return false;
   pseed.position = pmean;
   pseed.multiplicity = ngood;
   return true;
}



//=============================================================================
// weighedMean
//=============================================================================
seedPoint wMean(const std::vector<closeNode>& close_nodes, 
                const seedTrack& base_track)
{
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

   for (const auto& cn : close_nodes) {
     if ( cn.take == 0 ) continue;

     double errxy2 = 0.1*0.1;

     double c2 = cos2Theta( base_track.lbtrack->firstState().slopes(),
                            cn.seed_track->lbtrack->firstState().slopes() );
     double ctanth2 = c2/(1.-c2);
     double errz2 = 2.*ctanth2*errxy2;

     double wx = 1./ errxy2;
     double wy = 1./ errxy2;
     double wz = 1./ errz2;
     sum_wx += wx;
     sum_wy += wy;
     sum_wz += wz;
     sum_wxx += wx*cn.closest_point.X();
     sum_wyy += wy*cn.closest_point.Y();
     sum_wzz += wz*cn.closest_point.Z();
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
   return pseed;
}

}
//-----------------------------------------------------------------------------
// Implementation file for class : PVSeed3DTool
//
// 2008-04-20 : Mariusz Witek
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( PVSeed3DTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PVSeed3DTool::PVSeed3DTool( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  : base_class ( type, name , parent )
{
  declareInterface<IPVSeeding>(this);
  auto p = declareProperty("TrackPairMaxDistance", m_TrackPairMaxDistance = 0.3 * Gaudi::Units::mm );
  p->declareUpdateHandler( [=](Property&) {
    this->m_TrackPairMaxDistanceSq = std::pow(this->m_TrackPairMaxDistance,2);
  });
  p->useUpdateHandler();
  declareProperty("MinCloseTracks",  m_MinCloseTracks  = 4);
  p = declareProperty("zMaxSpread",   m_zMaxSpread   = 3. * Gaudi::Units::mm);
  p->declareUpdateHandler( [=](Property&){
        this->m_zMaxSpreadSq = std::pow(this->m_zMaxSpread,2);
  });
  p->useUpdateHandler();
}
//=============================================================================
// getSeeds
//=============================================================================
std::vector<Gaudi::XYZPoint>
PVSeed3DTool::getSeeds(const std::vector<const LHCb::Track*>& inputTracks,
                       const Gaudi::XYZPoint& beamspot) const {
  std::vector<Gaudi::XYZPoint> seeds;
  if(inputTracks.size() < 3 ) return seeds;

  if(msgLevel(MSG::DEBUG))  {
     debug() << " This is 3D PV seeding. Beam spot is ignored. BS: " << beamspot << endmsg;
  }

  std::vector<seedPoint> seed_points;
  std::vector<seedTrack> seed_tracks;
  seed_tracks.reserve(inputTracks.size());

  for (auto it = inputTracks.begin(); it != inputTracks.end(); it++ ) {
    const LHCb::Track* ptr = (*it);
    seedTrack seedtr;
    seedtr.used = false;
    seedtr.lbtrack = ptr;
    seed_tracks.push_back(seedtr);
  }

  if(msgLevel(MSG::DEBUG))  {
     debug() << " seed_tracks.size  " << seed_tracks.size() << endmsg;
     debug() << " inputTracks.size  " << inputTracks.size() << endmsg;;
  }

  std::vector<closeNode> close_nodes;

  for(auto its1 = seed_tracks.begin(); its1 != seed_tracks.end(); its1++) {

    if ( ! its1->used ) {
      int closeTracksNumber = 0;
      close_nodes.clear();

      for(auto its2 = seed_tracks.begin(); its2 != seed_tracks.end(); its2++) {
        if ( ! its2->used  && its1 != its2 ) {

          Gaudi::XYZPoint closest_point;
          double distance2;
          double cos2th;
          const LHCb::Track* lbtr1 = its1->lbtrack;
          const LHCb::Track* lbtr2 = its2->lbtrack;
          if (xPointParameters(lbtr1, lbtr2, distance2, cos2th, closest_point)
              && distance2 <  m_TrackPairMaxDistanceSq
              && cos2th < m_TrackPairMaxCos2Theta) {

            closeTracksNumber++;
            closeNode closetr;
            closetr.take           = true;
            closetr.seed_track    =  &(*its2);
            closetr.closest_point = closest_point;
            close_nodes.push_back( closetr );
          }
        }

      }  // its2

      if ( closeTracksNumber < m_MinCloseTracks ) continue;

      // debug
      if(msgLevel(MSG::DEBUG))  {
        debug() << " close nodes (pairs of tracks wrt one track): " << endmsg;
        std::vector<closeNode>::iterator itd;
        for ( itd = close_nodes.begin(); itd != close_nodes.end(); itd++ ) {
          debug() << format(" xyz %7.3f %7.3f %7.3f ",
                     itd->closest_point.X(),  itd->closest_point.Y(), itd->closest_point.Z() )
                  << endmsg;
        }
      }

      seedPoint mean_point;
      bool OK = simpleMean(close_nodes, mean_point, m_zMaxSpread*m_zMaxSpread,m_MinCloseTracks);
      if ( OK ) {
        its1->used=true; // base track
        for (auto&  cn : close_nodes) {
          if ( cn.take ) cn.seed_track->used = true;
        }
        seed_points.push_back(wMean(close_nodes, *its1));
      }
    }
  }

  std::stable_sort(seed_points.begin(), seed_points.end(), seedcomp);
  for ( unsigned int i=0; i<seed_points.size(); i++) {
    seeds.push_back(seed_points[i].position);

    if(msgLevel(MSG::DEBUG))
    {
      debug() << " xyz seed multi  "
              << seed_points[i].position.X()  << " "
              << seed_points[i].position.Y()  << " "
              << seed_points[i].position.Z()  << " | "
              << seed_points[i].error.X()  << " "
              << seed_points[i].error.Y()  << " "
              << seed_points[i].error.Z()  << " | "
              << seed_points[i].multiplicity  << " "
              << endmsg;
    }
  }
  return seeds;
}






//=============================================================================
