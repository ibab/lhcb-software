#include "HltBase/HltUtils.h"
#include "ESequences.h"

#include <cassert>

using namespace Gaudi;
using namespace LHCb;


void Hlt::TrackMerge(const LHCb::Track& track, LHCb::Track& otrack) {

  
  // setting ancestors
  otrack.addToAncestors(track);

  // adding info
  const GaudiUtils::VectorMap<int,double>& info = track.extraInfo();
  GaudiUtils::VectorMap<int,double>::const_iterator it = info.begin();
  for (; it != info.end(); ++it)
    otrack.addInfo(it->first,it->second);
  
  // adding states
  const std::vector<LHCb::State*>& states = track.states();
  for (std::vector<LHCb::State*>::const_iterator it = states.begin();
       it != states.end(); ++it) 
    otrack.addToStates(**it);
  
  // adding ids
  const std::vector<LHCb::LHCbID>& ids = track.lhcbIDs();
  for (std::vector<LHCb::LHCbID>::const_iterator it = ids.begin();
       it != ids.end(); ++it) 
    otrack.addToLhcbIDs(*it);
  

}



void Hlt::VertexCreator::operator() 
  (const LHCb::Track& track1, const LHCb::Track& track2,
   LHCb::RecVertex& ver) const {
  // XYZVector dis = HltUtils::closestDistance(track1,track2);
  ver.setPosition(HltUtils::closestPoint(track1,track2));
  ver.addToTracks(const_cast<Track*>(&track1));
  ver.addToTracks(const_cast<Track*>(&track2));
  // std::cout << " vertex position " << pos << std::endl;
}


double HltUtils::rImpactParameter(const RecVertex& vertex, 
                                  const Track& track)
{
  const State& state = track.firstState();
  double rt  = state.x();
  double phi = state.y();
  double tr  = state.tx();
  double zt  = state.z();
  
  const XYZPoint& p = vertex.position();
  double xv = p.x();
  double yv = p.y();
  double zv = p.z();

  rt = rt+tr*(zv-zt);
  zt = zv;
  double rv = xv*cos(phi)+yv*sin(phi);

  double Dr = rt-rv;
  double Dz = zt-zv;
  double dz = -(Dr*tr+Dz)/(1+tr*tr);
  double RR = Dr+tr*dz;
  double ZZ = Dz+dz;
  double rIP = sqrt(RR*RR+ZZ*ZZ);
  return (ZZ<0)? -rIP : rIP;
}


double HltUtils::impactParameter (const RecVertex& vertex, 
                                  const Track& track)
{
  const XYZPoint& pos = vertex.position();
  double xv = pos.x();
  double yv = pos.y();
  double zv = pos.z();
  
  const State& state = track.firstState();
  double xt = state.x();
  double yt = state.y();
  double zt = state.z();
  double tx = state.tx();
  double ty = state.ty();

  xt = xt + tx*(zv-zt);
  yt = yt + ty*(zv-zt);
  zt = zv;
  
  double Dx = xt-xv;
  double Dy = yt-yv;
  double Dz = zt-zv;
  
  double dz = -(Dx*tx+Dy*ty+Dz)/(1+tx*tx+ty*ty);
  double XX = Dx+tx*dz;
  double YY = Dy+ty*dz;
  double ZZ = Dz+   dz;
  
  double IP = sqrt(XX*XX+YY*YY+ZZ*ZZ);
  return (ZZ<0) ? -IP : IP;
}

double HltUtils::IPError(const Track& track)
{
  
  double invPt = 1. / track.firstState().pt();
  
  // units in mm
  double m_par0x = 0.0214;
  double m_par1x = -0.000801;
  double m_par2x = 0.0108;
  double m_par3x = -0.00122;
  double m_par4x = 0.0000547;

  double ipErr = m_par0x + m_par1x*invPt + m_par2x*pow(invPt,2) 
    +  m_par3x*pow(invPt,3) + m_par4x*pow(invPt,4);

  return ipErr;
  
}

double HltUtils::impactParameter(const XYZPoint& vertex,
                                 const XYZPoint& point,
                                 const XYZVector& direction)
{
  XYZVector ipVector = impactParameterVector(vertex,point,direction);
  return (ipVector.z() < 0) ? -ipVector.R() : ipVector.R();
}


XYZVector HltUtils::impactParameterVector(const XYZPoint& vertex,
                                        const XYZPoint& point,
                                        const XYZVector& direction) {
  XYZVector udir = direction.unit();
  XYZVector distance = point - vertex;
  return udir.Cross(distance.Cross(udir));
}

bool HltUtils::closestPoints(const XYZPoint& ori1, const XYZVector& dir1,
                             const XYZPoint& ori2, const XYZVector& dir2,
                             XYZPoint& close1, XYZPoint& close2) {
  
  //TODO new SMatrix
  // Calculate the point between two tracks
  // (closest distance to both tracks)
  // code from Paul Bourke, 
  // http://astronomy.swin.edu.au/~pbourke/geometry/lineline3d/
 
  double eps(1.e-6);
  
  close1 = XYZPoint(0.,0.,0.);
  close2 = XYZPoint(0.,0.,0.);
  
  XYZVector v0 = ori1 - ori2;
  XYZVector v1 = dir1.unit();
  if (fabs(v1.x())  < eps && fabs(v1.y())  < eps && fabs(v1.z())  < eps)
    return false;
  XYZVector v2 = dir2.unit();
  if (fabs(v2.x())  < eps && fabs(v2.y())  < eps && fabs(v2.z())  < eps)
    return false;
  
  double d02 = v0.Dot(v2);
  double d21 = v2.Dot(v1);
  double d01 = v0.Dot(v1);
  double d22 = v2.Dot(v2);
  double d11 = v1.Dot(v1);
  
  double denom = d11 * d22 - d21 * d21;
  if (fabs(denom) < eps) return false;
  
  double mu1 = (d02*d21 - d01*d22) / denom;
  double mu2 = (d02 + d21 * mu1) / d22;
  
  close1 = ori1 + mu1 * v1;  
  close2 = ori2 + mu2 * v2;
  
  return true;
}

XYZVector HltUtils::closestDistance(const Track& track1, 
                                  const Track& track2) {
  const State& state1 = track1.firstState();
  const State& state2 = track2.firstState();
  XYZPoint pos1(0.,0.,0.);
  XYZPoint pos2(0.,0.,0.);
  XYZVector dir1(state1.tx(),state1.ty(),1.);
  XYZVector dir2(state2.tx(),state2.ty(),1.);
  XYZPoint  ori1(state1.x(),state1.y(),state1.z());
  XYZPoint  ori2(state2.x(),state2.y(),state2.z());
  bool ok = closestPoints(ori1,dir1,ori2,dir2,pos1,pos2);
  if (!ok) return XYZVector(0.,0.,1.e6);
  return pos1 - pos2;
}

XYZPoint HltUtils::closestPoint(const Track& track1, 
                              const Track& track2) {
  const State& state1 = track1.firstState();
  const State& state2 = track2.firstState();
  
  XYZPoint pos1(0.,0.,0.);
  XYZPoint pos2(0.,0.,0.);
  XYZVector dir1(state1.tx(),state1.ty(),1.);
  XYZVector dir2(state2.tx(),state2.ty(),1.);
  XYZPoint  ori1(state1.x(),state1.y(),state1.z());
  XYZPoint  ori2(state2.x(),state2.y(),state2.z());
  closestPoints(ori1,dir1,ori2,dir2,pos1,pos2);
  return XYZPoint(0.5*(pos1.x()+pos2.x()),
                0.5*(pos1.y()+pos2.y()),
                0.5*(pos1.z()+pos2.z()));
}

RecVertex* HltUtils::newRecVertex(const Track& t1, const Track& t2) 
{
  XYZPoint x1=t1.position();
  XYZVector p1=t1.momentum();
  XYZPoint x2=t2.position();
  XYZVector p2=t2.momentum();

  RecVertex* sv = new RecVertex();
  sv->addToTracks(&t1);
  sv->addToTracks(&t2);
  
  double tol = 1.e-13;

  p1=p1.unit();
  p2=p2.unit();
  
  XYZVector v0=x1-x2;
  double d02=v0.Dot(p2);
  double d21=p2.Dot(p1);
  double d01=v0.Dot(p1);
  double d22=p2.Dot(p2);
  double d11=p1.Dot(p1);
  
  double den=d11*d22-d21*d21;
  if (fabs(den)<tol) {
    sv->setPosition(XYZPoint(0,0,-8000));
    return sv;
  }
  double num=d02*d21-d01*d22;
  double mu1=num/den;
  double mu2=(d02+d21*mu1)/d22;
  XYZPoint sv1=(x1+mu1*p1);
  XYZPoint sv2=(x2+mu2*p2);
  XYZPoint center(0.5*(sv1.x()+sv2.x()),
                0.5*(sv1.y()+sv2.y()),
                0.5*(sv1.z()+sv2.z()));
  sv->setPosition(center);
  
  v0=sv->position()-sv1;
  d11=v0.Dot(v0);
  sv->setChi2(d11);
  return sv;
}


double HltUtils::FC(const LHCb::RecVertex& vertex1, 
                    const LHCb::RecVertex& vertex2) 
{
  double dx =  vertex1.position().x()-vertex2.position().x();
  double dy =  vertex1.position().y()-vertex2.position().y();
  double dz =  vertex1.position().z()-vertex2.position().z();

  const Track& track1 = *(vertex1.tracks()[0]);
  const Track& track2 = *(vertex1.tracks()[1]);

  XYZVector dir(dx,dy,dz);
  XYZVector p1 = track1.momentum();
  XYZVector p2 = track2.momentum();
  XYZVector p = p1+p2;

  double dd = sqrt(dir.Dot(dir));
  double pp = sqrt(p.Dot(p));  
  double ctheta = dir.Dot(p)/(dd*pp);
  double stheta = sqrt(1.-ctheta*ctheta);
  
  double ptp = pp*stheta;
  double pt = track1.pt() + track2.pt();
  
  return ptp/(pt+ptp);
}

double HltUtils::FC2(const LHCb::RecVertex& vertex1, 
                     const LHCb::RecVertex& vertex2) 
{
  assert(vertex1.tracks().size()>1);
  const Track& track1 = *(vertex1.tracks()[0]);
  const Track& track2 = *(vertex1.tracks()[1]);

  XYZVector p = track1.momentum() + track2.momentum();
  XYZVector udir = (vertex1.position() - vertex2.position()).unit();
  XYZVector wdir = udir.Cross(udir.Cross(p.unit()));

  double ptPerp = fabs(p.Dot(wdir));
  double pt     = track1.pt() + track2.pt();
  return  ptPerp/(ptPerp+pt);
}


double HltUtils::impactParameterError(double pt0) 
{
  double pt = fabs(pt0);  

  double  Constant  = 0.0539;
  double  LogFactor = 90.41;
  double  LogOffset = -0.655;
  double  Exponent  = -1.498;
  return Constant +(LogFactor*(log(pt)+LogOffset))*(pow(pt,Exponent));
}

double HltUtils::closestDistanceMod(const LHCb::Track& track1,
                                    const LHCb::Track& track2) {
  return closestDistance(track1,track2).R();
} 


double HltUtils::invariantMass(const LHCb::Track& track1,
                               const LHCb::Track& track2,
                               double mass1, double mass2) {
  //TODO: is this the numerically most stable way of computing 
  //      invariant masses?? (esp. if mass^2 << mom.mag2 ! )
  double e = sqrt(mass1*mass1+track1.momentum().mag2())
           + sqrt(mass2*mass2+track2.momentum().mag2());
  return sqrt(e*e-(track1.momentum()+track2.momentum()).mag2());

}

double HltUtils::matchIDsFraction(const LHCb::Track& tref,
                                  const LHCb::Track& track) {
  // TODO: can we use std::set_intersection on lhcbIDs? 
  //       i.e. are they sorted?
  return (tref.lhcbIDs().empty()) ? 0. :
             double(zen::count(tref.lhcbIDs(),track.lhcbIDs())) / 
             double(tref.lhcbIDs().size());
}

double HltUtils::vertexMatchIDsFraction(const LHCb::RecVertex& vref,
                                        const LHCb::RecVertex& v) 
{
  assert(vref.tracks().size()==2);
  const LHCb::Track& rtrack1 = *(vref.tracks()[0]);
  const LHCb::Track& rtrack2 = *(vref.tracks()[1]);
  
  assert(v.tracks().size()==2);
  const LHCb::Track& track1 = *(v.tracks()[0]);
  const LHCb::Track& track2 = *(v.tracks()[1]);
  
  double r11 = HltUtils::matchIDsFraction(rtrack1,track1);
  double r12 = HltUtils::matchIDsFraction(rtrack1,track2);

  double r21 = HltUtils::matchIDsFraction(rtrack2,track1);
  double r22 = HltUtils::matchIDsFraction(rtrack2,track2);

  double f11 = r11+r22;
  double f12 = r12+r21;

  double f = f11>f12? f11 : f12;
  
  // std::cout << " vmatching r11 r22 " <<r11<<", "<<r22 <<std::endl;
//   std::cout << "           r12 r21 f " << r12 <<", " << r21 
//             << " -> " << f << std::endl;
  
  return f;
}


double HltUtils::deltaEta(const LHCb::Track& track1, 
                          const LHCb::Track& track2) {
  return track2.slopes().Eta() - track1.slopes().Eta();
}


double HltUtils::deltaPhi(const LHCb::Track& track1, 
                          const LHCb::Track& track2) {
  
  return track2.slopes().Phi() -  track1.slopes().Phi();;
}


double HltUtils::deltaAngle(const LHCb::Track& track1, 
                            const LHCb::Track& track2) {
  return acos( track1.slopes().Unit().Dot(  track2.slopes().Unit()  ) );
}

namespace {
struct maxPT {
    double operator()(double init,const LHCb::Track* t) { return std::max(init, t->pt()); }
};
struct minPT {
    double operator()(double init,const LHCb::Track* t) { return std::min(init, t->pt()); }
};
};

double HltUtils::VertexMinPT(const LHCb::RecVertex& vertex) {
  const SmartRefVector<LHCb::Track>& tracks = vertex.tracks();
  return std::accumulate(tracks.begin(),tracks.end(),1e12,minPT());
}

double HltUtils::VertexMaxPT(const LHCb::RecVertex& vertex) {
  const SmartRefVector<LHCb::Track>& tracks = vertex.tracks();
  return std::accumulate(tracks.begin(),tracks.end(),-1e12,maxPT());
}

namespace {
    struct isMuonStation {
    public:
       isMuonStation(unsigned int station) : m_station(station) {}
       bool operator()(const LHCbID& id)  const { 
           return (id.isMuon() && ( id.muonID().station()==m_station));
       }
    private:
       unsigned int m_station;
    };
}

bool HltUtils::doShareM3(const Track& t1, 
                         const Track& t2){

  const std::vector<LHCbID>& l1 = t1.lhcbIDs();
  const std::vector<LHCbID>& l2 = t2.lhcbIDs ();
  // TODO: check whether it makes sense to always do the
  //       shortest list first...
  // if (l1.size()>l2.size()) return doShareM3(t2,t1);
  std::vector<LHCbID>::const_iterator i1 = find_if(l1.begin(),
                                                   l1.end(),
                                                   isMuonStation(2));
  if (i1==l1.end()) return false;

  std::vector<LHCbID>::const_iterator i2 = find_if(l2.begin(),
                                                   l2.end(),
                                                   isMuonStation(2));

  if (i2==l2.end()) return false;

  std::vector<LHCbID>::const_iterator i3 = find_if(l1.begin(),
                                                   l1.end(),
                                                   isMuonStation(1));
  if (i3==l1.end()) return false;

  std::vector<LHCbID>::const_iterator i4 = find_if(l2.begin(),
                                                   l2.end(),
                                                   isMuonStation(1));

  return (i4==l2.end()) ?  false :  *i1 == *i2 && *i3==*i4;

}

//=============================================================================
// get3x3CellIDs
//=============================================================================
std::vector<LHCb::CaloCellID> HltUtils::get3x3CellIDs( const LHCb::CaloCellID& centercell )
{

	unsigned int calo = centercell.calo(),
  			     area = centercell.area(),
	 			 row = centercell.row(),
				 col = centercell.col();
	
    unsigned rowminus = ( row == 0 ? 0 : row - 1 );
	unsigned colminus = ( col == 0 ? 0 : col - 1 );

	std::vector<LHCb::CaloCellID> cells; cells.reserve(8);
	cells.push_back(CaloCellID( calo, area, row+1, colminus )) ;
	cells.push_back(CaloCellID( calo, area, row+1, col ));
	cells.push_back(CaloCellID( calo, area, row+1, col+1 ));
	cells.push_back(CaloCellID( calo, area, row,   col+1 )); 
	cells.push_back(CaloCellID( calo, area, rowminus, col+1 ));
	cells.push_back(CaloCellID( calo, area, rowminus, col ));
	cells.push_back(CaloCellID( calo, area, rowminus, colminus ));
	cells.push_back(CaloCellID( calo, area, row, colminus ));
	return cells;

}
//=============================================================================
// get2x2CellIDs
//=============================================================================
std::vector<LHCb::CaloCellID> HltUtils::get2x2CellIDs( const LHCb::CaloCellID& bottomleftcell )
{

	unsigned int calo = bottomleftcell.calo(),
			     area = bottomleftcell.area(),
				 row = bottomleftcell.row(),
				 col = bottomleftcell.col();
	
	std::vector<LHCb::CaloCellID> cells; cells.reserve(3);
	cells.push_back( CaloCellID( calo, area, row+1, col )) ;
	cells.push_back( CaloCellID( calo, area, row+1, col+1 ) );
	cells.push_back( CaloCellID( calo, area, row, col+1 ));	
	return cells;
}

//=============================================================================
// matchCellIDs
//=============================================================================
bool HltUtils::matchCellIDs( const std::vector<LHCb::CaloCellID>& oncells, 
						 	 const std::vector<LHCb::CaloCellID>& offcells )
{
    return std::find_first_of( oncells.begin(),oncells.end(),
                               offcells.begin(),offcells.end() ) 
           != oncells.end() ;
}																			 
