// $Id: MatchPatVeloRL0.cpp,v 1.4 2007-06-20 16:12:57 hernando Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "MatchPatVeloRL0.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MatchPatVeloRL0
//
// 2006-02-13 : Sandra Amato based on L1Match2dL0 by Mariusz Witek to 
// adapt to the new track event model
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( MatchPatVeloRL0 );

using namespace LHCb;


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MatchPatVeloRL0::MatchPatVeloRL0( const std::string& name,
			  ISvcLocator* pSvcLocator)
    : HltAlgorithm ( name , pSvcLocator )
{
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
  declareProperty( "maxChi2MatchMuon2d", m_maxChi2MatchMuon2d  =  16.0 );
}
//=============================================================================
// Destructor
//=============================================================================
MatchPatVeloRL0::~MatchPatVeloRL0() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode MatchPatVeloRL0::initialize() {

  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm


  //  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  //if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
  m_iPosTool=tool<IMuonPosTool>( "MuonPosTool" );
  m_sqrt12 = sqrt(12.);
  initializeHisto(m_histoChi,"Chi2",0.,100.,100);
  return StatusCode::SUCCESS;
};
//=============================================================================
bool MatchPatVeloRL0::acceptTrack(const Track& track) 
{
  bool ok = !(track.checkFlag( Track::Invalid ));
  debug() << " accepted track ? " << ok << endreq;
  return ok;
}


//=============================================================================
// Main execution
//=============================================================================
StatusCode MatchPatVeloRL0::execute() {
  debug() << "==> Execute" << endreq;
  bool ok  = true;
  ok=false;
  
  m_outputTracks->clear();
  setFilterPassed(false); 
 
  m_inputL0Muon=get<L0MuonCandidates>(L0MuonCandidateLocation::Default);

  if( 0 == ((int) m_inputL0Muon->size()) ) { 
    return StatusCode::SUCCESS;
  } else {
    debug() << " Nr of L0 muons: " << m_inputL0Muon->size() << endreq;
  }


  
  // loop over 2 d tracks
  for ( std::vector<Track*>::const_iterator itT = m_patInputTracks->begin();
	m_patInputTracks->end() != itT; itT++ ) {
    
    Track* pTr2d = (*itT);
    pTr2d->setFlag(Track::PIDSelected,false);
    if (!acceptTrack(*pTr2d)) continue;
    if( pTr2d->checkFlag( Track::Backward ) ) continue; // skip backward tracks
    float chi=999;
    
    StatusCode sc = match2dMuon(pTr2d,chi);  
    
    if(StatusCode::SUCCESS == sc) {
      m_outputTracks->push_back(pTr2d);
      
      pTr2d->setFlag(Track::PIDSelected,true);
      if(m_debug){        
        debug() << " Selected track" << endreq;
      }
      fillHisto(m_histoChi, chi,1.);
      ok=true;
      setFilterPassed(true);
    }
  }
  if(!ok)return StatusCode::SUCCESS;
  
  return StatusCode::SUCCESS;
};


//=============================================================================
//  match2dMuon method
//=============================================================================
StatusCode MatchPatVeloRL0::match2dMuon(Track* pTr2d, float & chi)  {
  double chi2BestMatchMuon = 999.;
  
  
  
  double trackDrDz = pTr2d->firstState().tx();
  int zone = pTr2d->specific();
  double trackPhi  = (zone * M_PI/4.) - 3.*M_PI/8.;
  double sectorSize= 45.*(M_PI/180.);
  //info()<<" qui "<< trackPhi <<" "<< pTr2d->firstState().y()<<endreq;  
  std::vector<L0MuonCandidate*>::const_iterator itL0Mu;
  // Loop over L0 muons:
  
  for (itL0Mu=m_inputL0Muon->begin();itL0Mu!=m_inputL0Muon->end();itL0Mu++){
//     if((*itL0Muon)->status()==L0Muon::OK){ 
      
      L0MuonCandidate* itMuon = *itL0Mu; 
      //    TrgL0Muon* itMuon = *itL0Mu;
    
      double theta  = itMuon->theta();
      double phi    = itMuon->phi();
    // can we discard matching on the basis of phi/theta 
    // of the track or muon already here?????
      if(m_debug){
        
        debug() << theta	 <<  " " <<phi << " muon "
              << trackDrDz << " " << trackPhi << " track" << endreq; 
      }
      
      std::vector<MuonTileID> list_of_tile= itMuon->muonTileIDs(1);
      MuonTileID tileM2=*(list_of_tile.begin());   

      //      MuonTileID tileM2=itMuon->pad(1);
      double xM2,yM2,zM2;
      double dx,dy,dz;
  
      m_iPosTool->calcTilePos(tileM2,xM2,dx,yM2,dy,zM2,dz);
      
      double xtile  = xM2;
      double ytile  = yM2;
      double ztile  = zM2;
      double dxtile = dx;
      double dytile = dy;
    //double dztile = itMuon->dztile();
      
    // Get energy and position of L0 muon candidate:
      double x	  = xtile ; // mm -> cm
      double y	  = ytile ; // mm -> cm
      double z	  = ztile ; // mm -> cm
      double ex	  = 2.*((float)dxtile)/m_sqrt12 ; // mm -> cm 
      double ey	  = 2.*((float)dytile)/m_sqrt12 ; // mm -> cm
      double et	  = itMuon->pt();
      double e	  = fabs(et/sin(theta));
      // NT 28/08/02 Implement energy correction.
      double de	= 0.3;
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
      double q	  = (et == 0) ? 0.0 : -1.*et/fabs(et);  
    
      // PT kick due to magnet. (Centre of magnet at 5.25 m)
      double xkick  = (z - 5250.0) * m_ptkickConstant/ecor;
      // Uncertainty on PT kick, depending on energy resolution.
      // NT 28/08/02 Use proper formula for xkick uncertainty!
      //double exkick = fabs(  (z - 525.0)*1.24* (1/ecor - 1/(ecor+de)) );
      double exkick = fabs(  (z - 5250.0)*m_ptkickConstant* (1/ecor)*(1/ecor))*de;
    
      // Calculate the slopes and their uncertainties:
      double dxdz   = tan(theta)*cos(phi);
      double edxdz  = sqrt(ex*ex + exkick*exkick)/z;
      double dydz   = tan(theta)*sin(phi);
      double edydz  = ey/z;
      //
      double drdz   = sqrt(dxdz*dxdz + dydz*dydz);
      double edrdz  = sqrt((dxdz*edxdz*dxdz*edxdz)+(dydz*edydz*dydz*edydz))/drdz;
      double ephi   = sqrt((dydz*edxdz*dydz*edxdz)+(dxdz*edydz*dxdz*edydz))/
        (drdz*drdz);
      
    
      // Calculate the matching chi2:
      double ephiTrack = sectorSize/m_sqrt12;
      double deltaPhi  = fabs(phi - trackPhi);
      if (deltaPhi > 2.*M_PI) deltaPhi =      deltaPhi - 2.*M_PI;
      if (deltaPhi > M_PI)    deltaPhi = fabs(deltaPhi - 2.*M_PI);
      double chi2Phi  = ( deltaPhi*deltaPhi) / (ephi*ephi + ephiTrack*ephiTrack);
      double deltaDrDz = fabs(drdz - trackDrDz); 
      double chi2DrDz  = ( deltaDrDz* deltaDrDz)/ (edrdz*edrdz );
      double chi2Matching = chi2Phi + chi2DrDz;
      
      // Output:
      if (chi2Matching < chi2BestMatchMuon) {
        chi2BestMatchMuon = chi2Matching;
      }
      
    // DEBUG:
      if ( msgLevel( MSG::VERBOSE ) ) {
        verbose() << " Found L0 Muon candidate (2d)" 
                  << " Pt: "    << et
                  << " E: "     << e
                  << " dE: "    << de
                  << " q: "     << q
                  << " theta: " << theta
                  << endreq;   
        verbose() << " X: "     << x
                  << " Y: "     << y
                  << " xkick: " << xkick
                  << " exkick: "<< exkick
                  << endreq;   
        verbose() << " dxdz: "  << dxdz << " " << x/z
                  << " dydz: "  << dydz << " " << y/z
                  << " edxdz: " << edxdz
                  << " edydz: " << edydz
                  << endreq;   
        verbose() << " drdz: "  << drdz 
                  << " phi: "   << phi
                  << " edrdz: " << edrdz
                  << " ephi: "  << ephi
                  << endreq;   
      verbose() << " 2d Track: " 
                << " drdz: "  << trackDrDz 
                << " phi: "   << trackPhi
                << " ephi: "  << ephiTrack
                << endreq;   
      verbose() << " Matching quality: "
                << " Delta drdz "  << deltaDrDz
                << " Delta phi "   << deltaPhi
                << " Chi2 drdz "   << chi2DrDz
                << " Chi2 phi "    << chi2Phi
                << " Chi2 "	   << chi2Matching
                << " (muon 2d) "
                << endreq;
      }
      
      // do not wait for the chi2 of the best L0 muon candidate 
      // break the loop if chi2Matching acceptable
      if(chi2Matching < m_maxChi2MatchMuon2d ) {
        break;
      }
    
      
//     } 
	// end loop over L0 muon candidates
     
  }
  
  if(chi2BestMatchMuon < m_maxChi2MatchMuon2d ) {
    chi=chi2BestMatchMuon;
    //    verbose() << " Found GOOD L0 Muon candidate (2d)"<<endreq; 
    return StatusCode::SUCCESS;
  } else {
    return StatusCode::FAILURE;
  }
  
}

//=============================================================================
//  eCorMuon
//=============================================================================
double MatchPatVeloRL0::eCorMuon(const int &istation,
				 const double &e,
				 double &de) {
  // v1 NT 28/08/02 p0 + p1*x + p2*x*x correction
  // v2 NT 29/08/02 p0 + p1/x + p2*x   correction
  // ------------------------------------------------
  
  double ecor = e;
  de	      = 0.235;
  
  if (istation < 0 || istation > 1) {
    err() << " Energy correction not implemented for station: " 
	  << istation << " E: " << e 
	  << endreq;
    return ecor;
  }
  
  double emin =   4000.0;
  double emax = 200000.0;
  
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
      paramecor[istation][0]	  + 
      paramecor[istation][1]/emin + 
      paramecor[istation][2]*emin  ;
  }else if (e > emax){
    ecorfunction = 
      paramecor[istation][0]	  + 
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
//=============================================================================
//  Finalize
//=============================================================================
StatusCode MatchPatVeloRL0::finalize() {

  return HltAlgorithm::finalize();
}


//=============================================================================
