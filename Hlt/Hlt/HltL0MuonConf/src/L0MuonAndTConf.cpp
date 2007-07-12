// $Id: L0MuonAndTConf.cpp,v 1.3 2007-07-12 17:45:06 asatta Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "Event/MuonCoord.h"

// local
#include "L0MuonAndTConf.h"
using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : L0MuonAndTConf
//
// 2007-02-21 : Alessia Satta
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0MuonAndTConf );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0MuonAndTConf::L0MuonAndTConf( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{
  declareProperty( "maxDist",m_maxdist=100);
//  declareProperty( "skipFilter",m_skipFilter=false);
  
}
//=============================================================================
// Destructor
//=============================================================================
L0MuonAndTConf::~L0MuonAndTConf() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode L0MuonAndTConf::initialize() {
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
  m_iPosTool  = tool<IMuonPosTool>( "MuonPosTool" );
  if(!m_iPosTool)info()<<"error retrieving the pos tool "<<endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0MuonAndTConf::execute() {

  debug() << "==> Execute" << endmsg;
  setFilterPassed(true);
//  HltAlgorithm::beginExecute();
  for ( std::vector<Track*>::const_iterator itT = m_inputTracks->begin();
        m_inputTracks->end() != itT; itT++ ) {   
     MuonTileID tileM4, tileM5;
    if(calcDLL(*itT, tileM4, tileM5)<m_maxdist){ 
      setFilterPassed(true);
       //if(!skipFilter){
//	(*itT)->setFlag(Track::L0Candidate,true);
  //     }
       (*itT)->addToAncestors (*itT);
       (*itT)->addToLhcbIDs(tileM4);
       (*itT)->addToLhcbIDs(tileM5);
       m_outputTracks->push_back(*itT);


    }
    
  }
  
//  HltAlgorithm::endExecute();
//  if(m_skipFilter) setFilterPassed(true);
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0MuonAndTConf::finalize() {

  debug() << "==> Finalize" << endmsg;

  return HltAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================



double  L0MuonAndTConf::calcDLL(Track* track, MuonTileID & tileM4, MuonTileID & tileM5)
{

  double distDLL=0;
  std::vector< LHCb::LHCbID >  lista=   track->lhcbIDs ();
  MuonTileID tileM2;
  MuonTileID tileM3;

      
  std::vector<LHCbID>::iterator it;
  for(it=lista.begin();it<lista.end();it++){ 
    if(it->isMuon())
    {
      MuonTileID tile=it->muonID();
      if(tile.station()==1)tileM2=tile;
      if(tile.station()==2)tileM3=tile;
       
    }
     
  }
  if(!(tileM2.isValid())||!(tileM3.isValid()))return 10000;
  
  //M2 and M3
  double x,y,z,dx,dy,dz;

  m_iPosTool-> calcTilePos(tileM2,x, dx,y, dy,z, dz);  

  State* stato=NULL;
  stato=&(track->firstState());
  float xpoint=stato->x();
  float ypoint=stato->y();
  float zpoint=stato->z();
  float xslope=stato->slopes().x()/stato->slopes().z();
  float yslope=stato->slopes().y()/stato->slopes().z();
  float x_M2=xpoint+xslope*(z-zpoint);
  float y_M2=ypoint+yslope*(z-zpoint); 
  distDLL=((x_M2-x)/dx)*((x_M2-x)/dx)+
    ((y_M2-y)/dy)*((y_M2-y)/dy);   
  debug()<<" dist M2 "<<distDLL<<" "<<x<<" "<<dx<<" "<<x_M2<<endreq;
  debug()<<" dist M2 "<<distDLL<<" "<<y<<" "<<dy<<" "<<y_M2<<endreq;
  //info()<<
  m_iPosTool-> calcTilePos(tileM3,x, dx,y, dy,z, dz);
  float x_M3=xpoint+xslope*(z-zpoint);
  float y_M3=ypoint+yslope*(z-zpoint);
  distDLL+=((x_M3-x)/dx)*((x_M3-x)/dx)+
    ((y_M3-y)/dy)*((y_M3-y)/dy);

  debug()<<" dist M3 "<<distDLL<<endreq;

  //M4 and M5




  LHCb::MuonCoords* coords =  get<LHCb::MuonCoords>(LHCb::MuonCoordLocation::MuonCoords);


  if ( coords==0 ) {
    err() << " Cannot retrieve MuonCoords " << endreq;
    return StatusCode::FAILURE;
  }
  // loop over the coords
  float dist_tempM4=100000;
  float dist_tempM5=100000;
  LHCb::MuonCoords::const_iterator iCoord;
  for ( iCoord = coords->begin() ; iCoord != coords->end() ; iCoord++ ){
    int region = (*iCoord)->key().region();
    int station = (*iCoord)->key().station();
    if(station==0||station==1||station==2)continue;
    if(region!=0&&(*iCoord)->uncrossed())continue;
    debug()<<station<<" "<<region<<endreq;

    m_iPosTool-> calcTilePos( (*iCoord)->key(),x, dx,y, dy,z, dz);
    float dist_temp=0;
    float x_Mi=xpoint+xslope*(z-zpoint);
    float y_Mi=ypoint+yslope*(z-zpoint);
    dist_temp=((x_Mi-x)/dx)*((x_Mi-x)/dx)+
      ((y_Mi-y)/dy)*((y_Mi-y)/dy);
   
    if(station==3){
      if(float(dist_temp)<float(dist_tempM4)){
         dist_tempM4=dist_temp;
         tileM4 = (*iCoord)->key();
      }
    }

    if(station==4){
      if(float(dist_temp)<float(dist_tempM5)){
         dist_tempM5=dist_temp;
         tileM5 = (*iCoord)->key();
      }
    }
    

    
  }

  distDLL+=dist_tempM4;
  debug()<<" dist M4 "<<distDLL<<endreq;
  distDLL+=dist_tempM5;
  debug()<<" dist M5 "<<distDLL<<endreq;
  return distDLL;

      
}  
