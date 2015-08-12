// Include files 

// STL
#include <cmath>

// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// local
#include "PVSeedTool.h"

bool  vtxcomp( vtxCluster *first, vtxCluster *second ) {
    return first->z < second->z;
}
bool  multcomp( vtxCluster *first, vtxCluster *second ) {
    return first->ntracks > second->ntracks;
}
bool  paircomp( pair_to_merge *first, pair_to_merge *second ) {
    return first->chi2dist < second->chi2dist;
}

//-----------------------------------------------------------------------------
// Implementation file for class : PVSeedTool
//
// 2005-11-19 : Mariusz Witek
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( PVSeedTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PVSeedTool::PVSeedTool( const std::string& type,
                    const std::string& name,
                    const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_pvecp2m(0),
    m_vecp2m(0),
    m_p2mstatic(0),
    m_maxChi2Merge(0.),
    m_factorToIncreaseErrors(0.),
    m_minClusterMult(0),
    m_dzCloseTracksInCluster(0.),
    m_minCloseTracksInCluster(0),
    m_highMult(0),
    m_ratioSig2HighMult(0.),
    m_ratioSig2LowMult(0.),
    m_x0MS(0.),
    m_scatCons(0.)
{
  declareInterface<IPVSeeding>(this);
  declareProperty( "x0MS",                         m_x0MS          =  0.01          );

  // steering parameters for merging procedure
  declareProperty( "maxChi2Merge",            m_maxChi2Merge            = 25. );
  declareProperty( "factorToIncreaseErrors" , m_factorToIncreaseErrors  = 15. );

  // steering parameters for final cluster selection			
  declareProperty( "minClusterMult",          m_minClusterMult =   3     );
  declareProperty( "dzCloseTracksInCluster",  m_dzCloseTracksInCluster  =   5.*Gaudi::Units::mm );
  declareProperty( "minCloseTracksInCluster", m_minCloseTracksInCluster =   3     );
  declareProperty( "highMult",                m_highMult                =  10     );
  declareProperty( "ratioSig2HighMult",       m_ratioSig2HighMult       = 1.0     );
  declareProperty( "ratioSig2LowMult",        m_ratioSig2LowMult        = 0.9     );

  m_p2mstatic=5000;

  // m_vecp2m used in static manner to reduce execution time
  m_vecp2m.reserve(m_p2mstatic);
  m_pvecp2m.reserve(m_p2mstatic);
  for(int i=0; i<m_p2mstatic; i++) {
    pair_to_merge pm;
    m_vecp2m.push_back(pm);
  }

  double X0 = m_x0MS;
  m_scatCons = (13.6*sqrt(X0)*(1.+0.038*log(X0)));

}
//=============================================================================
// Destructor
//=============================================================================
PVSeedTool::~PVSeedTool() {}

//=============================================================================
// getSeeds
//=============================================================================
void PVSeedTool::getSeeds(std::vector<const LHCb::Track*>& inputTracks, 
			  const Gaudi::XYZPoint& beamspot,
			  std::vector<Gaudi::XYZPoint>& seeds) {

  if(inputTracks.size() < 3 ) return; 

  std::vector<vtxCluster> vclusters;
  std::vector<double> zseeds;

  std::vector<const LHCb::Track*>::iterator it;
  for ( it = inputTracks.begin(); it != inputTracks.end(); it++ ) {
    const LHCb::Track* ptr = (*it);

    double sigsq; 
    double zclu;

    if ( ptr->type() == LHCb::Track::VeloR) {
      zclu = ptr->firstState().z() - ptr->firstState().x()/ptr->firstState().tx();
      errorForPVSeedFinding(ptr->firstState().tx(), 0.0, sigsq);
    }
    else {
      zclu = zCloseBeam(ptr,beamspot); 
      errorForPVSeedFinding(ptr->firstState().tx(), ptr->firstState().ty(),sigsq);
    }
    if ( fabs(zclu)>2000.) continue;
    vtxCluster clu;
    clu.z = zclu;
    clu.sigsq = sigsq;
    clu.sigsqmin = clu.sigsq;
    clu.ntracks = 1;
    vclusters.push_back(clu);
  } 

  findClusters(vclusters,zseeds);

  for ( std::vector<double>::iterator iz =  zseeds.begin(); iz != zseeds.end(); iz++ ) {
    Gaudi::XYZPoint ep(beamspot.X(), beamspot.Y(), *iz);
    seeds.push_back(ep);
  }

}


void PVSeedTool::findClusters(std::vector<vtxCluster>& vclus, 
                              std::vector<double>& zclusters) {


  zclusters.clear();

  if(vclus.size()<1) return;

  std::vector<vtxCluster*> vtmp;
  vtmp.reserve(300);

  std::vector<vtxCluster*> pvclus;
  pvclus.reserve(300);

  std::vector<vtxCluster>::iterator itvtx;

  for(itvtx = vclus.begin(); itvtx != vclus.end(); itvtx++) {
    itvtx->sigsq *= m_factorToIncreaseErrors*m_factorToIncreaseErrors; // blow up errors 
    itvtx->sigsqmin = itvtx->sigsq;
    vtxCluster* pivc = &(*itvtx);
    pvclus.push_back(pivc);
  }

  std::vector<vtxCluster*>::iterator ivc,ivc1,ivc2,ivc2up;

  std::stable_sort(pvclus.begin(),pvclus.end(),vtxcomp);
  //  print_clusters(pvclus);

  bool more_merging = true;
  while (more_merging) {
  // find pair of clusters for merging 

    // refresh flag for this iteration
    for(ivc = pvclus.begin(); ivc != pvclus.end(); ivc++) {
      (*ivc)->not_merged = 1;
    }

    // for a given cluster look only up to a few consequtive ones to merge
    // "a few" might become a property?
    ivc2up = pvclus.begin();
    for (int i=0; i<5; i++) { 
      if(ivc2up != pvclus.end()) ivc2up++;
    } 

    // use vecp2m in a static manner 
    int im = 0;
    m_pvecp2m.clear();
    for(ivc1 = pvclus.begin(); ivc1 != pvclus.end()-1; ivc1++) {
      if(ivc2up != pvclus.end()) ivc2up++;
      for(ivc2 = ivc1+1; ivc2 != ivc2up; ivc2++) {
        double zdist = fabs((*ivc1)->z-(*ivc2)->z);
        double chi2dist = zdist*zdist/((*ivc1)->sigsq+(*ivc2)->sigsq);
        if(chi2dist<m_maxChi2Merge && im<m_p2mstatic) {
                    
          pair_to_merge* ppm = &(m_vecp2m[im]);
          ppm->chi2dist = chi2dist;
          ppm->first  = *ivc1;
          ppm->second = *ivc2;
          m_pvecp2m.push_back(ppm);
          im++;
        }
      }
    }

    // done if no more pairs to merge
    if(m_pvecp2m.size() < 1) { 
      more_merging = false;
    }
    else {
      // sort if number of pairs reasonable. Sorting increases efficency.
      if(m_pvecp2m.size()<100) std::stable_sort(m_pvecp2m.begin(), m_pvecp2m.end(), paircomp);
      std::vector<pair_to_merge*>::iterator itp2m;
      
      // merge pairs 
      for(itp2m = m_pvecp2m.begin(); itp2m != m_pvecp2m.end(); itp2m++) {
        vtxCluster* pvtx1 = (*itp2m)->first;
        vtxCluster* pvtx2 = (*itp2m)->second;
        if(pvtx1->not_merged == 1 && pvtx2->not_merged == 1) {  

          double z1 = pvtx1->z;
          double z2 = pvtx2->z;
          double s1 = pvtx1->sigsq;
          double s2 = pvtx2->sigsq;
          double s1min = pvtx1->sigsqmin;
          double s2min = pvtx2->sigsqmin;
          double sigsqmin = s1min;
          if(s2min<s1min) sigsqmin = s2min;

          double w_inv = (s1*s2/(s1+s2));
          double zmerge = w_inv*(z1/s1+z2/s2);

          pvtx1->z        = zmerge;
          pvtx1->sigsq    = w_inv;
          pvtx1->sigsqmin = sigsqmin;
          pvtx1->ntracks += pvtx2->ntracks;
          pvtx2->ntracks  = 0;  // mark second cluster as used
          pvtx1->not_merged = 0;
          pvtx2->not_merged = 0;
	}
      }

      // remove clusters which where used
      vtmp.clear();
      for(ivc=pvclus.begin(); ivc != pvclus.end(); ivc++) {
        vtxCluster* cl = *ivc;
        if( cl->ntracks > 0 ) vtmp.push_back(cl);
      }
      pvclus.clear();
      for(ivc=vtmp.begin(); ivc !=vtmp.end(); ivc++) {
        vtxCluster* cl = *ivc;
        if( cl->ntracks > 0 ) pvclus.push_back(cl);
      }
      //      print_clusters(pvclus);

    }
  }

  // End of clustering.

  // Sort according to multiplicity

  if(pvclus.size()>1) {
    std::stable_sort(pvclus.begin(),pvclus.end(),multcomp);
  }
  
  // Select good clusters.

  for(ivc=pvclus.begin(); ivc != pvclus.end(); ivc++) {
    int n_tracks_close = 0;
    for(itvtx = vclus.begin(); itvtx != vclus.end(); itvtx++) {
      if(fabs(itvtx->z - (*ivc)->z ) < m_dzCloseTracksInCluster ) n_tracks_close++;
    }

    double dist_to_closest = 1000000.;
    if(pvclus.size() > 1) {
      for(ivc1=pvclus.begin(); ivc1 != pvclus.end(); ivc1++) {
	if( ivc!=ivc1 && ( fabs((*ivc1)->z-(*ivc)->z) < dist_to_closest) ) {
	  dist_to_closest = fabs((*ivc1)->z-(*ivc)->z);
	}
      } 
    }

    // ratio to remove clusters made of one low error track and many large error ones 
    double rat = (*ivc)->sigsq/(*ivc)->sigsqmin;
    int igood = 0;
    int ntracks = (*ivc)->ntracks;
    if( ntracks >= m_minClusterMult ) { 
      if( dist_to_closest>10. && rat<0.95) igood=1;
      if( ntracks >= m_highMult && rat < m_ratioSig2HighMult)  igood=1;
      if( ntracks <  m_highMult && rat < m_ratioSig2LowMult )  igood=1;
    }
    // veto
    if( n_tracks_close < m_minCloseTracksInCluster ) igood = 0;
    if(igood == 1)  zclusters.push_back((*ivc)->z);

  }

  //  print_clusters(pvclus);

}

void PVSeedTool::errorForPVSeedFinding(double tx, double ty, double &sigz2) {

  // the seeding results depend weakly on this eror parametrization

    double pMean = 3000.*Gaudi::Units::MeV;

    double tanTheta2 =  tx * tx + ty * ty;
    double sinTheta2 =  tanTheta2 / ( 1. + tanTheta2 );

    // assume that first hit in VD at 8 mm
    double distr        = 8.*Gaudi::Units::mm;
    double dist2        = distr*distr/sinTheta2;
    double sigma_ms2    = m_scatCons * m_scatCons * dist2 / (pMean*pMean);
    double fslope2      = 0.0005*0.0005;
    double sigma_slope2 = fslope2*dist2;

    sigz2 = (sigma_ms2 + sigma_slope2) / sinTheta2; 

}



void PVSeedTool::print_clusters(std::vector<vtxCluster*>& pvclus) {

  std::vector<vtxCluster*>::iterator ivc;

  std::cout << pvclus.size() << " clusters at this iteration" << std::endl;
  for(ivc=pvclus.begin(); ivc != pvclus.end(); ivc++) {
     std::cout << (*ivc)->ntracks << " " << (*ivc)->z << " " 
            <<  (*ivc)->sigsq << " " <<  (*ivc)->sigsqmin << std::endl;
  }

}

double PVSeedTool::zCloseBeam(const LHCb::Track* track, const Gaudi::XYZPoint& beamspot){
   

//  Gaudi::XYZVector unitVect;
//  unitVect = track->firstState().slopes().Unit();
//  LHCb::State& stateG = track->firstState(); 
//
//  double zclose = stateG.z() - unitVect.z() * 
//         (unitVect.x() * stateG.x() + unitVect.y() * stateG.y()) /
//         (1.0 - pow(unitVect.z(),2)); 
//
//  return zclose;

  Gaudi::XYZPoint tpoint = track->position();
  Gaudi::XYZVector tdir = track->slopes();

  double wx = ( 1. + tdir.x() * tdir.x() ) / track->firstState().errX2();
  double wy = ( 1. + tdir.y() * tdir.y() ) / track->firstState().errY2();

  double x0 = tpoint.x() - tpoint.z() * tdir.x() - beamspot.X();
  double y0 = tpoint.y() - tpoint.z() * tdir.y() - beamspot.Y();
  double den = wx * tdir.x() * tdir.x() + wy * tdir.y() * tdir.y();
  double zAtBeam = - ( wx * x0 * tdir.x() + wy * y0 * tdir.y() ) / den ;

  double xb = tpoint.x() + tdir.x() * ( zAtBeam - tpoint.z() ) - beamspot.X();
  double yb = tpoint.y() + tdir.y() * ( zAtBeam - tpoint.z() ) - beamspot.Y();
  double rAtBeam = sqrt( xb*xb + yb*yb );

  if (rAtBeam < 0.5) {
    return zAtBeam;
  } else {
    return 10e8;
  }
}

//=============================================================================
