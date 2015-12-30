// Include files
#include <cmath>
#include "MuonInterfaces/MuonHit.h"
#include "MuonInterfaces/MuonLogHit.h"
#include "MuonInterfaces/MuonLogPad.h"
#include "MuonDet/IMuonFastPosTool.h"

MuonHit::MuonHit() = default;

MuonHit::MuonHit(DeMuonDetector*,  IMuonFastPosTool* posTool):
  m_posTool(posTool)
{ }

//=============================================================================
// Constructor from a MuonLogPad
//=============================================================================
// obsolete constructor from a MuonPad (kept for backw. compat.)
MuonHit::MuonHit( DeMuonDetector*,
                  MuonLogPad* mp,
                  const double x,
                  const double y,
                  const double z,
                  IMuonFastPosTool* posTool):
  ROOT::Math::XYZPoint(x,y,z),
  m_posTool(posTool)
{
  createFromPad(mp);
}

MuonHit::MuonHit( DeMuonDetector*,
                  MuonLogPad* mp,
                  IMuonFastPosTool* posTool):
  ROOT::Math::XYZPoint(0.,0.,0.),
  m_posTool(posTool)
{
  createFromPad(mp);
}

void MuonHit::createFromPad(MuonLogPad* mp) {
  m_pads.clear();
  if (mp->type() == MuonLogPad::UNPAIRED) return;
  double x,dx,y,dy,z,dz;
  StatusCode sc = m_posTool->calcTilePos(mp->tile(),x,dx,y,dy,z,dz);
  if(sc) {
    m_pads.push_back(mp);
    m_padx.push_back(x);
    m_pady.push_back(y);
    m_padz.push_back(z);
    m_dx = dx/std::sqrt(12.);
    m_dy = dy/std::sqrt(12.);
    m_dz = dz/std::sqrt(12.);
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
MuonHit::~MuonHit() = default;

//=============================================================================
// public member functions
//

void MuonHit::addPad(MuonLogPad* mp){
  if (mp->type() == MuonLogPad::UNPAIRED) return;
  if ( m_pads.empty() ) {
    createFromPad(mp);
  } else {
    double x,dx,y,dy,z,dz;
    StatusCode sc = m_posTool->calcTilePos(mp->tile(),x,dx,y,dy,z,dz);
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
  for (auto ip=data->begin() ; ip != data->end(); ip++) {
    // check that this position is not already the same of a previous pad
    bool prendila= std::none_of( data->begin(), ip,
                                 [&](double p) {
                        return std::abs(*ip-p)<0.5*step;
    } );
    if (prendila) {
      ++np;
      sum  += (*ip);
      sum2 += (*ip)*(*ip);
    }
  }
  *pos = ( np>0 ? sum/np : 0.0 );
  if (np>1) {
    const float tmp = (sum2-sum*sum/np)/(np*np-np);
    *dpos = ( tmp>0 ? std::sqrt(tmp) : 0.0f );
  }
  *clsize = np;
}

void MuonHit::recomputeTime() {
  int np=0;
  float sum=0.,sum2=0.;
  std::vector<float> times;
  m_mintime=999.;
  m_maxtime=-999.;
  for (const auto& pad : m_pads )  {
    if( pad->type() == MuonLogPad::XTWOFE) { //consider the two measurements as independent
      times.push_back( pad->timeX() );
      times.push_back( pad->timeY() );
    } else {
      times.push_back( pad->time() );
    }
  }
  for (const auto& time : times) {
    sum += time;
    sum2 += time*time;
    np++;
    if (time < m_mintime) m_mintime=time;
    if (time > m_maxtime) m_maxtime=time;
  }

  m_time = ( np>0 ? sum/np : 0.0f );
  if (np>1) {
    const float tmp = (sum2-sum*sum/np)/(np*np-np);
    m_dtime = ( tmp>0 ? std::sqrt(tmp) : 0.0f );
  } else {
    m_dtime = m_pads[0]->dtime();
  }
}

/// return tile sizes
std::vector<double> MuonHit::hitTile_Size() const
{
  return { (m_hit_maxx-m_hit_minx)/2.,
           (m_hit_maxy-m_hit_miny)/2.,
           (m_hit_maxz-m_hit_minz)/2. };
}


/// return Hit PID
int MuonHit::hitPID() const
{
  return m_pid;
}
/// return Hit mother PID
int MuonHit::hitMother() const
{
  return m_mamy_pid;
}
double MuonHit::hitTile_dX() const
{
  return (m_hit_maxx-m_hit_minx)/2.;
}
double MuonHit::hitTile_dY() const
{
  return (m_hit_maxy-m_hit_miny)/2.;
}
double MuonHit::hitTile_dZ() const
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
int MuonHit::hitID() const
{
  return m_hit_ID;
}

/// return the hit station
int MuonHit::station() const
{
  return m_pads.empty() ? -1 : (int)m_pads[0]->tile().station();
}

 /// return the hit region
int MuonHit::region() const
{
  return m_pads.empty() ? -1 : (int)m_pads[0]->tile().region();
}

std::vector<const MuonLogHit*> MuonHit::getHits() const
{
  std::vector<const MuonLogHit*> out;
  for (const auto& pad : m_pads) {
    auto padhits = pad->getHits();
    out.insert(out.end(), padhits.begin(), padhits.end());
  }
  return out;
}


std::vector<LHCb::MuonTileID> MuonHit::getTiles() const
{
  std::vector<LHCb::MuonTileID> tiles;
  for (const auto& pad : m_pads) {
    auto padhits = pad->getHits();
    std::transform( padhits.begin(), padhits.end(),
                    std::back_inserter(tiles),
                    [](const MuonLogHit* mlh) {
        return mlh->tile();
    });
  }
  return tiles;
}

std::vector<LHCb::MuonTileID> MuonHit::getLogPadTiles() const
{
  std::vector<LHCb::MuonTileID> tiles; tiles.reserve(m_pads.size());
  std::transform( m_pads.begin(), m_pads.end(),
                  std::back_inserter(tiles),
                  [](const MuonLogPad* mlp) { return mlp->tile(); } );
  return tiles;
}

std::vector<float> MuonHit::getTimes() const
{
  auto hits = getHits();
  std::vector<float> times; times.reserve(hits.size());
  std::transform( hits.begin(), hits.end(),
                  std::back_inserter(times),
                  [](const MuonLogHit* mlh) { return mlh->time(); } );
  return times;
}

LHCb::MuonTileID MuonHit::tile() const
{
  return !m_pads.empty() ? m_pads[0]->tile() : LHCb::MuonTileID{};
}

LHCb::MuonTileID MuonHit::centerTile() const
{
  if (m_pads.size() == 1) {
    return m_pads[0]->tile();
  }
  double d2min=9999999.,d2;
  LHCb::MuonTileID out;
  for (unsigned int ip=0; ip<m_pads.size(); ip++) {
    d2 = (m_padx[ip]-X())*(m_padx[ip]-X()) +
      (m_pady[ip]-Y())*(m_pady[ip]-Y()) +
      (m_padz[ip]-Z())*(m_padz[ip]-Z());
    if (d2<d2min) {
      d2min = d2;
      out = m_pads[ip]->tile();
    }
  }
  if (!out.isDefined()) {
    for (unsigned int ip=0; ip<m_pads.size(); ip++) {
      std::cout << m_padx[ip] << " " <<m_pady[ip]<< " " <<m_padz[ip]<<std::endl;
    }
  }
  return out;
}

bool MuonHit::isTruePad() const
{
  return !m_pads.empty() && m_pads.front()->truepad();
}
