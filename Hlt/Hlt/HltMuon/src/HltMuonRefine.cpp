// $Id: HltMuonRefine.cpp,v 1.3 2007-01-11 16:03:05 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "HltMuonRefine.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltMuonRefine
//
// 2004-11-24 : Alessia Satta
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( HltMuonRefine );

using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltMuonRefine::HltMuonRefine( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{
//  declareProperty( "Par1", m_par1 = 46.0 );
//  declareProperty( "Par2", m_par2 = -0.96 );
//  declareProperty( "Par3", m_par3 = 0.0065 );
//  declareProperty( "Par4", m_par4 = 4.0 );
    declareProperty( "Par1", m_par1 = 66.3 );
    declareProperty( "Par2", m_par2 = 852 );
    declareProperty( "Par4", m_par4 = 1.2 );


    declareProperty("DXR1",m_dxR1=15);
    declareProperty("DXR2",m_dxR2=30);
    declareProperty("DXR3",m_dxR3=60);
    declareProperty("DXR4",m_dxR4=100);
    declareProperty("DYR1",m_dyR1=40);
    declareProperty("DYR2",m_dyR2=80);
    declareProperty("DYR3",m_dyR3=100);
    declareProperty("DYR4",m_dyR4=150);
 

    //  declareProperty( "InputLongTracksName" ,
    //             m_inputLongTracksName  = 
    //             TrgTrackLocation::HltGenericLong );
    //declareProperty( "InputMuonTracksName" ,
    //              m_inputMuonTracksName  = 
    //              TrgTrackLocation::HltGenericMuon);
    //declareProperty( "OutputMuonTracksName" ,
    //              m_outputTracksName  = 
    //              TrgTrackLocation::HltGenericMuon);

}
//=============================================================================
// Destructor
//=============================================================================
HltMuonRefine::~HltMuonRefine() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltMuonRefine::initialize() {

  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  // Counters
  m_countEvents = 0;
  m_countRemoved = 0;

  /*  PatDataStore* store = tool<PatDataStore> ( "PatDataStore", "" );
  m_onTES        = store->onTES();
  m_muonTracksContainer = store->tracks(m_inputMuonTracksName);
  m_tracksInput = store->tracks(m_inputLongTracksName);
  m_tracksOutput = store->tracks(m_outputTracksName);
  m_states       = store->states();
  release( store );
  */

  //  m_l0MuonProvider = tool<TrgL0MuonProvider>( "TrgL0MuonProvider" );
  //m_inputL0Muons   = m_l0MuonProvider->container( TrgL0MuonLocation::Default );


  
  m_iPosTool  = tool<IMuonPosTool>( "MuonPosTool" );
  if(!m_iPosTool)info()<<"error retrieving the pos tool "<<endreq;

  m_L0X[0]=m_dxR1;
  m_L0X[1]=m_dxR2;
  m_L0X[2]=m_dxR3;
  m_L0X[3]=m_dxR4;
  m_L0Y[0]=m_dyR1;
  m_L0Y[1]=m_dyR2;
  m_L0Y[2]=m_dyR3;
  m_L0Y[3]=m_dyR4;
 

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltMuonRefine::execute() {



  debug() << "==> Execute" << m_patInputTracks->size()<<endmsg;
//std::string nameLong=m_tracksInput->name();
//std::string nameMuon=m_muonTracksContainer->name();
  HltAlgorithm::beginExecute();
  setFilterPassed(false);
  m_inputL0Muon=get<L0MuonCandidates>(L0MuonCandidateLocation::Default );
  m_outputTracks->clear();  

  //  debug() << "Number of Long tracks in input: " << m_tracksInput->size() << " "<<m_inputLongTracksName<<endmsg;
  //debug() << "Number of Muon tracks in input: " << m_muonTracksContainer->size() <<endmsg;)

  m_countEvents += 1;
  
  //std::vector< State* >::iterator itState;
  std::vector< State* > statesList;
  float slX=0.,slY=0.,X=0.,Y=0.,Z=0.;

  Tracks::const_iterator itMuonTrack;  
  float distmin;
  float dist;
  double XMuon,YMuon,ZMuon;
  //ZMuon=15200;
  float momentum;
  double dx,dy,dz;
  
  Tracks::const_iterator pItr;

  int nRemoved = 0;
  MuonTileID tilemu;
  std::vector<L0MuonCandidate*>::const_iterator itL0Mu;

  
  for ( pItr = m_inputTracks->begin(); m_inputTracks->end() != pItr; 
        pItr++ ) { 
    Track* thisTrack = (*pItr);
    if(thisTrack->checkFlag( Track::Invalid ))continue;   
    //if(thisTrack->checkFlag( Track::Backward ))continue;

//    if((*pItr)->checkFlag( Track::PIDSelected)){  
      momentum=(*pItr)->firstState().p();      
      statesList= (*pItr)->states();
      if((*pItr)->hasStateAt(State::AtT)){
        State stato=(*pItr)->closestState(10000);
        if(stato.checkLocation(State::AtT)){
          slX=stato.tx();
          slY=stato.ty();
          X=stato.x();
          Y=stato.y();
          Z=stato.z();
          //          break;          
        }        
  //    }
      
      //mu parameters known
      distmin=9999;      
      //who was matched to the tracks ? L0 or HltMuon? 
      int whoselected=-1;
      //int region=-1;
      //      statesList= (*pItr)->states();
      if((*pItr)->hasStateAt(State::Muon)){
        State stato=(*pItr)->closestState(15000);
        if(stato.checkLocation(State::Muon)){
          slX=stato.tx();
          slY=stato.ty();
          X=stato.x();
          Y=stato.y();
          Z=stato.z();
          if(Z>11000&&Z<13000)whoselected=0;
          
          if(Z>14000&&Z<16000)whoselected=2;
          
        }
      
      
      }else 
        info()<<" che diavolo succede "<<endreq;
      
      
      if(whoselected<0){
        (*pItr)->setFlag(Track::PIDSelected,false); 
        nRemoved += 1;
        continue;
      }
      bool good=false;
      //      debug()<<" whoselected "<<whoselected<<" "<< endreq;     
      if(whoselected==1){
        for (itL0Mu=m_inputL0Muon->begin();itL0Mu!=m_inputL0Muon->end();
             itL0Mu++){
          L0MuonCandidate* itMuon = *itL0Mu;
          MuonTileID tileM2=itMuon->pad(1);
          m_iPosTool->
          calcTilePos(tileM2,XMuon, dx,YMuon, dy,ZMuon, dz);
          dist=calcDist(slX,slY,X,Y,Z,XMuon,YMuon,ZMuon);   
          if(dist<distmin)distmin=dist;        
          good=acceptMatch(distmin,momentum);          
          if(good)break;
        }      
      }
      
      if(whoselected==2){
        for(itMuonTrack = m_patInputTracks->begin();
            itMuonTrack < m_patInputTracks->end(); itMuonTrack++){
          // if(!(*itMuonTrack)->clone()){
          XMuon=(*itMuonTrack)->firstState().x();
          YMuon=(*itMuonTrack)->firstState().y();
          ZMuon=15200;
          
          dist=calcDist(slX,slY,X,Y,Z,XMuon,YMuon,ZMuon);   
          if(dist<distmin)distmin=dist;          
          //}
        }
        good=acceptMatch(distmin,momentum);
      }
      
      if(good){
        setFilterPassed(true);
        
               debug()<<" muon flag "<<whoselected<<" "<<thisTrack->p()<<endreq;       
        (*pItr)->setFlag(Track::PIDSelected,true);
        m_outputTracks->push_back(thisTrack);
        
      }else{
        debug()<<"remove track "<<whoselected<<" "
               <<slX<<" "<<slY<<" "<<X<<" "<<XMuon<<endreq;
        (*pItr)->setFlag(Track::PIDSelected,false);
        
        //debug()<<(*pItr)->checkFlag( Track::PIDSelected)<<endreq;;
        nRemoved += 1;
      }
      
    }    
  }  
  HltAlgorithm::endExecute();
  
  
  debug() << 
    "Average number of muon candidates discarded once long tracks are used: "
          << nRemoved << endmsg;
  m_countRemoved += nRemoved;
  
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltMuonRefine::finalize() {

  debug() << "==> Finalize" << endmsg;

  info () << "Number of events processed: " << double(m_countEvents) << endreq;
  info () << "Average number of muon candidates discarded once long tracks are used:" 
          << double(m_countRemoved)/double(m_countEvents) << endreq;

  return HltAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================


float HltMuonRefine::calcDist(float slX,float slY,float X,float Y,
                              float Z,float XMuon,float YMuon,float ZMuon)
{
  float distx=(slX*(ZMuon-Z)+X-XMuon);
  float disty=(slY*(ZMuon-Z)+Y-YMuon);
  float dista=sqrt(distx*distx+disty*disty/4);
  

  return dista;
  
};



bool HltMuonRefine::acceptMatch(float dist,float momentum)
{
  float pp=fabs(momentum)/1000.0;
  
//float   cutdist=(m_par1+m_par2*pp+m_par3*pp*pp)*m_par4;
  float cutdist=sqrt(m_par1*m_par1+m_par2*m_par2/(pp*pp))*m_par4;
  if(dist<cutdist)return true;
  else return false;
 
};

bool HltMuonRefine::acceptMatchL0(float slX,float slY,float X,float Y,float Z,
                              double XMuon,double YMuon,double ZMuon,
                                  int region){
  float distX=X+slX*(ZMuon-Z)-XMuon;
  float distY=Y+slY*(ZMuon-Z)-YMuon;
  if(fabs(distX)<m_L0X[region]&&fabs(distY)<m_L0Y[region])  return true;
  else return false;
};

