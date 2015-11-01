
// local
#include "MuIDMonitor.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MuIDMonitor
//
// first version
// 19/01/2008 : A. Sarti & M. Palutan
//
// Algorithm needed for muon Id monitoring studies
//-----------------------------------------------------------------------------
using namespace LHCb;
using namespace Gaudi;
using namespace Gaudi::Utils;
using namespace Gaudi::Units;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MuIDMonitor )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuIDMonitor::MuIDMonitor( const std::string& name,
                          ISvcLocator* pSvcLocator)
: DaVinciHistoAlgorithm ( name , pSvcLocator ),
  m_distPion( 9, 1.0 ),
  m_distMuon( 9, 1.0 ),
  m_xfoiParam1( 20, 1.0 ),
  m_xfoiParam2( 20, 1.0 ),
  m_xfoiParam3( 20, 1.0 ),
  m_yfoiParam1( 20, 1.0 ),
  m_yfoiParam2( 20, 1.0 ),
  m_yfoiParam3( 20, 1.0 )
{

  using namespace boost::assign;

  // Destination of MuonPID
  declareProperty("MuonTrackLocation",
                  m_MuonTracksPath = LHCb::TrackLocation::Muon);

  // Mass Window
  declareProperty( "MassMean", m_MassMean = 1115.68);
  declareProperty( "MassWindow", m_MassWin = 100.0);
  declareProperty( "JpsiAnalysis", m_JPAna = 0);
  declareProperty( "LambdaAnalysis", m_LMAna = 0);
  declareProperty( "HitInFoi", m_hitInFoi = 0);

  declareProperty( "EffMassWin", m_EffWin = 20);

  // Pre-selection momentum
  declareProperty( "PreSelMomentum", m_PreSelMomentum = 3000.0);

  // Different depths of stations considered in different momentum ranges
  m_MomentumCuts.push_back(6000.);
  m_MomentumCuts.push_back(10000.);
  declareProperty( "MomentumCuts", m_MomentumCuts );

  // function that defines the field of interest size
  // here momentum is scaled to Gaudi::Units::GeV....
  // new formula: p(1) + p(2)*momentum + p(3)*exp(-p(4)*momentum)

  declareProperty( "XFOIParameter1", m_xfoiParam1 );
  declareProperty( "XFOIParameter2", m_xfoiParam2 );
  declareProperty( "XFOIParameter3", m_xfoiParam3 );
  declareProperty( "YFOIParameter1", m_yfoiParam1 );
  declareProperty( "YFOIParameter2", m_yfoiParam2 );
  declareProperty( "YFOIParameter3", m_yfoiParam3 );

  declareProperty("FOIfactor",m_foifactor = 1.);

  declareProperty("distMuon",m_distMuon);
  declareProperty("distPion",m_distPion);

}

//=============================================================================
// Destructor
//=============================================================================
MuIDMonitor::~MuIDMonitor() {}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode MuIDMonitor::initialize()
{

  // Sets up various tools and services
  const StatusCode sc = DaVinciHistoAlgorithm::initialize();
  if ( sc.isFailure() ) { return sc; }

  debug() << " MuIDMonitor v5r2 " << endmsg;
  debug() << "==> Initialise" << endmsg;

  m_extrFail = 0;
  m_NStation = 0;
  m_NRegion = 0;
  MuonBasicGeometry basegeometry( detSvc(),msgSvc());
  m_NStation= basegeometry.getStations();
  m_NRegion = basegeometry.getRegions();
  int i=0;
  while(i<m_NStation){
    m_stationNames.push_back(basegeometry.getStationName(i));
    // debug()   <<" station "<<i<<" "<<m_stationNames[i]<<endmsg;
    i++;
  }

  // set the size of the local vectors
  m_padSizeX.resize(m_NStation * m_NRegion);
  m_padSizeY.resize(m_NStation * m_NRegion);
  m_regionInnerX.resize(m_NStation);
  m_regionOuterX.resize(m_NStation);
  m_regionInnerY.resize(m_NStation);
  m_regionOuterY.resize(m_NStation);

  // fill local arrays of pad sizes and region sizes
  m_mudet=getDet<DeMuonDetector>("/dd/Structure/LHCb/DownstreamRegion/Muon");
  int station,region;
  for(station = 0 ; station < m_NStation ; station++ ){
    m_regionInnerX[station] = m_mudet->getInnerX(station);
    m_regionOuterX[station] = m_mudet->getOuterX(station);
    m_regionInnerY[station] = m_mudet->getInnerY(station);
    m_regionOuterY[station] = m_mudet->getOuterY(station);
    m_stationZ[station] = m_mudet->getStationZ(station);
    for(region = 0 ; region < m_NRegion ; region++ ){
      m_padSizeX[station * m_NRegion + region]=m_mudet->getPadSizeX(station,region);
      m_padSizeY[station * m_NRegion + region]=m_mudet->getPadSizeY(station,region);

      if(m_padSizeX[station * m_NRegion + region]==0){
        error() << "Muon Chamber Pad Size could not be retrieved !!!" <<  endmsg;
        return StatusCode::FAILURE;
      }
    }
  }

  double myJpb[7] = {0,7000,9000,12000,16000,26000,70000};
  double myLmb[21] = {0,4000,5000,6000,7000,8000,9000,10000,12000,14000,16000,18000,20000,25000,
                      30000,35000,40000,50000,60000,70000,80000};
  for(int iD =0; iD<7; iD++)
    m_edgesJPX.push_back(myJpb[iD]);

  for(int iD =0; iD<21; iD++)
    m_edgesLMX.push_back(myLmb[iD]);

  if( m_MomentumCuts.empty() ||
      m_xfoiParam1.size() != (unsigned)m_NStation*m_NRegion ||
      m_xfoiParam2.size() != (unsigned)m_NStation*m_NRegion ||
      m_xfoiParam3.size() != (unsigned)m_NStation*m_NRegion ||
      m_yfoiParam1.size() != (unsigned)m_NStation*m_NRegion ||
      m_yfoiParam2.size() != (unsigned)m_NStation*m_NRegion ||
      m_yfoiParam3.size() != (unsigned)m_NStation*m_NRegion
      ){
    err() << "OPTIONS initialising MuIDMonitor are missing"
          << " or wrong size for " << m_NStation << " stations and "
          << m_NRegion << " regions"
          << endmsg;
    return StatusCode::FAILURE;
  }

  if( m_MomentumCuts.size() != 2 ){
    err()
      << "OPTIONS are wrong:"
      << " size of MomentumCuts vector is not correct"
      << endmsg;
    return StatusCode::FAILURE;
  }

  debug()  << " Momentum bins are (MeV/c) " <<endmsg;
  debug()  << " PreSelMomentum = "<<  m_PreSelMomentum << endmsg;

  std::vector<double>::const_iterator iMom;
  for(iMom = m_MomentumCuts.begin() ; iMom != m_MomentumCuts.end() ; iMom++){
    debug()  << "Mom. cuts = " << *iMom << endmsg ;
  }

  if( m_distPion.size() != 9 || m_distMuon.size() != 9){
    err() << "OPTIONS are wrong:"
          <<" size of m_distPion or m_distMuon vector is not correct"
          << endmsg;
    return StatusCode::FAILURE;
  }

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MuIDMonitor::execute() {

  debug()  << "==> Execute" << endmsg;

  double dllMu(-999),dllBg(-999);

  StatusCode sc = StatusCode::SUCCESS;

  m_coordPos.clear();
  m_coordPos.resize(m_NStation * m_NRegion);
  m_muonMap.clear();
  m_MuProb = 0;
  m_LklhMu = -10000; m_LklhPi = -10000;

  if(m_hitInFoi) {
    // get the MuonCoords for each station in turn
    LHCb::MuonCoords* coords = get<LHCb::MuonCoords>(LHCb::MuonCoordLocation::MuonCoords);
    if ( coords==0 ) {
      return Error( "Cannot retrieve MuonCoords" );
    }

    // loop over the coords
    LHCb::MuonCoords::const_iterator iCoord;
    for ( iCoord = coords->begin() ; iCoord != coords->end() ; iCoord++ ){

      int region = (*iCoord)->key().region();
      int station = (*iCoord)->key().station();
      double x,dx,y,dy,z,dz;
      LHCb::MuonTileID tile=(*iCoord)->key();
      sc = m_mudet->Tile2XYZ(tile,x,dx,y,dy,z,dz);
      if (sc.isFailure()){
        warning() << "Failed to get x,y,z of tile " << tile << endmsg;
        continue;
      }

      m_coordPos[station*m_NRegion+region].
        push_back(coordExtent_(x,dx,y,dy,*iCoord));

    }
  }

  double pl1(0.),pl2(0.),pl(0.);
  bool plotFlag(kFALSE);

  //Loop on matched tracks
  const Particle::Range selParts = this->particles();
  if (selParts.size() == 0) return StatusCode::SUCCESS;

  double cosTr(0),plTr(0);
  int id_jp(0);
  int size = 2*selParts.size();
  std::vector<int> my_pid;
  std::vector<int> my_chg;
  std::vector<double> pt_pid;
  std::vector<double> pl_pid;
  std::vector<int> idTag;
  my_pid.reserve(size);
  my_chg.reserve(size);
  pt_pid.reserve(size);
  pl_pid.reserve(size);
  idTag.reserve(size);
  double PMass(0);
  std::string name, nameD, tit; bool profFl(kFALSE);

  double eEcal(0), eHcal(0);
  for (Particle::Range::const_iterator iP = selParts.begin(); iP != selParts.end(); ++iP) {
    //Main particle daughters
    Particle::ConstVector dau = (*iP)->daughtersVector();

    //Mass of the 2body pair
    PMass = (*iP)->measuredMass();
    plot1D (PMass, "IM","Mass: tag muon only",m_MassMean-m_MassWin, m_MassMean+m_MassWin, 100 );
    pl1 = pl2 = 0; pl = -999;
    cosTr = 0; plTr = 0;
    plotFlag = kFALSE;

    for (Particle::ConstVector::const_iterator iD = dau.begin(); iD != dau.end(); ++iD) {

      const LHCb::ProtoParticle* sel_pp = (*iD)->proto();
      if(sel_pp){

        const LHCb::Track* sel_trk = sel_pp->track();
        /* Here you decide who is tagging and who is probing */

        if(m_JPAna) {
          //Here I need to define the Tag/Probe Mu:: my_pid == 1 --> Tag; 0 --> Probe
          my_pid[id_jp] = 0;
          if(sel_pp->muonPID()) {
            if(sel_trk->p()>6000 && sel_trk->pt()>1500 && sel_pp->muonPID()->IsMuon()) my_pid[id_jp] = 1;
          }
          pt_pid[id_jp] = sel_trk->pt();
          id_jp++;
        }

        if(m_LMAna) {
          //Implemented ID selection based on Armenteros Podolanski
          cosTr = ((*iP)->momentum().Px()*sel_trk->momentum().X()+
                   (*iP)->momentum().Py()*sel_trk->momentum().Y()+
                   (*iP)->momentum().Pz()*sel_trk->momentum().Z())/((*iP)->momentum().P()*sel_trk->p());
          plTr = sel_trk->p() * cosTr;

          my_chg[id_jp] = (*iD)->charge();
          pl_pid[id_jp] = plTr;
          id_jp++;
        }
      }
    }
    if(m_JPAna) {
      //In Jpsi analysis decide what is tag and what is probe
      if(my_pid[0] == 1 && my_pid[1] == 1) {
        //In case both passes tagging cuts
        idTag[0] = 1;  idTag[1] = 0;
        if(pt_pid[1]>pt_pid[0]) { idTag[1] = 1;  idTag[0] = 0; }
      } else {
        idTag[0] = my_pid[0];  idTag[1] = my_pid[1];
      }
    }
    if(m_LMAna) {
      //Lambda analysis: disentangle pion and proton
      //IdTag == 0 --> Proton; IdTag == 1 --> Pion
      pl = -999;
      pl1 = pl_pid[0]; pl2 = pl_pid[1];
      if(my_chg[1]>0) {pl1 = pl_pid[1]; pl2 = pl_pid[0];}

      if(pl1+pl2) {
        pl = (pl1-pl2)/(pl1+pl2);
        if(pl<0) {
          idTag[0] = 1; idTag[1] = 0;
          if(my_chg[1]>0) { idTag[0] = 0; idTag[1] = 1; }
        } else {
          idTag[0] = 0; idTag[1] = 1;
          if(my_chg[1]>0) { idTag[0] = 1; idTag[1] = 0; }
        }
      }
    }
    /*
      From now on you know who is tag and who is probe OR who is pion and who is proton
    */

    id_jp = 0;
    for (Particle::ConstVector::const_iterator iD = dau.begin(); iD != dau.end(); ++iD) {
      const LHCb::ProtoParticle* protop = (*iD)->proto();
      if(protop){

        m_dist = -999.; m_region = -1; m_LklhMu = -10000.; m_LklhPi = -10000.;

        const LHCb::Track* track = protop->track();

        //Extrapolate the track that is not belonging to the already IDed mu
        sc = trackExtrapolate(track);
        if ( sc.isFailure() ){
          warning() << " trackExtrapolate failed for track " << track << endmsg;
          m_extrFail++;
          return StatusCode::SUCCESS;
        }

        const LHCb::MuonPID * pMuid = protop->muonPID();

        if(m_JPAna) {
          //Hit in 2 * Foi just for the Probe!
          if(!idTag[id_jp]) {
            if(m_hitInFoi) {
              plotFlag = selectHitInFOI();
            } else {
              plotFlag = kTRUE;
            }
          } else {
            //Tag muon has it in *2 foi PER DEFINITION
            plotFlag = kTRUE;
          }
        }

        if(m_LMAna)  plotFlag = kTRUE;

        // Reading who's who:
        // idTag = 0 : Proton / Probe
        // idTag = 1 : Pion / Tag
        if(plotFlag) {
          if(m_JPAna ) {
            if(!idTag[id_jp]) { nameD = "probe/"; name = "probe"; tit = "probe #mu "; }
            else { nameD = "tag/"; name = "tag"; tit = "tag #mu "; }
          }

          if(m_LMAna ) {
            if(!idTag[id_jp]) {nameD = "proton/"; name = "proton/"; tit ="proton "; }
            else { nameD = "pion/"; name = "pion/"; tit = "#pi ";  }
          }


          plot1D ( (track)->p(), nameD+"P",tit+" P distribution",0., 100000., 100 );
          plot1D ( (track)->pt(), nameD+"Pt",tit+" Pt distribution",0., 20000., 100 );
          plot1D ( (track)->charge(), nameD+"Chg",tit+" charge distribution",-1.5, 1.5, 3 );

          if(pMuid){
            dllMu = TMath::Exp(pMuid->MuonLLMu());
            dllBg = TMath::Exp(pMuid->MuonLLBg());

            plot1D ( dllMu, nameD+"MuProb",tit+" Mu likelihood",0., 1., 100 );
            plot1D ( dllBg, nameD+"BkgProb",tit+" Bkg likelihood",0., 1., 100 );
            plot1D ( pMuid->MuonLLMu()-pMuid->MuonLLBg(), nameD+"Dll",tit+" Delta Log likelihood",
                     -10., 10., 100 );

            // calculate the distance from the hit to the extrapolated position
            m_dist = calc_closestDist(pMuid,m_MomM1,closest_region);

            if(m_dist) {
              plot1D ( m_dist, nameD+"Dist",tit+" Squared Distance",0., 500., 100 );
              plot1D ( m_dist, nameD+"ZmDist",tit+" Squared Distance (zoom)",0., 20., 100 );
              if(m_region == 0) {
                plot1D ( m_dist, nameD+"R1Dist",tit+" R1 Squared Distance ",0., 500., 100 );
                plot1D ( m_dist, nameD+"ZmR1Dist",tit+" R1 Squared Distance (zoom)",0., 20., 100 );
              } else if(m_region == 1) {
                plot1D ( m_dist, nameD+"R2Dist",tit+" R2 Squared Distance ",0., 500., 100 );
                plot1D ( m_dist, nameD+"ZmR2Dist",tit+" R2 Squared Distance (zoom)",0., 20., 100 );
              } else if(m_region == 2) {
                plot1D ( m_dist, nameD+"R3Dist",tit+" R3 Squared Distance ",0., 500., 100 );
                plot1D ( m_dist, nameD+"ZmR3Dist",tit+" R3 Squared Distance (zoom)",0., 20., 100 );
              } else if(m_region == 3) {
                plot1D ( m_dist, nameD+"R4Dist",tit+" R4 Squared Distance ",0., 500., 100 );
                plot1D ( m_dist, nameD+"ZmR4Dist",tit+" R4 Squared Distance (zoom)",0., 20., 100 );
              }
            }
          }//If pmuid

          plot1D ( m_region, nameD+"R_illu",tit+" Region illumination",-0.5, 3.5, 4 );

          eEcal =  protop->info( LHCb::ProtoParticle::CaloEcalE , -1 * Gaudi::Units::GeV  );
          eHcal =  protop->info( LHCb::ProtoParticle::CaloHcalE , -1 * Gaudi::Units::GeV  );

          plot1D ( eEcal, nameD+"ecal",tit+" Ecal Energy deposit",0., 3000., 200 );
          plot1D ( eHcal, nameD+"hcal",tit+" Hcal Energy deposit",0., 10000., 200 );

          if(m_region == 0) {
            plot2D( m_trackX[1], m_trackY[1], nameD+"R1Illu",tit+" Illumination R1",-5000.,5000.,-5000.,5000.,100,100);
          } else if(m_region == 1) {
            plot2D( m_trackX[1], m_trackY[1], nameD+"R2Illu",tit+" Illumination R2",-5000.,5000.,-5000.,5000.,100,100);
          } else if(m_region == 2) {
            plot2D( m_trackX[1], m_trackY[1], nameD+"R3Illu",tit+" Illumination R3",-5000.,5000.,-5000.,5000.,100,100);
          } else if(m_region == 3) {
            plot2D( m_trackX[1], m_trackY[1], nameD+"R4Illu",tit+" Illumination R4",-5000.,5000.,-5000.,5000.,100,100);
          }

          /*
            Jpsi plots: Efficiency
          */
          if(m_JPAna) {
            //Only for probe muons
            if(!idTag[id_jp]) {
              plot1D (PMass, "IM_"+name,"Mass: tag + probe with hit in 2 foi",m_MassMean-m_MassWin, m_MassMean+m_MassWin, 100 );

              if(fabs(PMass-m_MassMean)<m_EffWin) {
                plot1D ( (track)->p(), nameD+"P_effDeno",tit+" P distribution",m_edgesJPX);
                profFl = kFALSE;
                if(pMuid) {
                  if(pMuid->IsMuonLoose()) {
                    profFl = kTRUE;
                  }
                }
                profile1D ( (track)->p(), profFl, nameD+"Prof_eff",tit+" Efficiency (sig+bkg) vs momentum",m_edgesJPX);
              }

              if(pMuid) {
                if(pMuid->IsMuonLoose()) {
                  plot1D (PMass, "IM_ism_"+name,"Mass: tag + probe after IsMuonLoose",
                          m_MassMean-m_MassWin, m_MassMean+m_MassWin, 100 );
                  if(fabs(PMass-m_MassMean)<m_EffWin) {
                    plot1D ( (track)->p(), nameD+"P_effNume",tit+" P distribution (after ismuon)",
                             m_edgesJPX);
                  }
                }//Requires IsMuonloose
              }//Requires pMuid for ismuon loose check

            }//Only for Probe muons
          }//JP analysis plots for efficiency

          /*
            Lambda plots: Efficiency
          */
          if(m_LMAna) {

            //For both pions and protons
            if(idTag[id_jp]) { plot1D (PMass, "IM_"+name,tit+" Mass Lambda Candidates",
                                       m_MassMean-m_MassWin, m_MassMean+m_MassWin, 100 ); }
            else  { plot1D (PMass, "IM_"+name,tit+" Mass Lambda Candidates",
                            m_MassMean-m_MassWin, m_MassMean+m_MassWin, 100 ); }
            if(fabs(PMass-m_MassMean)<m_EffWin) {

              if(idTag[id_jp]) { plot1D ( (track)->p(), nameD+"P_effDeno",tit+" P distribution",m_edgesLMX);}
              else { plot1D ( (track)->p(), nameD+"P_effDeno",tit+" P distribution",m_edgesLMX);}

              profFl = kFALSE;
              if(pMuid) {
                if(pMuid->IsMuonLoose()) {
                  profFl = kTRUE;
                }
              }
              if(idTag[id_jp]) {profile1D ( (track)->p(), profFl, nameD+"Prof_eff",tit+" Efficiency (sig+bkg) vs momentum",m_edgesLMX);}
              else {profile1D ( (track)->p(), profFl, nameD+"Prof_eff",tit+" Efficiency (sig+bkg) vs momentum",m_edgesLMX);}

            }

            if(pMuid) {

              if(pMuid->IsMuonLoose()) {

                if(idTag[id_jp]) { plot1D (PMass, "IM_ism_"+name,"Mass Lambda Candidates (after ismuon)",
                                           m_MassMean-m_MassWin, m_MassMean+m_MassWin, 100 );}
                else { plot1D (PMass, "IM_ism_"+name,"Mass Lambda Candidates (after ismuon)",
                               m_MassMean-m_MassWin, m_MassMean+m_MassWin, 100 );}
                if(fabs(PMass-m_MassMean)<m_EffWin) {
                  if(idTag[id_jp]) { plot1D ( (track)->p(), nameD+"P_effNume",tit+" P distribution (after ismuon)",m_edgesLMX);}
                  else { plot1D ( (track)->p(), nameD+"P_effNume",tit+" P distribution (after ismuon)",m_edgesLMX);}
                }
              }//Requires IsMuonloose
            }//Requires pMuid for ismuon loose check

          }//LM analysis plots for efficiency


        }//Select tag and probe (pion and proton)
        id_jp++;
      }//Only if there's a protoparicle
    }//Loop on daughters
  }//Loop on particles

  //MAss plot
  //S/B
  //Efficiency
  //DLL hits from mutracks (closest) .. distance on 4 regionsxs
  setFilterPassed(true);  // Mandatory. Set to true if event is accepted.
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MuIDMonitor::finalize()
{
  debug() <<"Number of tracks that failed extrapolation:: "<<m_extrFail<<endmsg;
  // Execute base class method
  return DaVinciHistoAlgorithm::finalize();
}

//=============================================================================
// Track extrapolation
//=============================================================================
StatusCode MuIDMonitor::trackExtrapolate(const LHCb::Track *pTrack){

  resetTrackLocals();

  // get first state for the preSel cut
  const LHCb::State * state1 = &(pTrack->firstState());
  // get state closest to M1 for extrapolation
  const LHCb::State * state = &(pTrack->closestState(m_stationZ[0]));

  if(!state1){
    err() << " Failed to get 1st state from track " << endmsg;
    return StatusCode::FAILURE;
  }
  if(!state){
    err() << " Failed to get state from track " << endmsg;
    return StatusCode::FAILURE;
  }

  // get the momentum (MeV/c)
  m_Momentum = state->p();
  m_MomM1 = state1->p();

  // get the track slope
  m_trackSlopeX = state->tx();

  int ch(-1),reg(-1);
  // Project the state into the muon stations
  int station; double newX, newY;
  for(station = 0; station < m_NStation ; station++){
    // x(z') = x(z) + (dx/dz * (z' - z))
    newX = state->x() + ( state->tx() *
                          (m_stationZ[station] - state->z()) );
    newY = state->y() + ( state->ty() *
                          (m_stationZ[station] - state->z()) );
    if(station==1) {
      m_mudet->Pos2StChamberNumber(newX,newY,station,ch,reg);
      m_region = reg;
    }
    m_trackX.push_back(newX);
    m_trackY.push_back(newY);
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// return the FOI in x in a station and region for momentum (in MeV/c)
// this is a simpler version of the parameterization:
// foi = par0 + par2*exp(-par3*p)
//=============================================================================
double MuIDMonitor::foiX(const int &station, const int &region, const double &p,
                         const double &dx){
  return ( m_xfoiParam1[ station * m_NRegion + region ] +
           m_xfoiParam2[ station * m_NRegion + region ]*
           std::exp(-m_xfoiParam3[ station * m_NRegion + region ]*p/Gaudi::Units::GeV ) )*dx;

  //in the future optimize this checking that 2*dx =m_padSizeX[station * m_NRegion + region]
  //then eliminates dx from function
}

//=============================================================================
// return the FOI in y in a station and region for momentum (in MeV/c)
//=============================================================================
double MuIDMonitor::foiY(const int &station, const int &region, const double &p,
                         const double &dy){
  return ( m_yfoiParam1[ station * m_NRegion + region ] +
           m_yfoiParam2[ station * m_NRegion + region ]*
           std::exp(-m_yfoiParam3[ station * m_NRegion + region ]*p/Gaudi::Units::GeV ) )*dy;
}

//=============================================================================
// reset vectors
//=============================================================================
void MuIDMonitor::resetTrackLocals(){
  m_Momentum = -1.;
  m_MomM1 = -1.;
  m_trackSlopeX = 0.;
  m_trackX.clear();
  m_trackY.clear();
  m_occupancy.clear();
  m_CoordX.clear();
  m_occupancy.resize(m_NStation,0);
  m_CoordX.resize(m_NStation,0.);
}

//=============================================================================
// Select the muon using hit info
//=============================================================================
bool MuIDMonitor::selectHitInFOI(){

  int station(1);
  int region; bool foundHit = false;
  double x(0), dx(0), y(0), dy(0), foiXDim(0), foiYDim(0);
  for(region = 0 ; region < m_NRegion ; region++){

    if( !m_coordPos[station*m_NRegion + region].empty() ){

      std::vector<coordExtent_>::const_iterator itPos;
      for(itPos = m_coordPos[station*m_NRegion + region].begin();
          itPos != m_coordPos[station*m_NRegion + region].end();
          itPos++){

        x = itPos->m_x;
        dx = itPos->m_dx;
        y = itPos->m_y;
        dy = itPos->m_dy;


        // not optimal this should be called only once per station, region
        foiXDim = m_foifactor*foiX( station, region, m_MomM1, dx);
        foiYDim = m_foifactor*foiY( station, region, m_MomM1, dy);

        // check if the hit is in the window (2* dimension!)
        if(  ( fabs( x - m_trackX[station] ) < 2*foiXDim ) &&
             ( fabs( y - m_trackY[station] ) < 2*foiYDim )  ) {
          foundHit = true;
        }
      }// itPos
    }
  } //region

  return foundHit;
}

StatusCode MuIDMonitor::get_closest(const LHCb::MuonPID *pMuid, double *closest_x, double *closest_y, double *closest_region){
  //=============================================================================
  // comment: get the closest hit
  // authors: G. Lanfranchi & S. Furcas,
  // date: 10/5/09
  //=============================================================================

  double foiXDim,foiYDim;
  int nhits=0;

  for (int ista=0; ista<5; ista++){
    closest_x[ista] = -1;
    closest_y[ista] = -1;
    small_dist[ista] = 10000000.;
    Fdist[ista] = 0;
    closest_region[ista] = -1;
  }


  const LHCb::Track * muTrack = pMuid->muonTrack();
  if(muTrack==NULL)
  {
    return Warning("Mu Track is Null",StatusCode::FAILURE, 1);
  }


  std::vector<LHCb::LHCbID> mucoords = muTrack->lhcbIDs();
  std::vector<LHCb::LHCbID>::iterator iCoord;
  double x,dx,y,dy,z,dz; int station,region;

  for ( iCoord = mucoords.begin() ; iCoord != mucoords.end() ; iCoord++ ){

    LHCb::MuonTileID tile=iCoord->muonID();
    StatusCode sc =
      m_mudet->Tile2XYZ(tile,x,dx,y,dy,z,dz);
    if (sc.isFailure()){
      warning() << "Failed to get x,y,z of tile " << tile << endmsg;
      continue;
    }

    station = tile.station();
    region = tile.region();
    foiXDim = m_foifactor*foiX( station, region, m_MomM1, dx);
    foiYDim = m_foifactor*foiY( station, region, m_MomM1, dy);

    // only for M2-M3-M4-M5:
    if (station > 0 ) {

      if(  ( fabs( x - m_trackX[station] ) < foiXDim ) &&
           ( fabs( y - m_trackY[station] ) < foiYDim )  ) {

        Fdist[station] =((x - m_trackX[station])/dx * (x - m_trackX[station])/dx)
          +((y - m_trackY[station])/dy * (y - m_trackY[station])/dy);

        nhits++;

        if(Fdist[station] < small_dist[station]){

          small_dist[station] = Fdist[station];
          closest_x[station]  = (x - m_trackX[station]) / dx;
          closest_y[station]  = (y - m_trackY[station]) / dy;
          closest_region[station] = region;
          m_occupancy[station]++;
        }//Fdist
      }//foi
    }//station
  }//coord

  return StatusCode::SUCCESS;
}

//=====================================================================
double MuIDMonitor::calc_closestDist(const LHCb::MuonPID *pMuid, const double& p, double *closest_region){
  //=====================================================================
  // comment: Returns the squared distance calculated with closest hit
  // authors: G. Lanfranchi & S. Furcas,
  // date:    10/5/09
  //=====================================================================

  double closest_dist=0;

  StatusCode sc = get_closest(pMuid,closest_x,closest_y,closest_region);

  if ( sc.isFailure() ){
    warning() << " Closest_hit failed " << endmsg;
    return -1;
  }


  if(p>m_PreSelMomentum && p<m_MomentumCuts[0]){
    //3 or 2 stations
    if(m_occupancy[1]>0 && m_occupancy[2]>0 && m_occupancy[3]>0){//M2 &&M3 && M4
      closest_dist = (closest_x[1]*closest_x[1]+closest_y[1]*closest_y[1]) +
        (closest_x[2]*closest_x[2]+closest_y[2]*closest_y[2])+
        (closest_x[3]*closest_x[3]+closest_y[3]*closest_y[3]);
      closest_dist = closest_dist/3.;
      return closest_dist;
    }
    if(m_occupancy[1]>0 && m_occupancy[2]>0){//M2 &&M3
      closest_dist = (closest_x[1]*closest_x[1]+closest_y[1]*closest_y[1]) +
        (closest_x[2]*closest_x[2]+closest_y[2]*closest_y[2]);
      closest_dist = closest_dist/2.;
      return closest_dist;
    }
    if(m_occupancy[1]>0 && m_occupancy[3]>0){//M2 &&M4
      closest_dist = (closest_x[1]*closest_x[1]+closest_y[1]*closest_y[1]) +
        (closest_x[3]*closest_x[3]+closest_y[3]*closest_y[3]);
      closest_dist = closest_dist/2.;
      return closest_dist;
    }
    if(m_occupancy[2]>0 && m_occupancy[3]>0){//M3 &&M4
      closest_dist = (closest_x[2]*closest_x[2]+closest_y[2]*closest_y[2]) +
        (closest_x[3]*closest_x[3]+closest_y[3]*closest_y[3]);
      closest_dist = closest_dist/2.;
      return closest_dist;
    }
  }//3-6

  if(p>m_MomentumCuts[0]){
    if(m_occupancy[1]>0 && m_occupancy[2]>0 && m_occupancy[3]>0 && m_occupancy[4]>0){//M2 &&M3&&M4&&M5
      closest_dist = (closest_x[1]*closest_x[1]+closest_y[1]*closest_y[1]) +
        (closest_x[2]*closest_x[2]+closest_y[2]*closest_y[2])+
        (closest_x[3]*closest_x[3]+closest_y[3]*closest_y[3])+
        (closest_x[4]*closest_x[4]+closest_y[4]*closest_y[4]);
      closest_dist = closest_dist/4.;
      return closest_dist;
    }
    if(m_occupancy[1]>0 && m_occupancy[2]>0 && m_occupancy[3]){//M2 && M3 && M4
      closest_dist = (closest_x[1]*closest_x[1]+closest_y[1]*closest_y[1]) +
        (closest_x[2]*closest_x[2]+closest_y[2]*closest_y[2])+
        (closest_x[3]*closest_x[3]+closest_y[3]*closest_y[3]);
      closest_dist = closest_dist/3.;
      return closest_dist;
    }
    if(m_occupancy[1]>0 && m_occupancy[2]>0 && m_occupancy[4]){//M2 && M3 && M5
      closest_dist = (closest_x[1]*closest_x[1]+closest_y[1]*closest_y[1]) +
        (closest_x[2]*closest_x[2]+closest_y[2]*closest_y[2]) +
        (closest_x[4]*closest_x[4]+closest_y[4]*closest_y[4]);
      closest_dist = closest_dist/3.;
      return closest_dist;
    }
    if(m_occupancy[2]>0 && m_occupancy[3]>0 && m_occupancy[4]>0){//M3 &&M4 && M5
      closest_dist = (closest_x[2]*closest_x[2]+closest_y[2]*closest_y[2]) +
        (closest_x[3]*closest_x[3]+closest_y[3]*closest_y[3])+
        (closest_x[4]*closest_x[4]+closest_y[4]*closest_y[4]);
      closest_dist = closest_dist/3.;
      return closest_dist;
    }
    if(m_occupancy[1]>0 && m_occupancy[3]>0 && m_occupancy[4]>0){//M2 &&M4 && M5
      closest_dist = (closest_x[1]*closest_x[1]+closest_y[1]*closest_y[1]) +
        (closest_x[3]*closest_x[3]+closest_y[3]*closest_y[3])+
        (closest_x[4]*closest_x[4]+closest_y[4]*closest_y[4]);
      closest_dist = closest_dist/3.;
      return closest_dist;
    }
  }

  if(!closest_dist) return 0;
  return closest_dist;

}
