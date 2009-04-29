// $Id: HltMuonRefine.cpp,v 1.10 2009-04-29 14:18:11 asatta Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "boost/assign/list_of.hpp"

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
  , m_selections(*this)
{
//  declareProperty( "Par1", m_par1 = 46.0 );
//  declareProperty( "Par2", m_par2 = -0.96 );
//  declareProperty( "Par3", m_par3 = 0.0065 );
//  declareProperty( "Par4", m_par4 = 4.0 );
    declareProperty( "Par1", m_par1 = 66.3 );
    declareProperty( "Par2", m_par2 = 852 );
    declareProperty( "Par4", m_par4 = 1.2 );


    declareProperty("DXR",m_L0X = boost::assign::list_of(15.)(30.)( 60.)(100.) );
    declareProperty("DYR",m_L0Y = boost::assign::list_of(40.)(80.)(100.)(150.) );
 

    declareProperty( "TESInputTracksName" ,
                     m_TESInputTracksName = LHCb::TrackLocation::HltForward);
    m_selections.declareProperties();

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

  m_selections.retrieveSelections();
  m_selections.registerSelection();

  // Counters
  m_countEvents = 0;
  m_countRemoved = 0;


  //  m_l0MuonProvider = tool<TrgL0MuonProvider>( "TrgL0MuonProvider" );
  //m_inputL0Muons   = m_l0MuonProvider->container( TrgL0MuonLocation::Default );  
  m_iPosTool  = tool<IMuonFastPosTool>( "MuonFastPosTool" );
  if(!m_iPosTool)info()<<"error retrieving the pos tool "<<endreq;

  if(m_L0X.size()!=4) {
    error()<< "wrong number of arguments for m_L0X" << endmsg;
    return StatusCode::FAILURE;
  }
  if(m_L0Y.size()!=4) {
    error()<< "wrong number of arguments for m_L0Y" << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltMuonRefine::execute() {

  Tracks* m_TESInputTracks = get<Tracks>(m_TESInputTracksName);

//std::string nameLong=m_tracksInput->name();
//std::string nameMuon=m_muonTracksContainer->name();
  setFilterPassed(false);
  m_inputL0Muon=get<L0MuonCandidates>(L0MuonCandidateLocation::Default );
  m_selections.output()->clean();  

  //  debug() << "Number of Long tracks in input: " << m_tracksInput->size() << " "<<m_inputLongTracksName<<endmsg;
  //debug() << "Number of Muon tracks in input: " << m_muonTracksContainer->size() <<endmsg;)

  m_countEvents += 1;
  
  //std::vector< State* >::iterator itState;
  std::vector< State* > statesList;
  double slX=0.,slY=0.,X=0.,Y=0.,Z=0.;

  Tracks::const_iterator itMuonTrack;  
  double distmin;
  double dist;
  double XMuon,YMuon,ZMuon;
  //ZMuon=15200;
  double momentum;
  double dx,dy,dz;
  
  Tracks::const_iterator pItr;

  int nRemoved = 0;
  MuonTileID tilemu;
  std::vector<L0MuonCandidate*>::const_iterator itL0Mu;

  
  for ( pItr = m_selections.input<1>()->begin(); m_selections.input<1>()->end() != pItr; 
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
	  std::vector<MuonTileID> list_of_tile= itMuon->muonTileIDs(1);
          MuonTileID tileM2=*(list_of_tile.begin());
          m_iPosTool->
          calcTilePos(tileM2,XMuon, dx,YMuon, dy,ZMuon, dz);
          dist=calcDist(slX,slY,X,Y,Z,XMuon,YMuon,ZMuon);   
          if(dist<distmin)distmin=dist;        
          good=acceptMatch(distmin,momentum);          
          if(good)break;
        }      
      }
      
      if(whoselected==2){
        for(itMuonTrack = m_TESInputTracks->begin();
            itMuonTrack < m_TESInputTracks->end(); itMuonTrack++){
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
        m_selections.output()->push_back(thisTrack);
        
      }else{
        debug()<<"remove track "<<whoselected<<" "
               <<slX<<" "<<slY<<" "<<X<<" "<<XMuon<<endreq;
        (*pItr)->setFlag(Track::PIDSelected,false);
        
        //debug()<<(*pItr)->checkFlag( Track::PIDSelected)<<endreq;;
        nRemoved += 1;
      }
      
    }    
  }  
  
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





bool HltMuonRefine::acceptMatch(double dist,double momentum)
{
  double pp=fabs(momentum)/1000.0; //@FIXME: use explicit units...
  double cutdist=sqrt(m_par1*m_par1+m_par2*m_par2/(pp*pp))*m_par4;
  return dist<cutdist;
};

bool HltMuonRefine::acceptMatchL0(double slX,double slY,double X,double Y,double Z,
                              double XMuon,double YMuon,double ZMuon,
                                  int region){
  double distX=X+slX*(ZMuon-Z)-XMuon;
  double distY=Y+slY*(ZMuon-Z)-YMuon;
  return(fabs(distX)<m_L0X[region]&&fabs(distY)<m_L0Y[region]);
};
