// $Id: HltDeltaPTool.cpp,v 1.1 2008-10-01 09:32:53 leandrop Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

//boost
#include <boost/assign/list_of.hpp>

// local
#include "HltDeltaPTool.h"
#include "Event/MuonCoord.h"

//-----------------------------------------------------------------------------
// Implementation file for class :HltDeltaPTool
//
// 2008-09-25 : Leandro de Paula
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( HltDeltaPTool );

using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltDeltaPTool::HltDeltaPTool( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  // DeltaP parameters
  declareProperty( "ptrackcut", m_pCut = boost::assign::list_of (60000.)(30000.)(15000.)(7500.)(-2.));
  declareInterface<ITrackFunctionTool>(this);
}
//=============================================================================
// Destructor
//=============================================================================
HltDeltaPTool::~HltDeltaPTool() {} 

//=============================================================================
//=============================================================================
// Initialization
//=============================================================================
StatusCode HltDeltaPTool::initialize() {
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if (msgLevel (MSG::DEBUG)) debug() << "==> Initialize" << endmsg;
  m_iPosTool  = tool<IMuonPosTool>( "MuonPosTool" );
  if(!m_iPosTool)if (msgLevel (MSG::DEBUG)) debug()<<"error retrieving the pos tool "<<endreq;
  m_muonBuffer=tool<IMuonRawBuffer>("MuonRawBuffer");
  if(!m_muonBuffer)info()<<"error retrieving the decoding tool "<<endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================

double  HltDeltaPTool::function(const Track& ctrack)
{
  
  // A dirty trick!
  Track* track = (Track*) &ctrack;
  
  std::vector< LHCb::LHCbID >  lista=   track->lhcbIDs ();
  MuonTileID tile, tileM;
  std::vector<LHCbID>::iterator it;
  bool L0Candidate=true;
  float ptrack, pMu, dpop;
  pMu = 0;
  ptrack=ctrack.p(); 
  // looping over lhcbIDs 
  for(it=lista.end()-1;it>=lista.begin();it--){ 
    if(it->isMuon()){
      MuonTileID tile=it->muonID();
      if(tile.station()==4){
        tileM = tile;
        pMu =  mSMomentum(tileM);
        L0Candidate=false;
      }else if(L0Candidate && tile.station()==2){
        tileM = tile;
        pMu = l0Momentum(tileM);
      }
      if(pMu>0){
        // Checking dP/P
        dpop = (ptrack-pMu)/ptrack;
        if (msgLevel (MSG::DEBUG)) debug() << " pTt = " << ptrack << "  pMu = " << pMu << " dpop = " << dpop << endreq;
        if(dpop < m_pCut[4]){
          int region = tileM.region();
          if(ptrack < m_pCut[region]) return 0;;
        }
        return 1;
      }
    }
  }
 // if(!L0Candidate) return 1;
  // no muon hits
  return 0;
} 
//=====================================
float  HltDeltaPTool::l0Momentum(MuonTileID tileMX){
  float pMu;
  m_inputL0Muon=get<L0MuonCandidates>(L0MuonCandidateLocation::Default);
  std::vector<L0MuonCandidate*>::const_iterator itL0;
  for (itL0=m_inputL0Muon->begin();itL0!=m_inputL0Muon->end();itL0++){
     std::vector<MuonTileID> list_of_tile= (*itL0)->muonTileIDs(2);
     MuonTileID l0tileM3=*(list_of_tile.begin());
     if(l0tileM3==tileMX){
        return pMu = fabs((*itL0)->pt())/sin((*itL0)->theta());
     }
  }
  return 0;
} 
//=====================================
float  HltDeltaPTool::mSMomentum(MuonTileID tileMX){
  float pMu;
  std::vector<LHCbID>::iterator it1;
  m_inputMuSeg=get<LHCb::Tracks>("Hlt/Track/MuonSegmentForL0Single");
  for (LHCb::Tracks::const_iterator itMS = m_inputMuSeg->begin();itMS !=m_inputMuSeg->end();itMS++){
     std::vector< LHCb::LHCbID >  lista1=   (*itMS)->lhcbIDs ();
     for(it1=lista1.end()-1;it1>=lista1.begin();it1--){ 
       if(it1->isMuon()){
         MuonTileID tileMS=it1->muonID();
         if(tileMS.station()==4){
           if(tileMS==tileMX){
             debug() << " pt " << (*itMS)->pt() << endmsg;
             return pMu = fabs((*itMS)->p());
           }
         }
       }
     }
  }
  return 0;
} 

