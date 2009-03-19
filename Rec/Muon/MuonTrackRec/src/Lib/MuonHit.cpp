// $Id: MuonHit.cpp,v 1.1.1.1 2009-03-19 14:38:47 ggiacomo Exp $
// Include files
#include "MuonTrackRec/MuonHit.h"
#include "MuonTrackRec/MuonLogHit.h"
#include "MuonTrackRec/MuonLogPad.h"
#include "MuonDet/IMuonPosTool.h"

MuonHit::MuonHit(  ):
  ROOT::Math::XYZPoint(),
  m_pid(0),
  m_mamy_pid(0),
  m_time(0),
  m_dtime(0)
{
}

//=============================================================================
// Constructor from a MuonLogPad
//=============================================================================
MuonHit::MuonHit( DeMuonDetector* md, 
                  MuonLogPad* mp,
                  const double x,
                  const double y,
                  const double z,
                  IMuonPosTool* posTool):
  ROOT::Math::XYZPoint(x,y,z),
  m_muonDet(md),
  m_pad(mp),
  m_tile(mp->tile()),
  m_pid(0),
  m_mamy_pid(0),
  m_time(0),
  m_dtime(0),
  m_posTool(posTool)
{
}

///=============================================================================
// Destructor
//=============================================================================
MuonHit::~MuonHit() {
} 

//=============================================================================
// public member functions
//
/// return tile sizes
std::vector<double> MuonHit::hitTile_Size()
{
  double x,dx,y,dy,z,dz;
  m_hit_size.clear();
  StatusCode sc = m_posTool->calcTilePos(*m_tile,x,dx,y,dy,z,dz);
  if(!sc){
    m_hit_size.clear();
    return m_hit_size;
  }
  m_hit_size.push_back(dx);
  m_hit_size.push_back(dy);
  m_hit_size.push_back(dz);
  return m_hit_size;
}

/// Store MC truth for this hit 
StatusCode MuonHit::setHitMCTruth(const LHCb::MCParticle* pp)
{
  if( NULL == pp ) return StatusCode::FAILURE;
  m_pid = pp->particleID().pid();

  if( NULL == pp->mother()) {
    m_mamy_pid = -999;
    return StatusCode::FAILURE;
  }

  m_mamy_pid = pp->mother()->particleID().pid();

  return StatusCode::SUCCESS;
}

/// return Hit PID
int MuonHit::hitPID()
{
  return m_pid;
}
/// return Hit mother PID
int MuonHit::hitMother()
{
  return m_mamy_pid;
}
double MuonHit::hitTile_dX()
{
  m_hit_dx=0;
  if( hitTile_Size().size() !=0 ) m_hit_dx = hitTile_Size().at(0);
  return m_hit_dx;
}
double MuonHit::hitTile_dY()
{
  m_hit_dy=0;
  if( hitTile_Size().size() !=0 ) m_hit_dy = hitTile_Size().at(1);
  return m_hit_dy;
}
double MuonHit::hitTile_dZ()
{
  m_hit_dz=0;
  if( hitTile_Size().size() !=0 ) m_hit_dz = hitTile_Size().at(2);
  return m_hit_dz;
}
/// store a progressive hit number for debugging
StatusCode MuonHit::setHitID(const int id)
{
  if( 0 == id) return StatusCode::FAILURE;
  m_hit_ID = id;
  return StatusCode::SUCCESS;
}
 /// retireve the hit ID
int MuonHit::hitID()
{
  return m_hit_ID;
}

/// return the hit station
int MuonHit::station()
{
  return m_tile->station();
}

 /// return the hit region
int MuonHit::region()
{
  return m_tile->region();
}

float MuonHit::hitTime()
{
  return m_pad->time();
}

float MuonHit::hitDeltaTime()
{
  return m_pad->dtime();
}


std::vector<MuonLogHit*> MuonHit::getHits()
{
  return m_pad->getHits();
}


const std::vector<LHCb::MuonTileID*> MuonHit::getTiles()
{
  std::vector<LHCb::MuonTileID*> tiles;
  std::vector<MuonLogHit*> hits = getHits();
  std::vector<MuonLogHit*>::iterator ih;
  for(ih = hits.begin(); ih != hits.end(); ih++){
    tiles.push_back((*ih)->tile());
  }
    return tiles;
}

std::vector<float> MuonHit::getTimes()
{
  std::vector<float> times;
  std::vector<MuonLogHit*> hits = getHits();
  std::vector<MuonLogHit*>::iterator ih;
  for(ih = hits.begin(); ih != hits.end(); ih++){
    times.push_back((*ih)->time());
  }
  
  return times;
}

const LHCb::MuonTileID* MuonHit::tile()
{
  return m_tile;
}

bool MuonHit::isTruePad()
{
  return m_pad->truepad();
}
