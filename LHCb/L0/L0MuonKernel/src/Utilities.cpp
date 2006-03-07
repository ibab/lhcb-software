#include "L0MuonKernel/Utilities.h"
#include <math.h>

void L0Muon::xyFromPad(LHCb::MuonTileID pad, double& x, double& y,
                       std::vector<double> ptparam)  {

  double dx = 1.0;
  double dy = 2.5;
  double l1 = ptparam[4]; //Z position of M1 = 1210.;
  double l2 = ptparam[5]; //Z position of M2 = 1527.;
  double l3 = ptparam[6]; //Z position of M3 = 1647.;
    
  int ns = pad.station();
  int nq = pad.quarter();
  int nr = pad.region();
  int nx = pad.nX();
  int ny = pad.nY();
    
  double nreg = 1.;
  if ( nr == 1) {
    nreg = 2.;
  } else if ( nr == 2) {
    nreg = 4.;
  } else if ( nr == 3) {
    nreg = 8.;
  }
  
  double factorX, factorY;
  factorX = 24./double(pad.layout().xGrid());
  factorY = 8./double(pad.layout().yGrid());
  
  x = dx*(nx+0.5)*nreg*factorX;
  y = dy*(ny+0.5)*nreg*factorY;
  if ( ns == 1 ) {
    x *= l2/l1;
    y *= l2/l1;
  } else if ( ns == 2 ) {
    x *= l3/l1;
    y *= l3/l1;
  }
  if ( nq == 1 ) {
    y = -y;
  } else if ( nq == 2 ) {
    x = -x;
    y = -y;
  } else if ( nq == 3 ) {
    x = -x;
  }    
    
}


// Compute pt, theta and phi
std::vector<double> L0Muon::ptcalc(LHCb::MuonTileID p1, LHCb::MuonTileID p2,
                                   std::vector<double> ptparam, bool debug) {
  //   MsgStream  msg( msgSvc(), name() );

  // the kinematic variables to be returned
  std::vector<double> kine;
  double pt,theta,phi;

  double d1    = ptparam[0]; // Z(MAG)-Z(VTX) =  545.
  double d2    = ptparam[1]; // Z(M1)-Z(MAG)  =  665.5
  double d3    = ptparam[2]; // Z(M2)-Z(M1)   =  309.5
  double alpha = ptparam[3]; // alpha (KICK?) = 1.39

//   std::vector<double> xM1 = xyFromPad(p1,ptparam); // M1 hit coordinates
//   std::vector<double> xM2 = xyFromPad(p2,ptparam); // M2 hit coordinates

//   double x1 = xM1[0];
//   double y1 = xM1[1];

//   double x2 = xM2[0];
//   double y2 = xM2[1];
  double x1=0., y1=0.;// M1 hit coordinates
  double x2=0., y2=0.;// M2 hit coordinates

  xyFromPad(p1,x1,y1,ptparam);
  xyFromPad(p2,x2,y2,ptparam);

  double x0 = x1 - d2*(x2-x1)/d3;
  double y0 = y1*d1/(d1+d2);

  double sq = (d1+d2)*x2-(d1+d2+d3)*x1;
  if(sq == 0.) {
    sq = 0.0001;
  } else {
    sq = 1./((d1+d2)*x2-(d1+d2+d3)*x1);
  }

  //   std::cout << "*******Calculation pt " << std::endl;
  //   std::cout << "x1: " << x1 << std::endl;
  //   std::cout << "y1: " << y1 << std::endl;
  //   std::cout << "x2: " << x2 << std::endl;

  double sr = sqrt(((d2+d3)*x1 - d2*x2)*((d2+d3)*x1 - d2*x2) +
                   (d1*d3*y1/(d1+d2))*(d1*d3*y1/(d1+d2))   );
  double sc = sqrt((d1+d2)*(d1+d2)+y1*y1)/(d1+d2);
  double st = (d1*d3*d3+2.*(x1*(d3+d2)-x2*d2)*(x2-x1))/(d1*d3*d3);
  double ptm = fabs(alpha*sq*sr*sc*st);

  double xslope = (x2 - x1)/d3;
  double xin = x2 - xslope*(d1+d2+d3);
  if(xin < 0.) ptm = -ptm;

  // Pt should be in MeV
  pt = ptm*1000.;
  theta = atan(sqrt(x0*x0+y0*y0)/d1);
  if (fabs(x0)<0.0001) {
    phi = 3.14159/2.;
    if ( y0 < 0.) phi = -phi;
  } else {
    phi = atan(y0/x0);
    if ( phi > 0. && y0 < 0. ) {
      phi -= 3.14159;
    }
    if ( phi < 0. && y0 > 0. ) {
      phi += 3.14159;
    }
  }


  if (debug)    std::cout  <<"---  L0MuonCandidatesFromRaw::ptcalc: "
                           << " p1 = " <<   p1.toString()
                           << " p2 = " <<   p2.toString()
                           << " d1 = " <<   d1
                           << " d2 = " <<   d2
                           << " d3 = " <<   d3
                           << " alpha= " << alpha
                           << " x1 = " <<   x1
                           << " y1 = " <<   y1
                           << " x2 = " <<   x2
                           << " y2 = " <<   y2
                           << " sq= " <<    sq
                           << " sr= " <<    sr
                           << " sc= " <<    sc
                           << " st= " <<    st
                           << " ptm= " <<   ptm
                           << " xslope= " <<xslope
                           << " xin= " <<   xin
                           << " pt= " <<    pt 
                           << " theta= " << theta
                           << " phi= " << phi
                           << std::endl;

  kine.push_back(pt);
  kine.push_back(theta);
  kine.push_back(phi);
  return kine;

}


