#include "HltUtils.h"

EVector HltUtils::impactParameterVector(const EPoint& vertex,
                                        const EPoint& point,
                                        const EVector& direction) {
  EVector trackDirection = direction.unit();
  EVector distance = point - vertex;
  EVector ipVector = trackDirection.cross(distance.cross(trackDirection));
  return ipVector;
}

double HltUtils::impactParameter(const EPoint& vertex,
                                  const EPoint& point,
                                  const EVector& direction)
{
  EVector ipVector = impactParameterVector(vertex,point,direction);
  double ip = ipVector.mag();
  if (ipVector.z() < 0) ip = -ip;
  return ip;
}


bool HltUtils::closestPoints(const EPoint& ori1, const EVector& dir1,
                              const EPoint& ori2, const EVector& dir2,
                              EPoint& close1, EPoint& close2) {
  
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
  EPoint t1e = ori1 + udir1;
  EPoint t2e = ori2 + udir2;
  
  EVector v0 = ori1 - ori2;
  EVector v1 = udir1;
  if (fabs(v1.x())  < eps && fabs(v1.y())  < eps && fabs(v1.z())  < eps)
    return false;
  EVector v2 = udir2;
  if (fabs(v2.x())  < eps && fabs(v2.y())  < eps && fabs(v2.z())  < eps)
    return false;
  
  double d02 = v0*v2;
  double d21 = v2*v1;
  double d01 = v0*v1;
  double d22 = v2*v2;
  double d11 = v1*v1;
  
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

 
bool HltUtils::SortTrackByPt::operator() (const Track* ini_track, 
                                           const Track* end_track ) const {
  const State& ini_state = ini_track->firstState();
  const State& end_state = end_track->firstState();
  double ini_pt = fabs(ini_state.pt());
  double end_pt = fabs(end_state.pt());
  double ini_z = fabs(ini_state.z());
  double end_z = fabs(end_state.z());
  bool ok = (ini_pt +0.001*fabs(ini_z)>= end_pt+0.001*fabs(end_z));
  return ok;
}


EVector HltUtils::closestDistance(const State& state1, 
                                   const State& state2) {
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

EPoint HltUtils::vertex(const State& state1, 
                         const State& state2) {
  
  EPoint ver(0.,0.,0.);
  EPoint pos1(0.,0.,0.);
  EPoint pos2(0.,0.,0.);
  EVector dir1(state1.tx(),state1.ty(),1.);
  EVector dir2(state2.tx(),state2.ty(),1.);
  EPoint  ori1(state1.x(),state1.y(),state1.z());
  EPoint  ori2(state2.x(),state2.y(),state2.z());
  closestPoints(ori1,dir1,ori2,dir2,pos1,pos2);
  ver = (pos1 + pos2)/2;
  //  std::cout << " HltUtils vertex " << ver << std::endl;
  return ver;
}

EVector HltUtils::impactParameterVector(const TVertex& vertex, 
                                         const State& state) {
  EPoint ver = vertex.position();
  EPoint ori(state.x(),state.y(),state.z());
  EVector dir(state.tx(), state.ty(),1.);
  return HltUtils::impactParameterVector(ver,ori,dir);
}


double HltUtils::impactParameter(const TVertex& vertex, 
                                  const State& state) {
  EPoint ver = vertex.position();
  EPoint ori(state.x(),state.y(),state.z());
  EVector dir(state.tx(), state.ty(),1.);
  return HltUtils::impactParameter(ver,ori,dir);
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
