#include "HltBase/HltUtils.h"
#include "HltBase/ESequences.h"

using namespace Gaudi;
using namespace LHCb;

double HltUtils::rImpactParameter(const RecVertex& vertex, 
                                  const Track& track)
{
  const State& state = track.firstState();
  double rt  = state.x();
  double phi = state.y();
  double tr  = state.tx();
  double zt  = state.z();
  
  const EPoint& p = vertex.position();
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
  if (ZZ<0.) rIP = -1.*rIP;

  return rIP;
}


double HltUtils::impactParameter (const RecVertex& vertex, 
                                  const Track& track)
{
  const EPoint& pos = vertex.position();
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
  double ZZ = Dz+dz;
  
  double IP = sqrt(XX*XX+YY*YY+ZZ*ZZ);
  if (ZZ<0.) IP = -1.*IP;
  return IP;
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

double HltUtils::impactParameter(const EPoint& vertex,
                                 const EPoint& point,
                                 const EVector& direction)
{
  EVector ipVector = impactParameterVector(vertex,point,direction);
  double ip = sqrt(ipVector.mag2());
  if (ipVector.z() < 0) ip = -ip;
  return ip;
}


EVector HltUtils::impactParameterVector(const EPoint& vertex,
                                        const EPoint& point,
                                        const EVector& direction) {
  EVector udir = direction.unit();
  EVector distance = point - vertex;
  return udir.Cross(distance.Cross(udir));
}

bool HltUtils::closestPoints(const EPoint& ori1, const EVector& dir1,
                             const EPoint& ori2, const EVector& dir2,
                             EPoint& close1, EPoint& close2) {
  
  //TODO new SMatrix
  // Calculate the point between two tracks
  // (closest distance to both tracks)
  // code from Paul Bourke, 
  // http://astronomy.swin.edu.au/~pbourke/geometry/lineline3d/
 
  double eps(1.e-6);
  
  close1 = EPoint(0.,0.,0.);
  close2 = EPoint(0.,0.,0.);
  
  EVector udir1 = dir1.unit();
  EVector udir2 = dir2.unit();
  
  EPoint t1b = ori1;
  EPoint t2b = ori2;
  // EPoint t1e = ori1 + udir1;
  // EPoint t2e = ori2 + udir2;
  
  EVector v0 = ori1 - ori2;
  EVector v1 = udir1;
  if (fabs(v1.x())  < eps && fabs(v1.y())  < eps && fabs(v1.z())  < eps)
    return false;
  EVector v2 = udir2;
  if (fabs(v2.x())  < eps && fabs(v2.y())  < eps && fabs(v2.z())  < eps)
    return false;
  
  double d02 = v0.Dot(v2);
  double d21 = v2.Dot(v1);
  double d01 = v0.Dot(v1);
  double d22 = v2.Dot(v2);
  double d11 = v1.Dot(v1);
  
  // d02 = v0.x() * v2.x() + v0.y() * v2.y() + v0.z() * v2.z();
  // d21 = v2.x() * v1.x() + v2.y() * v1.y() + v2.z() * v1.z();
  // d01 = v0.x() * v1.x() + v0.y() * v1.y() + v0.z() * v1.z();
  // d22 = v2.x() * v2.x() + v2.y() * v2.y() + v2.z() * v2.z();
  // d11 = v1.x() * v1.x() + v1.y() * v1.y() + v1.z() * v1.z();
  
  double denom = d11 * d22 - d21 * d21;
  if (fabs(denom) < eps) return false;
  double numer = d02 * d21 - d01 * d22;
  
  double mu1 = numer / denom;
  double mu2 = (d02 + d21 * mu1) / d22;
  
  close1 = t1b + mu1 * v1;  
  close2 = t2b + mu2 * v2;
  
  // std::cout << " HltUtils closest points (1) pos " << ori1
  // 	    << " dir " << udir1 << std::endl;
  //   std::cout << " HltUtils closest points (2) pos " << ori2
  // 	    << " dir " << udir2 << std::endl;
  //   std::cout << " HltUtils closest points [1-2] "
  // 	    << close1 << " : " << close2 << std::endl;
  
  return true;
}

EVector HltUtils::closestDistance(const Track& track1, 
                                  const Track& track2) {
  const State& state1 = track1.firstState();
  const State& state2 = track2.firstState();
  EPoint pos1(0.,0.,0.);
  EPoint pos2(0.,0.,0.);
  EVector dir1(state1.tx(),state1.ty(),1.);
  EVector dir2(state2.tx(),state2.ty(),1.);
  EPoint  ori1(state1.x(),state1.y(),state1.z());
  EPoint  ori2(state2.x(),state2.y(),state2.z());
  bool ok = closestPoints(ori1,dir1,ori2,dir2,pos1,pos2);
  if (!ok) return EVector(0.,0.,1.e6);
  return pos1 - pos2;
}

EPoint HltUtils::closestPoint(const Track& track1, 
                              const Track& track2) {
  const State& state1 = track1.firstState();
  const State& state2 = track2.firstState();
  
  EPoint pos1(0.,0.,0.);
  EPoint pos2(0.,0.,0.);
  EVector dir1(state1.tx(),state1.ty(),1.);
  EVector dir2(state2.tx(),state2.ty(),1.);
  EPoint  ori1(state1.x(),state1.y(),state1.z());
  EPoint  ori2(state2.x(),state2.y(),state2.z());
  closestPoints(ori1,dir1,ori2,dir2,pos1,pos2);
  return EPoint(0.5*(pos1.x()+pos2.x()),
                0.5*(pos1.y()+pos2.y()),
                0.5*(pos1.z()+pos2.z()));
}

RecVertex* HltUtils::newRecVertex(const Track& t1, const Track& t2) 
{
  
  EPoint x1=t1.position();
  EVector p1=t1.momentum();
  EPoint x2=t2.position();
  EVector p2=t2.momentum();
  RecVertex* sv = new RecVertex();
  sv->addToTracks(&t1);
  sv->addToTracks(&t2);
  
  double tol = 1.e-13;

  p1=p1.unit();
  p2=p2.unit();
  
  EVector v0=x1-x2;
  double d02=v0.Dot(p2);
  double d21=p2.Dot(p1);
  double d01=v0.Dot(p1);
  double d22=p2.Dot(p2);
  double d11=p1.Dot(p1);
  
  double den=d11*d22-d21*d21;
  if (fabs(den)<tol) {
    sv->setPosition(EPoint(0,0,-8000));
    return sv;
  }
  double num=d02*d21-d01*d22;
  double mu1=num/den;
  double mu2=(d02+d21*mu1)/d22;
  // EPoint sv1=(x1+mu1*p1.Scale3D(mu1));
  // EPoint sv2=(x2+mu2*p2.Scale3D(mu2));
  EPoint sv1=(x1+mu1*p1);
  EPoint sv2=(x2+mu2*p2);
  EPoint center(0.5*(sv1.x()+sv2.x()),
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

  EVector dir(dx,dy,dz);
  EVector p1 = track1.momentum();
  EVector p2 = track2.momentum();
  EVector p = p1+p2;

  double dd = sqrt(dir.Dot(dir));
  double pp = sqrt(p.Dot(p));  
  double ctheta = dir.Dot(p)/(dd*pp);
  double stheta = sqrt(1.-ctheta*ctheta);
  
  double ptp = pp*stheta;
  double pt = track1.pt() + track2.pt();
  
  double fc = ptp/(pt+ptp);
  // std::cout << " dd  " << dd <<std::endl; 
  // std::cout << " pp  " << pp <<std::endl; 
  // std::cout << " ptp " << ptp <<std::endl; 
  // std::cout << " pt  " << pt <<std::endl; 
  // std::cout << " pt1,pt2  " << track1.pt() << " " << track2.pt() <<std::endl; 
  // std::cout << " fc  " << fc <<std::endl; 
  return fc;
}

double HltUtils::FC2(const LHCb::RecVertex& vertex1, 
                     const LHCb::RecVertex& vertex2) 
{
  double dx =  vertex1.position().x()-vertex2.position().x();
  double dy =  vertex1.position().y()-vertex2.position().y();
  double dz =  vertex1.position().z()-vertex2.position().z();

  const Track& track1 = *(vertex1.tracks()[0]);
  const Track& track2 = *(vertex1.tracks()[1]);

  EVector dir(dx,dy,dz);
  EVector udir = dir.unit();
  EVector p1 = track1.momentum();
  EVector p2 = track2.momentum();
  EVector p = p1+p2;

  EVector vv = udir.Cross(p);
  EVector vdir = vv.unit();
  EVector wdir = udir.Cross(vdir);

  double ptPerp = fabs(p.Dot(wdir));
  double pt     = track1.pt() + track2.pt();
  double fc = ptPerp/(ptPerp+pt);


  // stdout << " [2] ptp " << ptPerp <<std::endl;
  // std::cout << " [2] pt  " << pt <<std::endl; 
  // std::cout << " [2] wdir " << wdir << std::endl;
  // std::cout << " [2] slope1 " << track1.slopes() << std::endl;
  // std::cout << " [2] slope2 " << track2.slopes() << std::endl;
  // std::cout << " [2] pt1,pt2  " << track1.pt() 
  //           << " " << track2.pt() <<std::endl;
  // std::cout << " [2] fc  " << fc <<std::endl; 
  return fc;
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
  EVector dis = closestDistance(track1,track2);
  return sqrt(dis.Dot(dis));
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

double HltUtils::matchIDsFraction(const LHCb::Track& track1,
                                  const LHCb::Track& track2) {
  size_t n0 = track1.lhcbIDs().size();
  if (n0 <=0) return false;
  size_t n  = ELoop::count(track1.lhcbIDs(),track2.lhcbIDs());
  return double(n)/double(n0);
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
  double deta = deltaEta(track1,track2);
  double dphi = deltaPhi(track1,track2);
  return sqrt ( deta*deta + dphi*dphi );
}

double HltUtils::minPT(const LHCb::RecVertex& vertex) {
  const SmartRefVector<LHCb::Track>& tracks = vertex.tracks();
  double pt = 1e12;
  for (SmartRefVector<LHCb::Track>::const_iterator it = tracks.begin();
       it != tracks.end(); it++)
    if ((*it)->pt() < pt) pt = (*it)->pt();
  return pt;
}

double HltUtils::maxPT(const LHCb::RecVertex& vertex) {
  const SmartRefVector<LHCb::Track>& tracks = vertex.tracks();
  double pt = -1e12;
  for (SmartRefVector<LHCb::Track>::const_iterator it = tracks.begin();
       it != tracks.end(); it++)
    if ((*it)->pt() > pt) pt = (*it)->pt();
  return pt;
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
    typedef std::vector<LHCb::CaloCellID>::const_iterator iter;
	for (iter on = oncells.begin(); on != oncells.end(); ++on) {
        if (std::find(offcells.begin(),
                      offcells.end(),
                      *on)  != offcells.end() ) return true;
    }
	return false;				
}																			 

