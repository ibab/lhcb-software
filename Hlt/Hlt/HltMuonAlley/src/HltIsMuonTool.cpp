// $Id: HltIsMuonTool.cpp,v 1.3 2008-02-07 14:03:36 hernando Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

//boost
#include <boost/assign/list_of.hpp>

// local
#include "HltIsMuonTool.h"
#include "Event/MuonCoord.h"

//-----------------------------------------------------------------------------
// Implementation file for class :HltIsMuonTool
//
// 2008-01-25 : Leandro de Paula
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( HltIsMuonTool );

using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltIsMuonTool::HltIsMuonTool( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
// FOI parameters from off line   28/1/8
  declareProperty( "par1x", m_p1x = boost::assign::list_of (6.5)(5.0)(3.0)(3.0));
  declareProperty( "par2x", m_p2x = boost::assign::list_of (35.)(35.)(30.)(25.));
  declareProperty( "par3x", m_p3x = boost::assign::list_of (.03)(.05)(.08)(.10));
  declareProperty( "par1y", m_p1y = boost::assign::list_of (5.5)(4.0)(2.5)(2.5));
  declareProperty( "par2y", m_p2y = boost::assign::list_of (35.)(35.)(25.)(25.));
  declareProperty( "par3y", m_p3y = boost::assign::list_of (.07)(.17)(.17)(.22));

  declareInterface<ITrackFunctionTool>(this);
}
//=============================================================================
// Destructor
//=============================================================================
HltIsMuonTool::~HltIsMuonTool() {} 

//=============================================================================
//=============================================================================
// Initialization
//=============================================================================
StatusCode HltIsMuonTool::initialize() {
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
  m_iPosTool  = tool<IMuonPosTool>( "MuonPosTool" );
  if(!m_iPosTool)debug()<<"error retrieving the pos tool "<<endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================

double  HltIsMuonTool::function(const Track& ctrack)
{

  // A dirty trick!
  Track* track = (Track*) &ctrack;
  
  std::vector< LHCb::LHCbID >  lista=   track->lhcbIDs ();
  MuonTileID tileM5;
  MuonTileID tileM4;
  MuonTileID tileM3;
  MuonTileID tileM2;
  std::vector<LHCbID>::iterator it;
  bool inFOIM5=false;
  bool inFOIM2=false;
  bool muonSeg=false;

// looping over lhcbIDs 
  for(it=lista.end()-1;it>=lista.begin();it--){ 
    if(it->isMuon()){
      MuonTileID tile=it->muonID();
      if(tile.station()==4){
         muonSeg=true;
         tileM5=tile;
         // M5 hit out of FOI or invalid
         if(!(tileM5.isValid())||!(isInFOI(track,tileM5))) return 0;
      }else if(tile.station()==3){
         tileM4=tile;
         // M4 hit out of FOI or invalid
         if(!(tileM4.isValid())||!(isInFOI(track,tileM4))) return 0;
      }else if(tile.station()==2){
         tileM3=tile;
         // M3 hit out of FOI or invalid
         if(!(tileM3.isValid())||!(isInFOI(track,tileM3))) return 0;
      }else if(tile.station()==1){
         tileM2=tile;
         if(!(tileM2.isValid()))continue;
         // M2 hits inside FOI
         if(isInFOI(track,tileM2)) {inFOIM2=true; break;}
      }else{
         // All M2 hits outside FOI
         return 0;
      }
    }
  }
  if(!inFOIM2) return 0;
  if(muonSeg) return 1;
  //M4 and M5 for L0 candidates
  LHCb::MuonCoords* coords =  get<LHCb::MuonCoords>(LHCb::MuonCoordLocation::MuonCoords);
  if ( coords==0 ) {
    err() << " Cannot retrieve MuonCoords " << endreq;
    return StatusCode::FAILURE;
  }

  // loop over the coords starting from M5
  LHCb::MuonCoords::const_iterator iCoord;
  for ( iCoord = coords->end() -1 ; iCoord >= coords->begin() ; iCoord-- ){
    int region = (*iCoord)->key().region();
    int station = (*iCoord)->key().station();
    debug()<<station<<" "<<region<<endreq;
    //if(region!=0 && (*iCoord)->uncrossed())continue;
    if(station==4){
        if(inFOIM5) continue;
        // M5 hit found inside FOI
        if(isInFOI(track,(*iCoord)->key())) inFOIM5=true;
    }else if(station==3){
        // All M5 hits outside FOI
        if(!inFOIM5) return 0;
        // M2, M3, M4 and M5 hits inside FOI
        if(isInFOI(track,(*iCoord)->key())) return 1;
    }else{
        // All M4 hits outside FOI
        return 0;
    }
  }
// no muon hits
return 0;
} 

bool  HltIsMuonTool::isInFOI(Track* track, MuonTileID tileMX){
 
  State* stato=NULL;
  double x,y,z,dx,dy,dz;
  int region = tileMX.region();

  m_iPosTool-> calcTilePos(tileMX,x, dx,y, dy,z, dz);  
  stato=&(track->closestState(z));
  float xpoint=stato->x();
  float ypoint=stato->y();
  float zpoint=stato->z();
  float xslope=stato->slopes().x()/stato->slopes().z();
  float yslope=stato->slopes().y()/stato->slopes().z();
  float x_MX=xpoint+xslope*(z-zpoint);
  float y_MX=ypoint+yslope*(z-zpoint); 
  float ppoint=stato->p()/Gaudi::Units::GeV;
  float x_window = (m_p1x[region]+m_p2x[region]*exp(-m_p3x[region]*ppoint))*dx;
  float y_window = (m_p1y[region]+m_p2y[region]*exp(-m_p3y[region]*ppoint))*dy;

  debug()<<" station M"<< tileMX.station()+1 <<" "<< x_window <<" "<< x <<" "<< x_MX <<" "<< dx << endreq ;
  debug()<<"   region " <<          region+1 <<" "<< y_window <<" "<< y <<" "<< y_MX <<" "<< dy <<endreq;
  if((fabs(x_MX-x)>x_window)||(fabs(y_MX-y)>y_window))  { debug() << "bad " << endreq; return false;}
  debug() << "accepted" << endreq;
  return true;
}
