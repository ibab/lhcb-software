#include "HltBase/HltUtils.h"

using namespace Gaudi;
using namespace LHCb;

double HltUtils::rImpactParameter(const RecVertex& vertex, 
                                  const Track& track)
{
  const State& state = track.firstState();
  double r   = state.x();
  double phi = state.y();
  double tr  = state.tx();
  double z   = state.z();
  // std::cout << " state " << state << std::endl;
  
  const EPoint& p = vertex.position();
  double x = p.x();
  double y = p.y();
  double z0 = p.z();
  double r0 = x*cos(phi)+y*sin(phi);
  // std::cout << " point " << p << std::endl; 

  // computation
  double lambda = sqrt(1+tr*tr);
  double xhi = (z-z0)*tr-(r-r0);
  double ip = (lambda>0? fabs(xhi)/lambda : -1e6); 
  if (xhi<0) ip = -1.*ip;
  // std::cout << " rIP " << ip << std::endl;

  // EPoint ori(r,0.,z);
  // EVector dir(tr,0.,1.);
  // EPoint pos(r0,0,z0);
  // double ip2 = impactParameter(pos,ori,dir);
  // std::cout << " rIP2 " << ip2 << std::endl;

  return ip;
}


double HltUtils::impactParameter (const RecVertex& vertex, 
                                  const Track& track)
{
  const EPoint& pos = vertex.position();

  const State& state = track.firstState();
  EPoint ori(state.x(),state.y(),state.z());
  EVector dir(state.tx(),state.ty(),1.);
  
  return impactParameter(pos,ori,dir);

  //   double tx = state.tx();
  //   double ty = state.ty();
  //   double lambda = 1./sqrt(1.+tx*tx+ty*ty);
  
  
  //   double dx   = state.x() - p.x();
  //   double dy   = state.y() - p.y();
  //   double dz   = state.z() - p.z(); 
  
  //   double xx =  dy    - dz*ty;
  //   double yy = -dx    + dz*tx;
  //   double zz =  dx*ty - dy*tx;
  
  //   double ip = lambda*sqrt(xx*xx+yy*yy+zz*zz);
  
  //   if (zz<0.) ip = -1.*ip;
  //   return ip;
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
  EVector ipVector = udir.Cross(distance.Cross(udir));
  // EVector ipVector = trackDirection;
  return ipVector;
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
  EVector dis(0.,0.,0.);
  EPoint pos1(0.,0.,0.);
  EPoint pos2(0.,0.,0.);
  EVector dir1(state1.tx(),state1.ty(),1.);
  EVector dir2(state2.tx(),state2.ty(),1.);
  EPoint  ori1(state1.x(),state1.y(),state1.z());
  EPoint  ori2(state2.x(),state2.y(),state2.z());
  closestPoints(ori1,dir1,ori2,dir2,pos1,pos2);
  dis = pos1 - pos2;
  return dis;
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
  EPoint center(0.5*(pos1.x()+pos2.x()),
                0.5*(pos1.y()+pos2.y()),
                0.5*(pos1.z()+pos2.z()));
  //  std::cout << " HltUtils closestPoint " << center << std::endl;
  return center;
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
  double error = Constant +(LogFactor*(log(pt)+LogOffset))*(pow(pt,Exponent));
  return error;
}

double HltUtils::closestDistanceMod(const LHCb::Track& track1,
                                    const LHCb::Track& track2) {
  EVector dis = closestDistance(track1,track2);
  double dd = sqrt(dis.Dot(dis));
  return dd;
} 


double HltUtils::invariantMass(const LHCb::Track& track1,
                               const LHCb::Track& track2,
                               double mass1, double mass2) {
  Gaudi::XYZVector      p1=track1.momentum();  
  Gaudi::XYZVector      p2=track2.momentum();
  Gaudi::XYZVector pTot=p1+p2;
  double e1=sqrt(pow(mass1,2)+p1.mag2());
  double e2=sqrt(pow(mass2,2)+p2.mag2());
//std::cout<<" calcu mass "<<p1.mag()<<" "<<p2.mag()<<" "<<pTot.mag()<<" "<<e1<$
  double resultMass=sqrt(pow((e1+e2),2)-pTot.mag2());
  return resultMass;

}
