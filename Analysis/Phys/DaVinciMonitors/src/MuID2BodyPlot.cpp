// Include files
#include <cstdio>
#include <map>
#include <vector>

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "Event/Particle.h"
#include <Kernel/IContextTool.h>

#include "GaudiKernel/IRegistry.h" // IOpaqueAddress
#include "Event/ODIN.h" // event & run number

#include "Event/MuonDigit.h"
#include "Event/IntLink.h"

// local
#include "MuID2BodyPlot.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MuID2BodyPlot
//
// first version
// 19/01/2008 : Alessio Sarti
//
// Algorithm needed for muon Id monitoring studies
//-----------------------------------------------------------------------------
using namespace LHCb;
using namespace Gaudi;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MuID2BodyPlot );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuID2BodyPlot::MuID2BodyPlot( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator ) {

  // Source of track to ID
  declareProperty("TrackLocation",
                  m_TracksPath = LHCb::TrackLocation::Default);

  // Destination of MuonPID
  declareProperty("MuonTrackLocation",
                  m_MuonTracksPath = LHCb::TrackLocation::Muon);

  // Mass Window
  declareProperty( "MassMean", m_MassMean = 1115.68);
  declareProperty( "MassWindow", m_MassWin = 100.0);
  declareProperty( "JpsiAnalysis", m_JPAna = 0);
  declareProperty( "LambdaAnalysis", m_LMAna = 0);

  // Pre-selection momentum
  declareProperty( "PreSelMomentum", m_PreSelMomentum = 3000.0);

  // Different depths of stations considered in different momentum ranges
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
MuID2BodyPlot::~MuID2BodyPlot() {};

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode MuID2BodyPlot::initialize() {

  // Sets up various tools and services
  const StatusCode sc = DVAlgorithm::initialize();
  if ( sc.isFailure() ) { return sc; }

  debug()   << " MuID2BodyPlot v5r2 " << endmsg;

  debug()  << "==> Initialise" << endreq;
  debug()  << "Input tracks in: " << m_TracksPath << endreq;

  m_NStation = 0;
  m_NRegion = 0;
  MuonBasicGeometry basegeometry( detSvc(),msgSvc());
  m_NStation= basegeometry.getStations();
  m_NRegion = basegeometry.getRegions();
  int i=0;
  while(i<m_NStation){
    m_stationNames.push_back(basegeometry.getStationName(i));
    // debug()   <<" station "<<i<<" "<<m_stationNames[i]<<endreq;
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
        error() << "Muon Chamber Pad Size could not be retrieved !!!" <<  endreq;
        return StatusCode::FAILURE;
      }
    }
  }


  if( m_MomentumCuts.empty() ||
      m_xfoiParam1.size() != (unsigned)m_NStation*m_NRegion ||
      m_xfoiParam2.size() != (unsigned)m_NStation*m_NRegion ||
      m_xfoiParam3.size() != (unsigned)m_NStation*m_NRegion ||
      m_yfoiParam1.size() != (unsigned)m_NStation*m_NRegion ||
      m_yfoiParam2.size() != (unsigned)m_NStation*m_NRegion ||
      m_yfoiParam3.size() != (unsigned)m_NStation*m_NRegion
      ){
    err() << "OPTIONS initialising MuID2BodyPlot are missing"
          << " or wrong size for " << m_NStation << " stations and "
          << m_NRegion << " regions"
          << endreq;
    return StatusCode::FAILURE;
  }

  if( m_MomentumCuts.size() != 2 ){
    err()
      << "OPTIONS are wrong:"
      << " size of MomentumCuts vector is not correct"
      << endreq;
    return StatusCode::FAILURE;
  }

  debug()  << " Momentum bins are (MeV/c) " <<endreq;
  debug()  << " PreSelMomentum = "<<  m_PreSelMomentum << endreq;

  std::vector<double>::const_iterator iMom;
  for(iMom = m_MomentumCuts.begin() ; iMom != m_MomentumCuts.end() ; iMom++){
    debug()  << "Mom. cuts = " << *iMom << endreq ;
  }

  if( m_distPion.size() != 9 || m_distMuon.size() != 9){
    err() << "OPTIONS are wrong:"
          <<" size of m_distPion or m_distMuon vector is not correct"
          << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MuID2BodyPlot::execute() {

  debug()  << "==> Execute" << endreq;

  bool my_IsMu(kFALSE), my_IsNonMu(kFALSE), my_IsDecay(kFALSE); 
  bool algo_IsMu = kFALSE; bool isKs = kFALSE;
  bool matchInp = kFALSE;
  int extr_fail = 0; int ntracks(0);
  m_extrapolated = kFALSE;
  m_prere = -1;
  int run = -1;
  int ev = -1;
  double dllMu(-999),dllPi(-999);

  StatusCode sc;

  LHCb::ODIN* odin(0);

  if( exist<ODIN>( LHCb::ODINLocation::Default ) ){
    odin = get<ODIN>( LHCb::ODINLocation::Default );
    run = odin->runNumber();
    ev = odin->eventNumber();
  } else {
    Error("Can't get LHCb::ODINLocation::Default (" +
	  LHCb::ODINLocation::Default + ")" );
  }

  m_coordPos.clear();
  m_coordPos.resize(m_NStation * m_NRegion);
  m_muonMap.clear();
  m_MuProb = 0;
  m_LklhMu = -10000; m_LklhPi = -10000;

  // get the MuonCoords for each station in turn
  LHCb::MuonCoords* coords = get<LHCb::MuonCoords>(LHCb::MuonCoordLocation::MuonCoords);
  if ( coords==0 ) {
    err() << " Cannot retrieve MuonCoords " << endreq;
    return StatusCode::FAILURE;
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
      warning() << "Failed to get x,y,z of tile " << tile << endreq;
      continue;
    }

    m_coordPos[station*m_NRegion+region].
      push_back(coordExtent_(x,dx,y,dy,*iCoord));

  }

  LHCb::Tracks* trTracks = get<LHCb::Tracks>(m_TracksPath);
  if ( trTracks==0 ){
    err() << " Cannot retrieve Tracks " << endreq;
    return StatusCode::FAILURE;
  }
  debug()  << "Number of input tracks for MuID2BodyPlot: " << trTracks->size() << endreq;

  double dau_pplus(0.), dau_pminus(0.); double pl1(0.),pl2(0.),pl(0.); int posPion(-1);
  bool hitInFoi(kFALSE);
  double mom,px,py,pz,en;
  std::vector<const LHCb::Track*> savedTr;
  std::vector<double> plVec;
  std::vector<int> idVec;
  //Loop on matched tracks
  const Particle::ConstVector& selParts = desktop()->particles();
  bool is_signal = kFALSE;
  Gaudi::LorentzVector *plusTr,*minusTr,ksTr; double PMass;
  plusTr = new Gaudi::LorentzVector(0,0,0,0);
  minusTr = new Gaudi::LorentzVector(0,0,0,0);
  for (Particle::ConstVector::const_iterator iP = selParts.begin(); iP != selParts.end(); ++iP) {
    //Main particle daughters
    PMass = (*iP)->measuredMass();
    if(fabs(PMass-m_MassMean)<m_MassWin) is_signal = kTRUE;

    Particle::ConstVector dau = (*iP)->daughtersVector();

    pl1 = pl2 = 0; pl = -999;
    posPion = -1;
    hitInFoi = kFALSE;
    if(is_signal) {
      for (Particle::ConstVector::const_iterator iD = dau.begin(); iD != dau.end(); ++iD) {
	//      const LHCb::ParticleProperty* pp = particleProperty( (*iD)->particleID() );
	//      if ( !pp ) return StatusCode::SUCCESS;
	const LHCb::ProtoParticle* protop = (*iD)->proto();
	if(protop){
	  const LHCb::Track* track = protop->track();
	  savedTr.push_back(track);
	  
	  if(m_JPAna) {
	    if((*iD)->charge()<0) {
	      //Extrapolate the track that is not belonging to the already IDed mu
	      sc = trackExtrapolate(track);
	      
	      sc = selectHitInFOI();
	      if ( ! sc.isFailure() ){
		info()<<"Found the hit in station 2"<<endreq;
		hitInFoi = kTRUE;

		plot1D (PMass, "selected/InvariantMass","Selected candidates",m_MassMean-m_MassWin, m_MassMean+m_MassWin, 100 );

	      }
	    }
	  }

	  if(m_LMAna) {
	    //KS veto
	    mom = (*iD)->momentum().P();
	    px = (*iD)->momentum().Px();
	    py = (*iD)->momentum().Py();
	    pz = (*iD)->momentum().Pz();

	    en = sqrt(pow(139.57071,2)+pow(mom,2));

	    //Implemented ID selection based on Armenteros Podolanski
	    double cosTr = ((*iP)->momentum().Px()*track->momentum().X()+
			    (*iP)->momentum().Py()*track->momentum().Y()+
			    (*iP)->momentum().Pz()*track->momentum().Z())/((*iP)->momentum().P()*track->p());
	    double plTr = track->p() * cosTr;
	    if((*iD)->charge()>0) {
	      plusTr = new Gaudi::LorentzVector(px,py,pz,en);
	      dau_pminus = track->p();
	      pl1 = plTr;
	    } else {
	      minusTr = new Gaudi::LorentzVector(px,py,pz,en);
	      dau_pplus = track->p();
	      pl2 = plTr;
	    }
	  }
	}
      }//Loop on daughters
      
      //Vectors for further analysis here!
      if(m_LMAna) {
	//KS veto
	ksTr = *plusTr + *minusTr;
	plot1D (ksTr.M(), "selected/KsMass","Selected candidates",400., 600., 200 );
	if(fabs(ksTr.M()-496)<50) {
	  isKs = kTRUE;
	} else {
	  plot1D (PMass, "selected/InvariantMass","Selected candidates",m_MassMean-m_MassWin, m_MassMean+m_MassWin, 100 );
	}
      }
      pl = -999; posPion = -1;
      if(pl1+pl2) {
	pl = (pl1-pl2)/(pl1+pl2);
	if(pl<0) {
	  posPion = 1;
	} else {
	  posPion = 0;
	}
	plVec.push_back(pl);
	idVec.push_back(posPion);
      }
    } //Only for candidates inside the mass window
  }

  info()<<"Saved Track "<<savedTr.size()<<endreq;

  LHCb::Tracks::const_iterator iTrack;
  int trIsPion = -1; 

  //So far discarded multiple candidates
  if(savedTr.size() == 2) {
    
    for( iTrack = trTracks->begin() ; iTrack != trTracks->end() ; iTrack++){
      
      //Matching tracks with reconstructed ones.
      matchInp = kFALSE;
      for(int idxTr = 0; idxTr<(int)savedTr.size(); idxTr++){
	const LHCb::Track * trMatch = savedTr.at(idxTr);
	if(trMatch->key() == (*iTrack)->key()) {
	  matchInp = kTRUE;
	}
      }// End of Loop on matched tracks
      

      // in the clone killed output we want only
      // unique && (matched || forward || downstream)
      my_IsMu = false; my_IsNonMu = false;  my_IsDecay = false; 
      m_dist = -999.; m_region = -1;
      m_LklhMu = -10000.;
      m_LklhPi = -10000.;
      trIsPion = -1;

      if(matchInp) {

	if(!(*iTrack)->checkFlag(LHCb::Track::Clone)  &&
	   ((*iTrack)->checkType(LHCb::Track::Long) ||
	    (*iTrack)->checkType(LHCb::Track::Downstream))){
	  

	  // do the track extrapolations
	  sc = trackExtrapolate(*iTrack);
	  if ( sc.isFailure() ){
	    m_extrapolated = false;
	    warning() << " trackExtrapolate failed for track " << *iTrack << endreq;
	    extr_fail++;
	  } else {
	    
	    // Do the complete ID, calculating IsMuon and likelihoods
	    LHCb::MuonPID * pMuid = new LHCb::MuonPID;
	    pMuid->setIDTrack(*iTrack);
	    sc = doID(pMuid);
	    
	    algo_IsMu = pMuid->IsMuon();
	    
	    // calculate the distance from the hit to the extrapolated position
	    m_dist = calc_closestDist(pMuid,m_MomentumPre,closest_region);
	    
	    ntracks++;
	    
	    if(m_JPAna) {
	      int achTr = (*iTrack)->charge();
	      if(hitInFoi && achTr<0) { my_IsMu = kTRUE; }
	    }
	    if(m_LMAna) {
	      int chTr = (*iTrack)->charge();
	      if(chTr>0) {
		if(posPion>0) {
		  trIsPion = 1;
		} else if(posPion == 0) {
		  trIsPion = 0;
		}
	      } else {
		if(posPion == 0) {
		  trIsPion = 1;
		} else if(posPion > 0) {
		  trIsPion = 0;
		}
	      }
	      
	      //Only for protons from Lambda
	      if(!isKs) {
		if(trIsPion == 0) {
		  my_IsNonMu = kTRUE;
		} else if(trIsPion == 1) {
		  my_IsDecay = kTRUE;
		}
	      }
	    }

	  } //failed to extrapolate tracks

	}//Good Tracks
	
	
	info()<<"Region:: "<<(*iTrack)->p()/Gaudi::Units::GeV<<" "<<m_region<<"  "<<my_IsDecay<<" "<<my_IsNonMu<<" "<<algo_IsMu<<" "<<m_dist<<endreq;

	if(m_LklhMu<=0) dllMu = -999;
	else dllMu = log(m_LklhMu);
	if(m_LklhPi<=0) dllPi = -999;
	else dllPi = log(m_LklhPi);
	
	plot1D ( (*iTrack)->p()/Gaudi::Units::GeV, "matched/Momentum",
		 "P matched Sample",
		 0., 100., 100 );
	plot1D ( (*iTrack)->pt()/Gaudi::Units::GeV, "matched/TrMomentum",
		 "Pt matched Sample",
		 0., 20., 100 );
	plot1D ( dllMu, "matched/SigLikelihood",
		 "Mu lik matched Sample",
		 -50., 0., 1000 );
	plot1D ( dllPi, "matched/BkgLikelihood",
		 "Bg lik matched Sample",
		 -50., 0., 1000 );
	if(m_dist) {
	  plot1D ( m_dist, "matched/Distance","Distance matched Sample",0., 500., 100 );
	  plot1D ( m_dist, "matched/ZmDistance","Distance matched Sample",0., 20., 100 );
	  if(m_region == 0) {
	    plot1D ( m_dist, "matched/R1Distance","Distance matched Sample",0., 500., 100 );
	    plot1D ( m_dist, "matched/R1ZmDistance","Distance matched Sample",0., 20., 100 );
	  } else if(m_region == 1) {
	    plot1D ( m_dist, "matched/R2Distance","Distance matched Sample",0., 500., 100 );
	    plot1D ( m_dist, "matched/R2ZmDistance","Distance matched Sample",0., 20., 100 );
	  } else if(m_region == 2) {
	    plot1D ( m_dist, "matched/R3Distance","Distance matched Sample",0., 500., 100 );
	    plot1D ( m_dist, "matched/R3ZmDistance","Distance matched Sample",0., 20., 100 );
	  } else if(m_region == 3) {
	    plot1D ( m_dist, "matched/R4Distance","Distance matched Sample",0., 500., 100 );
	    plot1D ( m_dist, "matched/R4ZmDistance","Distance matched Sample",0., 20., 100 );
	  } 
	}
	std::string name;
	if(my_IsMu) { 	  name = "muons";	}

	if(my_IsNonMu) {	  name = "nonmuons";	}

	if(my_IsDecay) {	  name = "decay";	}

	plot1D ( (*iTrack)->p()/Gaudi::Units::GeV, name+"/Momentum",
		 "P Muon Sample",
		 0., 100., 100 );
	plot1D ( (*iTrack)->pt()/Gaudi::Units::GeV, name + "/TrMomentum",
		 "Pt Muon Sample",
		 0., 20., 100 );
	profile1D( (*iTrack)->p()/Gaudi::Units::GeV,
		   algo_IsMu,name + "/MomentumEfficiency","Muon Efficiency vs P",
		   0., 100., 100 );
	profile1D( (*iTrack)->pt()/Gaudi::Units::GeV,
		   algo_IsMu,name + "/TrMomentumEfficiency","Muon Efficiency vs P",
		   0., 20., 100 );
	plot1D ( dllMu, name + "/SigLikelihood",
		 "Mu lik Non muons Sample",
		 -50., 0., 1000 );
	plot1D ( dllPi, name + "/BkgLikelihood",
		 "Bg lik non muons Sample",
		 -50., 0., 1000 );
	if(m_dist) {
	  plot1D ( m_dist, name + "/Distance","Distance decay Sample",0., 500., 100 );
	  plot1D ( m_dist, name + "/ZmDistance","Distance decay Sample",0., 20., 100 );
	  if(m_region == 0) {
	    plot1D ( m_dist, name + "/R1Distance","Distance decay Sample",0., 500., 100 );
	    plot1D ( m_dist, name + "/R1ZmDistance","Distance decay Sample",0., 20., 100 );
	  } else if(m_region == 1) {
	    plot1D ( m_dist, name + "/R2Distance","Distance decay Sample",0., 500., 100 );
	    plot1D ( m_dist, name + "/R2ZmDistance","Distance decay Sample",0., 20., 100 );
	  } else if(m_region == 2) {
	    plot1D ( m_dist, name + "/R3Distance","Distance decay Sample",0., 500., 100 );
	    plot1D ( m_dist, name + "/R3ZmDistance","Distance decay Sample",0., 20., 100 );
	  } else if(m_region == 3) {
	    plot1D ( m_dist, name + "/R4Distance","Distance decay Sample",0., 500., 100 );
	    plot1D ( m_dist, name + "/R4ZmDistance","Distance decay Sample",0., 20., 100 );
	  } 
	}

      } // long or downstream tracks and NO Clones
    }  // loop over tracks
  }

  setFilterPassed(true);  // Mandatory. Set to true if event is accepted. 
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MuID2BodyPlot::finalize() {

  debug()  << "==> Finalize" << endreq;
  // Execute base class method
  return DVAlgorithm::finalize();
}


//=============================================================================
// Do the identification
//=============================================================================
StatusCode MuID2BodyPlot::doID(LHCb::MuonPID *pMuid){

  // Initializes data members
  pMuid->setIsMuon(0);
  pMuid->setNShared(0);
  pMuid->setPreSelMomentum(0);
  pMuid->setInAcceptance(0);
  pMuid->setMuonLLMu(-10000.);
  pMuid->setMuonLLBg(-10000.);

  // First do a preselection:
  // track is in acceptance? Track has minimum momentum?
  bool passed;
  StatusCode sc = preSelection( pMuid );
  passed = m_passed;
  if(sc.isFailure()){
    warning() <<" preSelection failed to MuonPID object" << pMuid << endreq;
    return sc;
  }

  // OK: track failed preselection say so and return
  if(!passed){
    debug() << " Track failed preselection " << endreq;
    return StatusCode::SUCCESS;
  }

  // find the coordinates in the fields of interest
  sc = setCoords( pMuid );
  if(sc.isFailure()){
    warning() <<" setCoords failed to MuonPID object" << pMuid << endreq;
    return sc;
  }

  // apply ID: depends on the track momentum

  // find the momentum bin we are in
  // it can be 0, 1 or 2
  int momentumBin = 0;

  // m_PreSelMomentum = 3000 MeV
  if (m_MomentumPre> m_PreSelMomentum && m_MomentumPre < m_MomentumCuts[0]) {momentumBin = 0;}
  if (m_MomentumPre >= m_MomentumCuts[0] && m_MomentumPre < m_MomentumCuts[1]) {momentumBin = 1;}
  if (m_MomentumPre >= m_MomentumCuts[1]) {momentumBin = 2;}

  // now: implement original algorithm:
  // bin 0 M1.and.M2.and.M3
  // bin 1 M1.and.M2.and.M3.and.(M4.or.M5)
  // bin 2 M1.and.M2.and.M3.and.M4.and.M5
  bool isMuon=false;
  if (momentumBin == 0) {
    if (m_occupancy[1]>0 && m_occupancy[2]>0) {isMuon = true;}
  }
  if (momentumBin == 1) {
    if (m_occupancy[1]>0 && m_occupancy[2]>0 && (m_occupancy[3]>0 ||
                                                 m_occupancy[4]>0)) {isMuon = true;}
  }

  if (momentumBin == 2) {
    if (m_occupancy[1]>0 && m_occupancy[2]>0 && m_occupancy[3]>0 &&
        m_occupancy[4]>0) {isMuon = true;}
  }

  pMuid->setIsMuon(isMuon);

  // calculate MuProb
  m_MuProb = calcMuProb( pMuid );
  debug()  << "MuProb= " << m_MuProb <<endreq;

  // calculate Muon DLL
  sc = calcMuonLL( pMuid );
  if(sc.isFailure()){
    warning() << " calcMuonLL failed to MuonPID object " << pMuid << endreq;
  }

  return StatusCode::SUCCESS;
}


//=============================================================================
// Calculate MuProb
//=============================================================================
// if found a muon make a probability from the DxDz matching
float MuID2BodyPlot::calcMuProb(LHCb::MuonPID * pMuid){

  if(pMuid->IsMuon()){
    // slope calculated in M2 and M3
    m_xMatchStation = 1;
    // find slope difference between track and Coords in M2-M3
    double coordSlopeX = ((m_CoordX[m_xMatchStation] -
                           m_CoordX[m_xMatchStation+1])/
                          (m_stationZ[m_xMatchStation] -
                           m_stationZ[m_xMatchStation+1]));
    double dSlopeX = fabs( m_trackSlopeX - coordSlopeX );

    // formula to make this a probability is
    // dSlopeX < 0.005 = 1.0
    // 0.005 < dSlopeX < 0.085 = 1. - ( (dSlopeX-0.005) / 0.08 )
    // 0.085 < dSlopeX = 0.
    if( dSlopeX < 0.005 ) {
      return 1.0;
    }else if( dSlopeX > 0.085 ){
      return 0.0;
    } else {
      return (1.0 - ( (dSlopeX-0.005) / 0.08 ) );
    }
  }else{
    // not passed selection as a muon so probability is zero
    return 0.0;
  }
}

//=============================================================================
// Calculate the muon DLL
//=============================================================================
StatusCode MuID2BodyPlot::calcMuonLL(LHCb::MuonPID * muonid){

  if ( !muonid->IsMuon() ) {
    return StatusCode::SUCCESS;
  }

  const LHCb::Track* pTrack = muonid->idTrack();
  // do the track extrapolations
  if (m_extrapolated){
    warning() << "trackExtrapolate failed for track" << pTrack << endreq;
    m_LklhMu = -10000.;
    m_LklhPi = -10000.;
    m_OthDist = 0;
    return StatusCode::FAILURE;
  }

  int nhits=0;
  double dist = 0.;
  std::vector<LHCb::MuonCoord*> & mcoord = m_muonMap[muonid];
  std::vector<LHCb::MuonCoord*>::const_iterator iCoord;
  for ( iCoord = mcoord.begin() ; iCoord != mcoord.end() ; iCoord++ ){

    double x,dx,y,dy,z,dz;
    LHCb::MuonTileID tile=(*iCoord)->key();
    StatusCode sc =
      m_mudet->Tile2XYZ(tile,x,dx,y,dy,z,dz);
    if (sc.isFailure()){
      warning() << "Failed to get x,y,z of tile " << tile << endreq;
      continue;
    }

    int station = (*iCoord)->key().station();
    if (station > 0 ) {
      dist = dist + pow(((x - m_trackX[station])/dx),2) +
        pow(((y - m_trackY[station])/dy),2) ;
      nhits++;
    }
  }
  //  info()<<"Hit dist:: "<<dist<<" "<<nhits<<endreq;
  if(nhits) { dist = dist / nhits; }
  else { dist = 0; }

  m_OthDist = dist;   

  m_LklhMu = m_distMuon[0] * (TMath::Landau (dist,m_distMuon[1],m_distMuon[2])) + 
    m_distMuon[3] * (TMath::Landau (dist,m_distMuon[4],m_distMuon[5])) +
    m_distMuon[6] * (TMath::Landau (dist,m_distMuon[7],m_distMuon[8]));
  m_LklhPi = m_distPion[0] * (TMath::Landau (dist,m_distPion[1],m_distPion[2])) + 
    m_distPion[3] * (TMath::Landau (dist,m_distPion[4],m_distPion[5])) +
    m_distPion[6] * (TMath::Landau (dist,m_distPion[7],m_distPion[8]));

  return StatusCode::SUCCESS;

}

//=============================================================================
// apply the preselection
//=============================================================================
StatusCode MuID2BodyPlot::preSelection(LHCb::MuonPID * pMuid){

  m_passed = true;
  m_prere = 0;
  // compare momentum and position to acceptance
  // use the first state on track for the preSel
  if (m_MomentumPre < m_PreSelMomentum ){ 
    // failed preselection momentum cut
    pMuid->setPreSelMomentum(0);
    m_passed = false;
    m_prere = 1;
  }else{
    pMuid->setPreSelMomentum(1);
  }
  pMuid->setInAcceptance(1);
  // in first and last station acceptance
  debug()  << "trackX0 = " << m_trackX[0] << endreq;
  debug()  << "trackX4 = " << m_trackX[4] << endreq;
  debug()  << "trackY0 = " << m_trackY[0] << endreq;
  debug()  << "trackY4 = " << m_trackY[4] << endreq;
  if(  ! (fabs(m_trackX[0]) <  m_regionOuterX[0] &&
          fabs(m_trackY[0]) <  m_regionOuterY[0] )  ||
       ! (fabs(m_trackX[m_NStation-1]) <
          m_regionOuterX[m_NStation-1] &&
          fabs(m_trackY[m_NStation-1]) <
          m_regionOuterY[m_NStation-1] )
       ) {
    // outside M1 - M5 region
    pMuid->setInAcceptance(0);
    m_passed = false;
    m_prere = 2;
  }

  // Inner acceptance
  if(   (fabs(m_trackX[0]) <  m_regionInnerX[0] &&
         fabs(m_trackY[0]) <  m_regionInnerY[0] )  ||
        (fabs(m_trackX[m_NStation-1]) <
         m_regionInnerX[m_NStation-1] &&
         fabs(m_trackY[m_NStation-1]) <
         m_regionInnerY[m_NStation-1] ) ) {

    // inside M1 - M5 chamber hole
    pMuid->setInAcceptance(0);
    m_passed = false;
    m_prere = 3;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// get the coords
//=============================================================================
StatusCode MuID2BodyPlot::setCoords(LHCb::MuonPID *pMuid){

  int station;
  for(station = 0 ; station < m_NStation ; station++){
    int region;
    for(region = 0 ; region < m_NRegion ; region++){

      if( !m_coordPos[station*m_NRegion + region].empty() ){

        std::vector<coordExtent_>::const_iterator itPos;
        for(itPos = m_coordPos[station*m_NRegion + region].begin();
            itPos != m_coordPos[station*m_NRegion + region].end();
            itPos++){

          double x = itPos->m_x;
          double dx = itPos->m_dx;
          double y = itPos->m_y;
          double dy = itPos->m_dy;


	  // not optimal this should be called only once per station, region	 
          double foiXDim = m_foifactor*foiX( station, region, m_MomentumPre, dx);
          double foiYDim = m_foifactor*foiY( station, region, m_MomentumPre, dy);      
	  
          // check if the hit is in the window
          if(  ( fabs( x - m_trackX[station] ) < foiXDim ) &&
               ( fabs( y - m_trackY[station] ) < foiYDim )  ) {

            debug()  << "FOIfactor : " << m_foifactor << endreq;

            debug()  << "ratioX = " << fabs( x - m_trackX[station])
                     << "ratioY = " << fabs( y - m_trackY[station]) << " foiXDim = "
                     << foiXDim <<" foiYDim = " << foiYDim <<endreq;
            debug()  << "padX = " << dx << " padY = " << dy << endreq;

            // it is in the window
            // add the hit to the MuonPID
            m_muonMap[pMuid].push_back(itPos->m_pCoord);
            m_occupancy[station]++;

            // also need track angle in X in M2-M3 for angle matching
            if(1 == m_occupancy[station]) {
              // this is the first coord found
              m_CoordX[station] = x;
            }else{
              // get best match X
              if( fabs(x - m_trackX[station]) <
                  fabs(m_CoordX[station] - m_trackX[station]) ){
                // this Coord is a better match
                m_CoordX[station] = x;
              }
            }
          }
        }// itPos
      }
    } //region
  } //station
  return StatusCode::SUCCESS;
}

//=============================================================================
// Track extrapolation
//=============================================================================
StatusCode MuID2BodyPlot::trackExtrapolate(const LHCb::Track *pTrack){

  resetTrackLocals();

  // get first state for the preSel cut
  const LHCb::State * state1 = &(pTrack->firstState());
  // get state closest to M1 for extrapolation
  const LHCb::State * state = &(pTrack->closestState(m_stationZ[0]));

  if(!state1){
    err() << " Failed to get 1st state from track " << endreq;
    return StatusCode::FAILURE;
  }
  if(!state){
    err() << " Failed to get state from track " << endreq;
    return StatusCode::FAILURE;
  }

  // get the momentum (MeV/c)
  m_Momentum = state->p();
  m_MomentumPre = state1->p();

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
double MuID2BodyPlot::foiX(const int &station, const int &region, const double &p,
                    const double &dx){
  return ( m_xfoiParam1[ station * m_NRegion + region ] +
           m_xfoiParam2[ station * m_NRegion + region ]*
           exp(-m_xfoiParam3[ station * m_NRegion + region ]*p/Gaudi::Units::GeV ) )*dx;

  //in the future optimize this checking that 2*dx =m_padSizeX[station * m_NRegion + region]
  //then eliminates dx from function
}

//=============================================================================
// return the FOI in y in a station and region for momentum (in MeV/c)
//=============================================================================
double MuID2BodyPlot::foiY(const int &station, const int &region, const double &p,
                    const double &dy){
  return ( m_yfoiParam1[ station * m_NRegion + region ] +
           m_yfoiParam2[ station * m_NRegion + region ]*
           exp(-m_yfoiParam3[ station * m_NRegion + region ]*p/Gaudi::Units::GeV ) )*dy;
}

//=============================================================================
// reset vectors
//=============================================================================
void MuID2BodyPlot::resetTrackLocals(){
  m_Momentum = -1.;
  m_MomentumPre = -1.;
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
StatusCode MuID2BodyPlot::selectHitInFOI(){

  int station(1);
  int region; bool foundHit = false;
  for(region = 0 ; region < m_NRegion ; region++){
    
    if( !m_coordPos[station*m_NRegion + region].empty() ){
      
      std::vector<coordExtent_>::const_iterator itPos;
      for(itPos = m_coordPos[station*m_NRegion + region].begin();
	  itPos != m_coordPos[station*m_NRegion + region].end();
	  itPos++){
	
	double x = itPos->m_x;
	double dx = itPos->m_dx;
	double y = itPos->m_y;
	double dy = itPos->m_dy;
	
	
	// not optimal this should be called only once per station, region	 
	double foiXDim = m_foifactor*foiX( station, region, m_MomentumPre, dx);
	double foiYDim = m_foifactor*foiY( station, region, m_MomentumPre, dy);      
	
	// check if the hit is in the window (2* dimension!)
	if(  ( fabs( x - m_trackX[station] ) < 2*foiXDim ) &&
	     ( fabs( y - m_trackY[station] ) < 2*foiYDim )  ) {
	  foundHit = true;
	}
      }// itPos
    }
  } //region
  StatusCode sc = StatusCode::FAILURE;
  if(foundHit) sc = StatusCode::SUCCESS;
  return sc;
}

StatusCode MuID2BodyPlot::get_closest(LHCb::MuonPID *pMuid, double *closest_x, double *closest_y, double *closest_region){
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

  std::vector<LHCb::MuonCoord*> & mcoord = m_muonMap[pMuid];
  std::vector<LHCb::MuonCoord*>::const_iterator iCoord;
  for ( iCoord = mcoord.begin() ; iCoord != mcoord.end() ; iCoord++ ){
    
    double x,dx,y,dy,z,dz;
    LHCb::MuonTileID tile=(*iCoord)->key();
    StatusCode sc =
      m_mudet->Tile2XYZ(tile,x,dx,y,dy,z,dz);
    if (sc.isFailure()){
      warning() << "Failed to get x,y,z of tile " << tile << endreq;
      continue;
    }
    
    int station = (*iCoord)->key().station();
    int region = (*iCoord)->key().region();
    foiXDim = m_foifactor*foiX( station, region, m_MomentumPre, dx);
    foiYDim = m_foifactor*foiY( station, region, m_MomentumPre, dy);

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

        }//Fdist
      }//foi
    }//station
  }//coord

  return StatusCode::SUCCESS;
}

//=====================================================================
double MuID2BodyPlot::calc_closestDist(LHCb::MuonPID *pMuid, const double& p, double *closest_region){
//=====================================================================
// comment: Returns the squared distance calculated with closest hit
// authors: G. Lanfranchi & S. Furcas, 
// date:    10/5/09
//=====================================================================
  
  double closest_dist=0;
  
  StatusCode sc = get_closest(pMuid,closest_x,closest_y,closest_region);
  
  if ( sc.isFailure() ){
    warning() << " Closest_hit failed " << endreq;
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



