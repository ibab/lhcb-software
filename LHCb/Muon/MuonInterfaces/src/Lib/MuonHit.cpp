// $Id: MuonHit.cpp,v 1.3 2010-02-12 12:19:58 ggiacomo Exp $
// Include files
#include <cmath>
#include "MuonInterfaces/MuonHit.h"
#include "MuonInterfaces/MuonLogHit.h"
#include "MuonInterfaces/MuonLogPad.h"
#include "MuonDet/IMuonFastPosTool.h"

MuonHit::MuonHit():
  ROOT::Math::XYZPoint(),
  m_muonDet(NULL),
  m_pid(0),
  m_mamy_pid(0),
  m_hit_ID(-1),
  m_dx(0.), m_dy(0.), m_dz(0.),
  m_hit_minx(0.), m_hit_maxx(0.), 
  m_hit_miny(0.), m_hit_maxy(0.), 
  m_hit_minz(0.), m_hit_maxz(0.),
  m_xsize(0), m_ysize(0), m_zsize(0),
  m_time(0),  m_dtime(0),
  m_mintime(0.),m_maxtime(0.),
  m_posTool(NULL)
{
  m_pads.clear(); m_padx.clear(); m_pady.clear(); m_padz.clear();
}

MuonHit::MuonHit(DeMuonDetector* md,  IMuonFastPosTool* posTool):
  ROOT::Math::XYZPoint(),
  m_muonDet(md),
  m_pid(0),
  m_mamy_pid(0),
  m_hit_ID(-1),
  m_dx(0.), m_dy(0.), m_dz(0.),
  m_hit_minx(0.), m_hit_maxx(0.), 
  m_hit_miny(0.), m_hit_maxy(0.), 
  m_hit_minz(0.), m_hit_maxz(0.),
  m_xsize(0), m_ysize(0), m_zsize(0),
  m_time(0),  m_dtime(0),
  m_mintime(0.),m_maxtime(0.),
  m_posTool(posTool)
{
  m_pads.clear(); m_padx.clear(); m_pady.clear(); m_padz.clear();
}

//=============================================================================
// Constructor from a MuonLogPad
//=============================================================================
// obsolete constructor from a MuonPad (kept for backw. compat.)
MuonHit::MuonHit( DeMuonDetector* md, 
                  MuonLogPad* mp,
                  const double x,
                  const double y,
                  const double z,
                  IMuonFastPosTool* posTool):
  ROOT::Math::XYZPoint(x,y,z),
  m_muonDet(md),
  m_pid(0),
  m_mamy_pid(0),
  m_hit_ID(-1),
  m_xsize(0), m_ysize(0), m_zsize(0),
  m_posTool(posTool)
{
  createFromPad(mp);
}

MuonHit::MuonHit( DeMuonDetector* md, 
                  MuonLogPad* mp,
                  IMuonFastPosTool* posTool):
  ROOT::Math::XYZPoint(0.,0.,0.),
  m_muonDet(md),
  m_pid(0),
  m_mamy_pid(0),
  m_hit_ID(-1),
  m_xsize(0), m_ysize(0), m_zsize(0),
  m_posTool(posTool)
{
  createFromPad(mp);
}

void MuonHit::createFromPad(MuonLogPad* mp) {
  m_pads.clear();
  if (mp->type() == MuonLogPad::UNPAIRED) return;
  double x,dx,y,dy,z,dz;
  StatusCode sc = m_posTool->calcTilePos(*(mp->tile()),x,dx,y,dy,z,dz);
  if(sc) {
    m_pads.push_back(mp);
    m_padx.push_back(x);
    m_pady.push_back(y);
    m_padz.push_back(z);
    m_dx = dx/sqrt(12.);
    m_dy = dy/sqrt(12.);
    m_dz = dz/sqrt(12.);
    m_hit_minx=x-dx;
    m_hit_maxx=x+dx;
    m_hit_miny=y-dy;
    m_hit_maxy=y+dy;
    m_hit_minz=z-dz;
    m_hit_maxz=z+dz;
    SetXYZ(x,y,z);
    m_xsize=m_ysize=1;

    recomputeTime();
  }
}
///=============================================================================
// Destructor
//=============================================================================
MuonHit::~MuonHit() {
} 

//=============================================================================
// public member functions
//

void MuonHit::addPad(MuonLogPad* mp){
  if (mp->type() == MuonLogPad::UNPAIRED) return;
  if ( m_pads.empty() ) {
    createFromPad(mp);
  }
  else {
    double x,dx,y,dy,z,dz;
    StatusCode sc = m_posTool->calcTilePos(*(mp->tile()),x,dx,y,dy,z,dz);
    if(sc) {
      m_pads.push_back(mp);
      m_padx.push_back(x);
      m_pady.push_back(y);
      m_padz.push_back(z);
      if ((x-dx) < m_hit_minx) m_hit_minx = x-dx;
      if ((x+dx) > m_hit_maxx) m_hit_maxx = x+dx;
      if ((y-dy) < m_hit_miny) m_hit_miny = y-dy;
      if ((y+dy) > m_hit_maxy) m_hit_maxy = y+dy;
      if ((z-dz) < m_hit_minz) m_hit_minz = z-dz;
      if ((z+dz) > m_hit_maxz) m_hit_maxz = z+dz;
      recomputePos(&m_padx,&x,&m_dx,&m_xsize,dx);
      recomputePos(&m_pady,&y,&m_dy,&m_ysize,dy);
      recomputePos(&m_padz,&z,&m_dz,&m_zsize,10*dz);
      SetXYZ(x,y,z);

      recomputeTime();     
    }
  }
}

void MuonHit::recomputePos(std::vector<double> *data, 
                           double* pos, double* dpos,
                           int* clsize, double step) {
  int np=0;
  double sum=0.,sum2=0.;
  std::vector<double>::iterator ip,previp;
  for (ip=data->begin() ; ip != data->end(); ip++) {
    bool prendila=true;
    // check that this position is not already the same of a previous pad
    for (previp=data->begin() ; previp < ip; previp++) {
      if ( fabs((*ip)-(*previp))< 0.5*step) { 
        prendila=false;
        break;
      }
    }
    if (prendila) {
      np++;
      sum += (*ip);
      sum2 += (*ip)*(*ip);
    }
  }
  *pos = sum/np;
  if (np>1) {
    *dpos = sqrt( (sum2-sum*sum/np)/(np*np-np)  );
  }
  *clsize = np;
}

void MuonHit::recomputeTime() {
  int np=0;
  float sum=0.,sum2=0.;
  std::vector<float> times;
  m_mintime=999.;
  m_maxtime=-999.;
  for (m_ip=m_pads.begin() ; m_ip != m_pads.end(); m_ip++) {
    if( (*m_ip)->type() == MuonLogPad::XTWOFE) { //consider the two measurements as independent      
      times.push_back( (*m_ip)->timeX() );
      times.push_back( (*m_ip)->timeY() );
    }
    else {
      times.push_back( (*m_ip)->time() );
    }
  }
  for (std::vector<float>::iterator itime=times.begin() ; itime != times.end(); itime++) {
    sum += (*itime);
    sum2 += (*itime)*(*itime);
    np++;
    if ((*itime) < m_mintime) m_mintime=(*itime);
    if ((*itime) > m_maxtime) m_maxtime=(*itime);
  }

  m_time = sum/np;
  if (np>1) {
    m_dtime = sqrt( (sum2-sum*sum/np)/(np*np-np)  );
  }
  else {
    m_dtime = m_pads[0]->dtime();
  }
}


/// return tile sizes
std::vector<double> MuonHit::hitTile_Size()
{
  std::vector<double> hit_size;
  hit_size.push_back((m_hit_maxx-m_hit_minx)/2.);
  hit_size.push_back((m_hit_maxy-m_hit_miny)/2.);
  hit_size.push_back((m_hit_maxz-m_hit_minz)/2.);
  return hit_size;
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
  return (m_hit_maxx-m_hit_minx)/2.;
}
double MuonHit::hitTile_dY()
{
  return (m_hit_maxy-m_hit_miny)/2.;
}
double MuonHit::hitTile_dZ()
{
  return (m_hit_maxz-m_hit_minz)/2.;
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
  return ((m_pads.empty()) ? -1 : (int)m_pads[0]->tile()->station());
}

 /// return the hit region
int MuonHit::region()
{
  return ((m_pads.empty()) ? -1 : (int)m_pads[0]->tile()->region());
}



std::vector<MuonLogHit*> MuonHit::getHits()
{
  std::vector<MuonLogHit*> out;
  for (m_ip=m_pads.begin() ; m_ip != m_pads.end(); m_ip++) {
    std::vector<MuonLogHit*> padhits = (*m_ip)->getHits();
    out.insert(out.end(), padhits.begin(), padhits.end());
  }
  return out;
}


std::vector<LHCb::MuonTileID*> MuonHit::getTiles()
{
  std::vector<LHCb::MuonTileID*> tiles;
  std::vector<MuonLogHit*> hits = getHits();
  std::vector<MuonLogHit*>::iterator ih;
  for(ih = hits.begin(); ih != hits.end(); ih++){
    tiles.push_back((*ih)->tile());
  }
  return tiles;
}

std::vector<LHCb::MuonTileID*> MuonHit::getLogPadTiles() {
  std::vector<LHCb::MuonTileID*> tiles;
  for (m_ip=m_pads.begin() ; m_ip != m_pads.end(); m_ip++) {
    tiles.push_back(const_cast<LHCb::MuonTileID*>((*m_ip)->tile()));
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
  return ((m_pads.empty()) ? NULL : m_pads[0]->tile());
}

const LHCb::MuonTileID* MuonHit::centerTile()
{
  if (m_pads.size() == 1) {
    return m_pads[0]->tile();
  }
  double d2min=9999999.,d2;
  const LHCb::MuonTileID* out=NULL;
  for (unsigned int ip=0; ip<m_pads.size(); ip++) {
    d2 = (m_padx[ip]-X())*(m_padx[ip]-X()) +
      (m_pady[ip]-Y())*(m_pady[ip]-Y()) +
      (m_padz[ip]-Z())*(m_padz[ip]-Z());
    if (d2<d2min) {
      d2min = d2;
      out = m_pads[ip]->tile();
    }
  }
  if (out == NULL) {
    for (unsigned int ip=0; ip<m_pads.size(); ip++) {
      std::cout << m_padx[ip] << " " <<m_pady[ip]<< " " <<m_padz[ip]<<std::endl;
    }

  }
  return out;
}

bool MuonHit::isTruePad()
{
  return ((m_pads.empty()) ? false : m_pads[0]->truepad());
}
