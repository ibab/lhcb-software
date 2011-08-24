
// $Id: TrackMuonMatching.cpp,v 1.1 2010-06-04 13:02:57 svecchi Exp $
// Include files 

// from STD
#include <functional>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// from TrackEvent
#include "Event/TrackParameters.h"
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/Node.h"
#include "Event/MCHit.h"

// BOOST
#include "boost/foreach.hpp"
#include "boost/lambda/bind.hpp"
#include "Event/ODIN.h"
#include "Event/L0DUReport.h"


// local
#include "TrackMuonMatching.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackMuonMatching
// 2010-06-04 : Stefania Vecchi (based on OTMuonMatching by Jan Amoraal)
//-----------------------------------------------------------------------------

using namespace LHCb;
using namespace boost::lambda;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TrackMuonMatching );

TrackMuonMatching::TrackMuonMatching( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator )
{
  declareProperty( "TTracksLocation"     , m_tTracksLocation      = TrackLocation::Default     );
  declareProperty( "MuonTracksLocation"  , m_mTracksLocation      = TrackLocation::Muon        );
  declareProperty( "TracksOutputLocation", m_tracksOutputLocation = "Rec/Track/Best/TMuon"     );
  declareProperty( "Extrapolator"        , m_nExtrapolator        = "TrackLinearExtrapolator"  );
  declareProperty( "Chi2Calculator"      , m_nChi2Calculator      = "TrackChi2Calculator"      );
  declareProperty( "MatchAtZ"            , m_matchAtZ             = 12500*Gaudi::Units::mm     );
  declareProperty( "MatchAtFirstMuonHit" , m_matchAtFirstMuonHit  = false                      );
  declareProperty( "MatchChi2Cut"        , m_matchChi2Cut         = 100.0                      );
  declareProperty( "AllCombinations"     , m_allCombinations      = true                       );
  declareProperty( "WriteNtuple"         , m_ntuple               = true                       );
}

TrackMuonMatching::~TrackMuonMatching() {} 

StatusCode TrackMuonMatching::initialize() {
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_extrapolator   = tool<ITrackExtrapolator>(   m_nExtrapolator  , "Extrapolator"  , this );
  m_chi2Calculator = tool<ITrackChi2Calculator>( m_nChi2Calculator, "Chi2Calculator", this );
  m_muonDet=getDet<DeMuonDetector>(DeMuonLocation::Default); //non funziona, si rompe
  for(int iMS =0; iMS<nSTATIONS; iMS++){    
    m_zMS[iMS] = m_muonDet->getStationZ(iMS);
  }
  return StatusCode::SUCCESS;
}

StatusCode TrackMuonMatching::execute() {
  int runnum(-99), evnum(-99);
  if( exist<LHCb::ODIN>(LHCb::ODINLocation::Default)) {
    LHCb::ODIN* odin = get<LHCb::ODIN>(LHCb::ODINLocation::Default);
    runnum = odin->runNumber();
    evnum = odin->eventNumber();
  }
  
  int L0decision(-99), L0muon(-99), L0calo(-99);
  if( exist<LHCb::L0DUReport>(LHCb::L0DUReportLocation::Default) ) {
    LHCb::L0DUReport* l0rep = get<LHCb::L0DUReport>(LHCb::L0DUReportLocation::Default); 
    L0decision = int(l0rep->decision());
    L0muon = int(l0rep->channelDecisionByName("MUON,minbias"));    
    L0calo = int(l0rep->channelDecisionByName("CALO"));
    debug()<<" L0 decision "<<L0decision<<" L0calo "<<L0calo<<" L0muon "<<L0muon<<endmsg;    
  }


  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  Tracks*       matchedTracks = new Tracks();
  put( matchedTracks, m_tracksOutputLocation );


  const Tracks*       tTracks = NULL; 
  bool flagOT=false;  
  if ( exist<LHCb::Tracks>( m_tTracksLocation )){
    tTracks = get<Tracks>( m_tTracksLocation );
    if (tTracks->size()!=0) flagOT = true;
  }
  
  const Tracks*       mTracks = NULL;
  bool flagMUON=false;  
  if(exist<LHCb::Tracks>( m_mTracksLocation )){
    mTracks = get<Tracks>( m_mTracksLocation );
    if(mTracks->size()!=0) flagMUON = true;
  }
  if(!mTracks ) {
    Warning("the Muon track  container is empty / no matching or ntuple filling is possible", StatusCode::FAILURE, 5 ).ignore();   
    return StatusCode::SUCCESS;
  }

  std::vector< int >    mStation      ;
  std::vector< int >    mRegion       ; 
  std::vector< double > matchChi2s    ;
  std::vector< State  > matchedTStates; 
  std::vector< State  > matchedMStates;
  std::vector< State* > besttState    ;
  std::vector< State* > extraptState  ;  
  std::vector< double > bestchi2      ;

  int i = -1;
  double    z = m_matchAtZ;

  /// Now match this T track to muon tracks
  info()<<"muon track size "<<mTracks->size()<<" tTrack size "<< tTracks->size()<< endreq;
  
  for ( Tracks::const_iterator m = mTracks->begin(), mEnd = mTracks->end(); m != mEnd; ++m ) {
    debug()<<" MuonTrack chi2 "<<(*m)->chi2PerDoF()<<endmsg;
    if( (*m)->chi2PerDoF() > 5.) continue;
    
    State *tState=0;
    State *mState=0;
    i++;
    besttState.push_back(NULL);
    double minchi2 = 10000;

    if(flagOT){      
      if ( m_matchAtFirstMuonHit ) {
        if ( (*m)->hasStateAt( State::Muon ) ) mState = (*m)->stateAt( State::Muon );
        else if ( (*m)->hasStateAt( State::FirstMeasurement )  ) mState = (*m)->stateAt( State::FirstMeasurement );
        else   Warning( "Couldn't find muon state. Going to extrapolate to default z", StatusCode::FAILURE, 5 ).ignore();
        
        z = mState->z();
        verbose() << "Found muon state. Going to extrapolate to this state with z = " << z << endmsg;
      }
      /// Matched Muon-T track
      Track* best_matchedTrack = new Track();
      for ( Tracks::const_iterator t = tTracks->begin(), tEnd = tTracks->end(); t != tEnd; ++t ) {
        if(!(*t)->hasT()) continue;
        if( (*t)->chi2PerDoF() > 5.) continue;
        
        double chi2 = -9999.0;
        /// Get the T state closest to this z
        tState = &(*t)->closestState( z );
        /// Get the Muon state closest to this z
        mState = &(*m)->closestState( z );
        //  Calculate mach chi2
        StatusCode   sc = matchChi2( *tState, *mState, z, chi2 );
        
        if ( sc.isSuccess() && chi2 > -1.0 && chi2 < m_matchChi2Cut   ) {
          info() << "chi2 Matching is " << chi2 << "tTrack: "<< (*t)->type() <<" chi2/ndof "<< (*t)->chi2PerDoF() <<
            " tTrack.p() "<<(*t)->p()<< " mTrack "<< (*m)->chi2PerDoF() <<"  mTrack.p() "<<(*m)->p()<<endmsg;
          
          matchChi2s.push_back( chi2 );
          tState->setLocation(State::Muon);        
          mState->setLocation(State::Muon);        
          matchedTStates.push_back( *tState );
          matchedMStates.push_back( *mState );          
          mStation.push_back( (*m)->lhcbIDs().front().muonID().station() ); 
          mRegion.push_back( (*m)->lhcbIDs().front().muonID().region() );
          
          if(m_allCombinations) {
            Track* matchedTrack = new Track();
            matchedTrack->copy( *(*t) );
            matchedTrack->addToStates( *tState );
            /// Add muon ids to copied T track
            BOOST_FOREACH( LHCbID id, (*m)->lhcbIDs() ) matchedTrack->addToLhcbIDs( id );
            matchedTracks->insert( matchedTrack );
          }
          
          if(chi2<minchi2){              
            besttState[i]=tState;
            minchi2=chi2;
            if(!m_allCombinations){
              best_matchedTrack->copy( *(*t) );
              best_matchedTrack->addToStates( *tState );

              /// Add muon ids to copied T track
              BOOST_FOREACH( LHCbID id, (*m)->lhcbIDs() ) best_matchedTrack->addToLhcbIDs( id );
            }              
          }
        }  else {
          info()<< "matching failed "<<chi2 <<endmsg;
          sc = StatusCode::SUCCESS;
        }        
      }
      if(!m_allCombinations && minchi2 < m_matchChi2Cut ) {
        info()<< " inserting only best matched track "<< best_matchedTrack->p()<<endmsg;
        matchedTracks->insert( best_matchedTrack );
      }else{
        delete best_matchedTrack;        
      }
      
      bestchi2.push_back(minchi2);
    }  
  }  
  info()<<" Matched track container size "<<matchedTracks->size()<<endmsg;
  
   
  
  if(m_ntuple){
    
    Tuple Simone = nTuple(1,"Brunelmuonhit");
    

    /// Now match this T track to muon tracks
    i =-1;
    for ( Tracks::const_iterator m = mTracks->begin(), mEnd = mTracks->end(); m != mEnd; ++m ) {
      if( (*m)->chi2PerDoF() > 5.) continue;
      debug()<<"*** saving muon tracks ***"<<endreq;
      i++;
      double x, y, z, dx, dy, dz;
      std::vector<LHCb::LHCbID>  list_of_tile = (*m)->lhcbIDs () ;
      std::vector<LHCb::LHCbID>::const_iterator itTile;
      
      std::vector<double>        Vxm, Vym, Vzm, Vdxm, Vdym, Vdzm, region, station, Chamber, 
                                 OTx,OTerrx, OTy, OTerry, OTz, OTtx, OTerrtx, OTty, OTerrty;
      
      int p = 1;
      float momOT(-9999999);          
      
      for (itTile = list_of_tile.begin(); itTile != list_of_tile.end(); itTile++){
        if ((*itTile).isMuon() == true) {
          
          LHCb::MuonTileID tile = itTile->muonID();
          std::vector<DeMuonChamber*> vchambers;
          
          m_muonDet->Tile2XYZ(tile,x,dx,y,dy,z,dz);
          vchambers = m_muonDet->Tile2Chamber(tile);


          debug()<<"*** tile position ***"<<tile<<endreq;
          debug()<<" x = "<<x<<" y = "<<y<<" z = "<<z<<endreq;
          debug()<<" dx = "<<dx<<" dy ="<<dy<<" dz = "<<dz<<endreq;
          debug()<<" region "<<tile.region()<<" station "<<tile.station()<<endreq;
          debug()<<"*********************"<<tile<<endreq;
          
          Vxm.push_back(x);
          Vym.push_back(y);
          Vzm.push_back(z);
          Vdxm.push_back(dx);
          Vdym.push_back(dy);
          Vdzm.push_back(dz);
          region.push_back(float(tile.region()));
          station.push_back(float(tile.station()));
          Chamber.push_back(float(vchambers[0]->chamberNumber()));
          p++;

          float vOTx(-999), vOTy(-999), vOTz(-999),vOTtx(-999), vOTty(-999), 
                vOTerrx(-999), vOTerry(-999), vOTerrtx(-999), vOTerrty(-999);
          if(flagOT){
            if (besttState[i] != 0 ){              
              StatusCode sc = OTmuonExtrap( besttState[i], z );
              if(sc) {
                momOT = besttState[i]->p();
                vOTx = besttState[i]->x();
                vOTy = besttState[i]->y();
                vOTz = besttState[i]->z();
                vOTtx = besttState[i]->tx();
                vOTty = besttState[i]->ty();
                vOTerrx =  sqrt(besttState[i]->errX2() );
                vOTerry =  sqrt(besttState[i]->errY2() );
                vOTerrtx =  sqrt(besttState[i]->errTx2() );
                vOTerrty =  sqrt(besttState[i]->errTy2() );
              }              
            }     
          }
          
          OTx.push_back(vOTx);
          OTy.push_back(vOTy);
          OTz.push_back(vOTz);
          OTtx.push_back(vOTtx);
          OTty.push_back(vOTty);
          OTerrx.push_back(vOTerrx);
          OTerry.push_back(vOTerry);
          OTerrtx.push_back(vOTerrtx);
          OTerrty.push_back(vOTerrty);
          
        } else   verbose()<<" this LHCbID is not a MuonTile "<<endreq;
        
      }
      
      if (p<=30){
        float chi2=(*m)->chi2PerDoF();
        float mom=(*m)->p()*float((*m)->charge());
        debug()<<" chiquadro "<<chi2<<" momento "<<mom<<" momento OT"<< momOT<<endmsg;
        
        Simone->column( "run"                , runnum);
        Simone->column( "ev"                 , evnum);
        Simone->column( "L0decision"         , L0decision);
        Simone->column( "L0muon"             , L0muon);
        Simone->column( "L0calo"             , L0calo);
        Simone->column("Chi2"                , chi2);
        Simone->column("p"                   , mom);
        Simone->column("pOT"                 , momOT);
        
        Simone->farray("x",Vxm.begin(),Vxm.end(),"Npad",30);
        Simone->farray("y",Vym.begin(),Vym.end(),"Npad",30);
        Simone->farray("z",Vzm.begin(),Vzm.end(),"Npad",30);
        Simone->farray("dx",Vdxm.begin(),Vdxm.end(),"Npad",30);
        Simone->farray("dy",Vdym.begin(),Vdym.end(),"Npad",30);
        Simone->farray("dz",Vdzm.begin(),Vdzm.end(),"Npad",30);
        Simone->farray("station",station.begin(),station.end(),"Npad",30);
        Simone->farray("R",region.begin(),region.end(),"Npad",30);
        Simone->farray("NCh",Chamber.begin(),Chamber.end(),"Npad",30);


        Simone->farray("OTx",OTx.begin(),OTx.end(),"Npad",30);
        Simone->farray("OTy",OTy.begin(),OTy.end(),"Npad",30);
        Simone->farray("OTz",OTz.begin(),OTz.end(),"Npad",30);
        Simone->farray("OTtx",OTtx.begin(),OTtx.end(),"Npad",30);
        Simone->farray("OTty",OTty.begin(),OTty.end(),"Npad",30);
        Simone->farray("OTerrx",OTerrx.begin(),OTerrx.end(),"Npad",30);
        Simone->farray("OTerry",OTerry.begin(),OTerry.end(),"Npad",30);
        Simone->farray("OTerrtx",OTerrtx.begin(),OTerrtx.end(),"Npad",30);
        Simone->farray("OTerrty",OTerrty.begin(),OTerrty.end(),"Npad",30);
        
      }      
      LHCb::MCHits* muonhits = NULL;
      if( exist < LHCb::MCHits > (LHCb::MCHitLocation::Muon )) muonhits = get< LHCb::MCHits > (LHCb::MCHitLocation::Muon );
      if (muonhits!=NULL ){
        std::vector<double>  x_hit, y_hit, z_hit, tx_hit, ty_hit, time_hit, PID_hit, station_hit;      
        int hit_counter(0);  int counter[5]={0,0,0,0,0};
        
        for ( LHCb::MCHits::const_iterator iMc = muonhits->begin(); iMc != muonhits->end(); ++iMc ) {
          hit_counter++;          
          if(hit_counter>50)continue;
          x_hit.push_back((*iMc)->midPoint().x());
          y_hit.push_back((*iMc)->midPoint().y());
          z_hit.push_back((*iMc)->midPoint().z());
          int iSTATION=-1;          
          for(int i=0; i<nSTATIONS; i++)  if((*iMc)->midPoint().z() > m_zMS[i] - 400) iSTATION=i;
          station_hit.push_back(float(iSTATION));
          double PID =0.;          
          if ((*iMc)->mcParticle()!=NULL) PID = (*iMc)->mcParticle()->particleID().pid();
          PID_hit.push_back(PID);

          debug()<<" Hit pos = ("<<
            (*iMc)->midPoint().x()<<","<<(*iMc)->midPoint().y()<<","<<(*iMc)->midPoint().z()<<
            ") in station M"<<iSTATION+1<<" mother PID "<< PID<<endmsg;
          
          
          tx_hit.push_back((*iMc)->dxdz()); 
          ty_hit.push_back((*iMc)->dydz());
          time_hit.push_back((*iMc)->time());
          if(iSTATION>-1&&iSTATION<5) counter[iSTATION]++;
        }
        debug()<< " Found "<<hit_counter<<" hits in total; wrote first 50 // ";
        for(int i=0; i<nSTATIONS; i++) debug()<< counter[i]<<" hits in M"<<i+1<<endmsg;
          
        Simone->farray("x_hit",x_hit.begin(),x_hit.end(),"Nhit",50);
        Simone->farray("y_hit",y_hit.begin(),y_hit.end(),"Nhit",50);
        Simone->farray("z_hit",z_hit.begin(),z_hit.end(),"Nhit",50);
        Simone->farray("tx_hit",tx_hit.begin(),tx_hit.end(),"Nhit",50);
        Simone->farray("ty_hit",ty_hit.begin(),ty_hit.end(),"Nhit",50);
        Simone->farray("time_hit",time_hit.begin(),time_hit.end(),"Nhit",50);
        Simone->farray("PID_hit",PID_hit.begin(),PID_hit.end(),"Nhit",50);
        Simone->farray("station_hit",station_hit.begin(),station_hit.end(),"Nhit",50);
      }

      if(flagOT) Simone->fill("bestchi2",bestchi2[i]);
      else Simone->fill("bestchi2",-999);
      if (m_ntuple) { 
        Simone->write(); 
        debug()<<" Writing the Ntuple with Npad "<<Vxm.size()<<endmsg;        
      }
          
      Vxm.clear();
      Vym.clear();
      Vzm.clear();
      Vdxm.clear();
      Vdym.clear();
      Vdzm.clear();
      region.clear();
      station.clear();
      Chamber.clear();
      
      OTx.clear();      
      OTerrx.clear();      
      OTy.clear();      
      OTerry.clear();      
      OTz.clear();       
      OTtx.clear();      
      OTerrtx.clear();       
      OTty.clear();       
      OTerrty.clear();      

    }//end loop over tracks    
  }
  return StatusCode::SUCCESS;
}

  

StatusCode TrackMuonMatching::matchChi2( LHCb::State& tState, LHCb::State& mState, 
                                             const double& atZ,
                                             double& chi2 ) {
  StatusCode sc = StatusCode::SUCCESS;
  
  /// Extrapolate states
  sc = m_extrapolator->propagate( tState, atZ );
  if ( !sc.isSuccess() ) {
    Warning( "Could not propagate T state"   , StatusCode::FAILURE, 5 );
    return  StatusCode::FAILURE;
  }
  
  sc = m_extrapolator->propagate( mState, atZ );
  if ( !sc.isSuccess() ) {
    Warning( "Could not propagate Muon state", StatusCode::FAILURE, 5 );
    return  StatusCode::FAILURE;
  }
  
  if ( msgLevel( MSG::DEBUG  ) ) {
    debug() << "Extrapolated T state to z = "    << atZ << " is " << tState << endmsg
            << "Extrapolated Muon state to z = " << atZ << " is " << mState << endmsg;
  }

  /// Now calculate the match chi2
  sc = m_chi2Calculator->calculateChi2( tState.stateVector(), tState.covariance(), 
                                        mState.stateVector(), mState.covariance(),
                                        chi2 );
  if ( !sc.isSuccess() ) Error( "Could not invert matrices", StatusCode::FAILURE );

  return sc;
}

StatusCode TrackMuonMatching::OTmuonExtrap( LHCb::State* tState, const double& atZ) {
  StatusCode sc = StatusCode::SUCCESS;
  
  /// Extrapolate states
  sc = m_extrapolator->propagate( (*tState), atZ );
  if ( !sc.isSuccess() ) Warning( "Could not propagate T state"   , StatusCode::FAILURE, 5 );
  
  if ( msgLevel( MSG::DEBUG  ) ) {
    debug() << "Extrapolated T state to z = "    << atZ << " is " << (*tState) << endmsg;
  
  }


  return sc;
}
