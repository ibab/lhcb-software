// $Id: MatchPatVeloSpaceL0.cpp,v 1.3 2007-01-11 16:11:46 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// from Event 
#include "Event/Track.h"

// local
#include "MatchPatVeloSpaceL0.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MatchPatVeloSpaceL0
//   Implementation of matching PatVeloSpace with L0MuonCandidates
//   Based on the implementation by  Mariusz Witek
//   This version uses the new track event model
//
// 2006-02-14 : Sandra Amato
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( MatchPatVeloSpaceL0 );

using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MatchPatVeloSpaceL0::MatchPatVeloSpaceL0( const std::string& name,
					  ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{

  declareProperty("OutputMuonTracksName"   , 
                  m_outputMuonTracksName = "Hlt/Tracks/VeloSpaceMuon");
  declareProperty( "ptkickConstant",  m_ptkickConstant  =  1263.0      );
  declareProperty( "M1EcorParams0",   m_m1EcorParams0   =  0.220       );
  declareProperty( "M1EcorParams1",   m_m1EcorParams1   = -258.0       );
  declareProperty( "M1EcorParams2",   m_m1EcorParams2   = -0.00000204  );
  declareProperty( "M1EresParams0",   m_m1EresParams0   =  0.227       );
  declareProperty( "M1EresParams1",   m_m1EresParams1   =  0.000000793 );
  declareProperty( "NoM1EcorParams0", m_nom1EcorParams0 = -0.0427804   );
  declareProperty( "NoM1EcorParams1", m_nom1EcorParams1 = -1472.48     );
  declareProperty( "NoM1EcorParams2", m_nom1EcorParams2 =  0.00000248794  );
  declareProperty( "NoM1EresParams0", m_nom1EresParams0 =  0.25        );
  declareProperty( "NoM1EresParams1", m_nom1EresParams1 =  0.000008       );
  declareProperty( "maxChi2MatchMuon3d", m_maxChi2MatchMuon3d  =  9.0 );
  declareProperty("CheckMC", m_checkMC= true);
		  
}
//=============================================================================
// Destructor
//=============================================================================
MatchPatVeloSpaceL0::~MatchPatVeloSpaceL0() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode MatchPatVeloSpaceL0::initialize() {
  
  
  debug() << "==> Initialize" << endmsg;
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm


  m_iPosTool=tool<IMuonPosTool>( "MuonPosTool" );
  m_outputMuonTracks = 
    m_patDataStore->createTrackContainer( m_outputMuonTracksName, 20 );
  m_states            = m_patDataStore->states();
  
  
  m_sqrt12 = sqrt(12.);
  
  m_myState = new State();

 initializeHisto(m_histoChi,"Chi2",0.,100.,100);
 //HltHisto h_patOutTrack; 
 initializeHisto(h_patOutTrack,"Pat Output Tracks",0.,100,100);

  
  return StatusCode::SUCCESS;
};
//=============================================================================
bool MatchPatVeloSpaceL0::acceptTrack(const Track& track) 
{
  bool ok = !(track.checkFlag( Track::Invalid) );
  ok = ok && (!(track.checkFlag( Track::Backward) ));  
  //  verbose() << " accept track " << ok << endreq;
  return ok;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MatchPatVeloSpaceL0::execute() {
  debug() << "==> Execute" << endreq;

  bool ok  = HltAlgorithm::beginExecute();
  ok=false;
//debug()<<" dopo "<<endreq;  
  setFilterPassed(false);  
  m_inputL0Muon=get<L0MuonCandidates>(L0MuonCandidateLocation::Default);

  // loop over 3d tracks
  for ( std::vector<Track*>::const_iterator itT = m_inputTracks->begin();
        m_inputTracks->end() != itT; itT++ ) {
    
    Track* pTr3d = (*itT);
    pTr3d->setFlag(Track::PIDSelected,false);
    if (!acceptTrack(*pTr3d)) continue;
    
    // skip tracks which were not selected as a 2d candidate
    

  //  if( !pTr3d->checkFlag(Track::PIDSelected)) continue;
    
    MuonTileID tileIDMatched;
    float chi2match=999;
    
    StatusCode sc = match3dMuon(pTr3d, tileIDMatched, chi2match);  
    
    if(StatusCode::SUCCESS == sc) {
      pTr3d->setFlag(Track::PIDSelected,true);  
      
      Track* outTr = m_outputMuonTracks->newEntry();
      outTr->copy(*pTr3d);
      outTr->addToStates( *m_myState);

      outTr->firstState().setQOverP(m_myState->qOverP());
      outTr->setFlag(Track::PIDSelected,true);
      outTr->setHistory( Track::TrackMatching );
      outTr->addToAncestors( pTr3d );

      outTr->addToLhcbIDs(tileIDMatched);      

      setFilterPassed(true);  
      ok=true;
      fillHisto(m_histoChi, chi2match,1.);
      m_outputTracks->push_back(outTr);
    }
  }
  fillHisto(h_patOutTrack,m_outputMuonTracks->size(),1.0);
  

  if(!ok)return StatusCode::SUCCESS;
  HltAlgorithm::endExecute();  
  //debug()<<" end space muon l0 "<<m_outputMuonTracks->size()<<endreq;
  return StatusCode::SUCCESS;
};


//=============================================================================
//  match3dMuon method
//=============================================================================
StatusCode MatchPatVeloSpaceL0::match3dMuon(Track* pTr3d, 
					    MuonTileID & tileIDMatched, float& chi2match) {
  
 double chi2BestMatchMuon = 999.;
  double momentumMuon      = 999.;
  double chargeMuon        = 999.;
  
  // change firstState into exit VELO state
  double trackDxDz = pTr3d->firstState().tx();
  double trackDyDz = pTr3d->firstState().ty();

  double muonX,muonY,muonZ,muonDX,muonDY;
  muonX=0;
  muonY=0;
  muonZ=0;
  muonDX=0;
  muonDY=0;
 
  //debug()<<" track slopes "<<trackDxDz <<" "<<trackDyDz <<endreq;  
  std::vector<L0MuonCandidate*>::const_iterator itL0Mu;
  // Loop over L0 muons:
  
  for (itL0Mu=m_inputL0Muon->begin();itL0Mu!=m_inputL0Muon->end();itL0Mu++){
    
    L0MuonCandidate* itMuon = *itL0Mu;
    
    double theta  = itMuon->theta();
    double phi    = itMuon->phi();
    MuonTileID tileM2=itMuon->pad(1);
    double xM2,yM2,zM2;
    double dx,dy,dz;
    
    m_iPosTool->calcTilePos(tileM2,xM2,dx,yM2,dy,zM2,dz);
      

    double xtile  = xM2;
    double ytile  = yM2;
    double ztile  = zM2;
    double dxtile = dx;
    double dytile = dy;
    //double dztile = itMuon->dztile();
    int idtile = itMuon->pad(2);
  //  MuonTileID hh=idtile;
//info()<<" coordinate "<<xtile<<" "<<ytile<<" "<<ztile<<endreq;
//if(xtile==0)info()<<hh.station()<<" "<<hh.region()<<endreq;
 //   if(!hh.isValid())info()<<" che diavolo succede"<<endreq;

    
    // Get energy and position of L0 muon candidate:
    double x      = xtile ; // mm -> cm
    double y      = ytile ; // mm -> cm
    double z      = ztile ; // mm -> cm
    double ex     = 2.*((float)dxtile)/m_sqrt12 ; // mm -> cm 
    double ey     = 2.*((float)dytile)/m_sqrt12 ; // mm -> cm
    double et     = itMuon->pt();
    double e      = fabs(et/sin(theta));
    // NT 28/08/02 Implement energy correction.
    double de   = 0.3;
    double ecor = e  ;
    ecor   = eCorMuon(0, e, de);

    // Absolute energy uncertainty (note the strange sqrt term!)
    // DeltaE(muon)/E = 9% + 10% * sqrt(E)  gives flat pull of 1.
    //double de     = e*(sqrt( 0.09*0.09 + 0.10*0.10*e  ));

    // NB Charge needs a MINUS sign! Contacted Andrei Tsaregorodt.
    //double q      = (et == 0) ? 0.0 : -1.*et/fabs(et);  
    // NT 10/07/2002 First guess: solved??
    //double q      = (et == 0) ? 0.0 : 1.*et/fabs(et);  
    // NT 31/07/2002 Charge looks still inversed. Perhaps on purpose??
    double q      = (et == 0) ? 0.0 : -1.*et/fabs(et);  

    // PT kick due to magnet. (Centre of magnet at 5.25 m)
    double xkick  = (z - 5250.0) * m_ptkickConstant/ecor;
    // Uncertainty on PT kick, depending on energy resolution.
    // NT 28/08/02 Use proper formula for xkick uncertainty!
    //double exkick = fabs(  (z - 525.0)*1.24* (1/ecor - 1/(ecor+de)) );
    double exkick = fabs(  (z - 5250.0)*m_ptkickConstant* 
(1/ecor)*(1/ecor))*de;

    // Calculate the slopes and their uncertainties:
    double dxdz   = tan(theta)*cos(phi);
    double edxdz  = sqrt(ex*ex + exkick*exkick)/z;
    double dydz   = tan(theta)*sin(phi);
    double edydz  = ey/z;
    //debug()<<" edydz "<<ey<<" " <<z<<endreq;   
    
    // Calculate the matching chi2:
    double deltaX   = q*(dxdz - trackDxDz)/edxdz;
    double dydzSign = dydz/fabs(dydz);
    double deltaY   = dydzSign*(dydz - trackDyDz)/edydz;
    double chi2Matching = deltaX*deltaX + deltaY*deltaY;

    // Output:
    if (chi2Matching < chi2BestMatchMuon) {
      double xkickVelo    = (1./q)*(x - z*trackDxDz);
      double momentumVelo = (z - 5250.)*m_ptkickConstant/xkickVelo;

      chi2BestMatchMuon = chi2Matching;
      momentumMuon      = fabs(momentumVelo);
      chargeMuon        = ((x-z*trackDxDz)==0)?0.0:
        (x-z*trackDxDz)/fabs(x-z*trackDxDz);
      
      tileIDMatched = idtile;
      muonDX=dxdz;
      muonDY=dydz;
      
      muonX=xtile;
      muonY=ytile;
      muonZ=ztile;

 
      
   }

    // DEBUG:
    if ( msgLevel( MSG::VERBOSE ) ) {
      verbose() << " Found L0 Muon candidate "
                << " Pt: "    << et
                << " E VELO: "<< e
                << " dE: "    << de
                << " q: "     << q
                << " theta: " << theta
                << endreq
                << "      "
                << " X: "     << x
                << " Y: "     << y
                << " xkick: " << xkick
                << " exkick: "<< exkick
                << endreq
                << "      "
                << " dxdz: "  << dxdz << " " << x/z
                << " dydz: "  << dydz << " " << y/z
                << " edxdz: " << edxdz
                << " edydz: " << edydz
                << endreq
                << "       Matching quality: "
                << " Delta X "  << deltaX
                << " Delta Y "  << deltaY
                << " Chi2 "     << chi2Matching
                << " (muon 3d) "
                << endreq;
    }

  } // end loop over L0 muon candidates
                                      
  if(chi2BestMatchMuon < m_maxChi2MatchMuon3d ) {
    // fill the track state. 
    // Temporary first state parameters and 1/p from L0 muon  matching 
    double qp=0.;
    if (0. != momentumMuon) qp = chargeMuon/(momentumMuon);
    //m_myState->reset();
    chi2match=chi2BestMatchMuon ;
    /*    m_myState->setState( pTr3d->firstState().x(), 
                         pTr3d->firstState().y(),
                         pTr3d->firstState().z(),
                         pTr3d->firstState().tx(), 
                         pTr3d->firstState().ty(), 
                         qp );*/
    m_myState->setState( muonX, 
                         muonY,
                         muonZ,
                         muonDX, 
                         muonDY, 
                         qp );
//info()<<" fil my state "<<muonX<<endreq;
    debug()<<" muon l0 "<<chargeMuon<<" "<<momentumMuon<<endreq;
    
    m_myState->setLocation(State::Muon);
    return StatusCode::SUCCESS;
  }
  else {
    return StatusCode::FAILURE;
  }
  
}


//=============================================================================
//  eCorMuon
//=============================================================================
double MatchPatVeloSpaceL0::eCorMuon(const int &istation,
                                 const double &e,
                                 double &de) {
  // v1 NT 28/08/02 p0 + p1*x + p2*x*x correction
  // v2 NT 29/08/02 p0 + p1/x + p2*x   correction
  // ------------------------------------------------
  double ecor = e;
  de          = 0.235;

  if (istation < 0 || istation > 1) {
    err() << " Energy correction not implemented for station: " 
          << istation << " E: " << e 
          << endreq;
      return ecor;
  }

  double emin =   4.0;
  double emax = 200.0;
  
  double paramecor[2][3];
  double resolution[2][2];
  // Parameters for energy correction of M1+M2 muons
  // and the resolution after correction:
  paramecor[0][0]   = m_m1EcorParams0;
  paramecor[0][1]   = m_m1EcorParams1;
  paramecor[0][2]   = m_m1EcorParams2;
  resolution[0][0]  = m_m1EresParams0;
  resolution[0][1]  = m_m1EresParams1;
  // Parameters for energy correction of M2+M3 muons
  // and the resolution after correction:
  paramecor[1][0]   = m_nom1EcorParams0;
  paramecor[1][1]   = m_nom1EcorParams1;
  paramecor[1][2]   = m_nom1EcorParams2;
  resolution[1][0]  = m_nom1EresParams0;
  resolution[1][1]  = m_nom1EresParams1;
  
  double ecorfunction = 0.0;
  if (e < emin){
    ecorfunction = 
      paramecor[istation][0]      + 
      paramecor[istation][1]/emin + 
      paramecor[istation][2]*emin  ;
  }else if (e > emax){
    ecorfunction = 
      paramecor[istation][0]      + 
      paramecor[istation][1]/emax + 
      paramecor[istation][2]*emax  ;
  }else {
    ecorfunction = 
      paramecor[istation][0]   + 
      paramecor[istation][1]/e + 
      paramecor[istation][2]*e  ;
  }
  
  ecor = e*(1.0 - ecorfunction);
  de   = ecor*(resolution[istation][0]+resolution[istation][1]*ecor);

  return ecor;
}




//=============================================================================
//  Finalize
//=============================================================================
StatusCode MatchPatVeloSpaceL0::finalize() {

  if ( 0 != m_myState ) delete m_myState;

  return HltAlgorithm::finalize();
}


//=============================================================================
