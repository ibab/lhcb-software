// $Id: ProcUtilities.cpp,v 1.8 2008-12-16 15:59:50 jucogan Exp $

#include "L0MuonKernel/ProcUtilities.h"


int L0Muon::pendulumM1(int icol, int procVersion){
  if (procVersion==0)  return (icol==0) ? 0 : int(pow(-1.,icol+1)*int((icol+1)/2));  
  return (icol==0) ? 0 : int(pow(-1.,icol)*int((icol+1)/2));
}
int L0Muon::pendulumM2(int icol, int procVersion){
  if (procVersion==0)  return (icol==0) ? 0 : int(pow(-1.,icol+1)*int((icol+1)/2));  
  return (icol==0) ? 0 : int(pow(-1.,icol+1)*int((icol+1)/2));
}

void L0Muon::candidatesOrdering(std::vector<L0Muon::PMuonCandidate> * cands, int procVersion){
  if (procVersion>0) return;
  if (cands->size()<3) return;
  // Put the 2nd element at the end so it gets selected
  std::vector<L0Muon::PMuonCandidate>::iterator itcand=(cands->begin())+1;
  L0Muon::PMuonCandidate cand = (*itcand);
  cands->erase(itcand);
  cands->push_back(cand);
  return;
}

int L0Muon::extrapolationM1(int i,int procVersion){
  if (procVersion ==0) {
    const int ExtrapolationM1[6]={0,+4,+7,+11,+14,+18};
    return ExtrapolationM1[i];
  } else {
    const int ExtrapolationM1[6]={0,+4,+7,+11,+15,+18};
    return ExtrapolationM1[i];
  }
}


int L0Muon::addM1Offset(int M1nx, int offM1, int procVersion)
{
  if (procVersion==0) {// M1 Offset is expressed with M3 granularity
    M1nx += offM1;    
    M1nx =  M1nx >> 1;
  } else{              // M1 Offset is expressed with M1 granularity
    M1nx =  M1nx >> 1;
    M1nx += offM1;
  }
  return M1nx;
}

int L0Muon::offsetM1(int offM1, int procVersion)
{
  // offM1 in input is expressed with M3 granularity
  if (procVersion!=0) {
    if (offM1>=0) offM1++;
    offM1 = (offM1>>1);// expressed with M1 granularity (le hard est 'homogène') 
  }
  return offM1;
}

std::vector<LHCb::MuonTileID> L0Muon::add2pads(int quarter,int board, int pu, 
                                           int colM3, int rowM3, int offM2, int offM1,
                                           int procVersion, bool debug) {
  std::vector<LHCb::MuonTileID> tiles;

  if (procVersion<0) return tiles;
  
  // MuonTileID of the board where the candidate was found
  int region   = board/3;
  int boardloc = 1+ board - region*3;
  int boardnx  =  boardloc     & 1;
  int boardny  = (boardloc>>1) & 1;  
  LHCb::MuonTileID boardID(0,MuonLayout(1,1),region,quarter,boardnx,boardny);
  if(debug)  std::cout<< " L0Muon::add2pads boardID: " << boardID.toString() <<std::endl;

  // MuonTileID of the PU where the candidate was found
  int punx =  pu     & 1;
  int puny = (pu>>1) & 1;
  LHCb::MuonTileID puID(boardID,MuonLayout(2,2),punx,puny);
  if(debug)  std::cout<< "\t=> L0Muon::add2pads :"
                      <<" pu: "<<pu 
                      <<" (x: "<<punx <<" , y: "<<puny <<")"
                      <<" => puID: " << puID.toString() 
                      <<std::endl;

  // MuonTileID of the seed of the candidate (pad in M3)
  LHCb::MuonTileID padM3ID(puID,MuonLayout(48,8),colM3,rowM3);
  padM3ID.setStation(2);
  if(debug)  std::cout<< "\t=> L0Muon::add2pads : padM3ID: " << padM3ID.toString() 
                      << " (col: "<<colM3<<" , row: "<<rowM3<<" )"
                      <<std::endl;

  // MuonTileID of the pad in M2 of the candidate
  // M2 Offset can negative
  offM2 = offM2>=0x7 ? offM2-0x10 : offM2;
  if(debug)  std::cout<< "\t=> L0Muon::add2pads : offM2 = "<<offM2 <<std::endl;
  int M2nx = colM3+offM2;
  int M2ny = rowM3;
  LHCb::MuonTileID padM2ID(puID,MuonLayout(48,8),M2nx,M2ny);
  padM2ID.setStation(1);

  // If M2 pad is coming from a upper region, convert it
  if (!padM2ID.isValid()) {
    if ( padM2ID.nX()>=2*padM2ID.layout().xGrid() ) {
      std::vector<LHCb::MuonTileID> lpads =  padM2ID.layout().tilesInRegion(padM2ID,padM2ID.region());
      if (lpads.size()!=1) {
        if(debug){
          std::cout<<" M2 PAD DOES NOT COVER EXACTLY ONE PAD IN UPPER REGION"<<std::endl;
          std::cout<<"   * PB"<<board<<" PU"<<pu<<std::endl;
          std::cout<<"   * col/row M3= "<<colM3<<" / "<<rowM3<<" offM2= "<<offM2<<std::endl;
          std::cout<<"   * padM2ID= "<<padM2ID.toString()<< " lpads.size()= "<<lpads.size()<<std::endl;
        }
        return tiles;
      }
      padM2ID = lpads[0];
    } else {
      if(debug)  std::cout<< "WARNING: M2 PAD IS NOT VALID: padM2ID= " << padM2ID.toString()  <<" ?? "<<std::endl;
    }
  }

  // Extrapolation in M1
  int extrapM1 = L0Muon::extrapolationM1(abs(offM2),procVersion);

  extrapM1 = offM2>0 ? extrapM1 : -1*extrapM1;
  if(debug)  std::cout<< "\t=> L0Muon::add2pads : extrapM1 = "<<extrapM1 <<std::endl;

  // MuonTileID of the pad in M1 of the candidate
  offM1 = offM1>=0x7 ? offM1-0x10 : offM1;
  if(debug)  std::cout<< "\t=> L0Muon::add2pads : offM1 = "<<offM1 <<std::endl;

  int M1nx = colM3+extrapM1;
  int M1ny = rowM3;  
  M1nx = L0Muon::addM1Offset(M1nx, offM1, procVersion);

  if(debug)  std::cout<< "\t=> L0Muon::add2pads : M1nx = "<<M1nx <<" M1ny = "<<M1ny <<std::endl;
  LHCb::MuonTileID padM1ID(puID,MuonLayout(24,8),M1nx,M1ny);;
  padM1ID.setStation(0);
  if(debug) std::cout <<"\t=> L0Muon::add2pads : padM1ID= "<<padM1ID.toString()<<std::endl;

  // If M1 pad is coming from a upper region, convert it
  if (!padM1ID.isValid()) {
    if ( padM1ID.nX()>=2*padM1ID.layout().xGrid() ) {
      std::vector<LHCb::MuonTileID> lpads =  padM1ID.layout().tilesInRegion(padM1ID,padM1ID.region());
      if (lpads.size()==0) {
        if(debug){
          std::cout <<" L0Muon::add2pads : M1 PAD DOES NOT COVER ANY PAD IN UPPER REGION"<<std::endl;
          std::cout<<"   * PB"<<board<<" PU"<<pu<<std::endl;
          std::cout<<"   * col /row M3= "<<colM3<<" / "<<rowM3<<" offM2= "<<offM2<<"offM1= "<<offM1<<std::endl;
          std::cout<<"   * padM2ID= "<<padM2ID.toString()<< std::endl;
          std::cout <<"  * padM1ID= "<<padM1ID.toString()<< " ; lpads.size()= "<<lpads.size()<<std::endl;
        }
        return tiles;
      } else {
        if (lpads.size()>1) {
          if(debug){
            std::cout <<" L0Muon::add2pads : M1 PAD DOES NOT COVER EXACTLY ONE PAD IN UPPER REGION"<<std::endl;
            std::cout<<"   * PB"<<board<<" PU"<<pu<<std::endl;
            std::cout<<"   * col /row M3= "<<colM3<<" / "<<rowM3<<" offM2= "<<offM2<<"offM1= "<<offM1<<std::endl;
            std::cout<<"   * padM2ID= "<<padM2ID.toString()<< std::endl;
            std::cout <<"  * padM1ID= "<<padM1ID.toString()<< " ; lpads.size()= "<<lpads.size()<<std::endl;
          }
        }
        padM1ID = lpads[0];
      }
    } else {
      if(debug)   std::cout << "WARNING: M1 PAD IS NOT VALID: padM1ID= " << padM1ID.toString()  <<" ?? "<<std::endl;
    }
  }

  // Fill the vector to be returned
  tiles.push_back(padM1ID);
  tiles.push_back(padM2ID);
  tiles.push_back(padM3ID);

  return tiles;

}

void  L0Muon::xyFromPad(LHCb::MuonTileID pad, double& x, double& y, int procVersion)  {

  if (procVersion<0) return;

  double dx = 1.0;
  double dy = 2.5;
  double l1 = 1210.; //Z position of M1 = 1210.;
  double l2 = 1527.; //Z position of M2 = 1527.;
  double l3 = 1647.; //Z position of M3 = 1647.;
    
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

/*
// Compute pt, theta and phi
std::vector<double> L0Muon::kine(LHCb::MuonTileID p1, LHCb::MuonTileID p2, int procVersion, bool debug) {

double x1=0., y1=0., z1=0.;// M1 hit coordinates
double x2=0., y2=0., z2=0.;// M2 hit coordinates

xyFromPad(p1,x1,y1,procVersion);
xyFromPad(p2,x2,y2,procVersion);

return kine(p1,p2,procVersion,debug,x1,y1,z1,x2,y2,z2);
}
*/

// Compute pt, theta and phi
std::vector<double> L0Muon::kine(LHCb::MuonTileID p1, LHCb::MuonTileID p2, int procVersion, bool debug,\
                                 double x1, double y1, double z1,\
                                 double x2, double y2, double z2) {
  if (procVersion==0) return  L0Muon::kineV0(p1,p2,debug);

  // the kinematic variables to be returned
  std::vector<double> kine;
  double pt,theta,phi;

  double d1    = 545.  ; // Z(MAG)-Z(VTX) =  545.  
  double d2    = 665.5 ; // Z(M1)-Z(MAG)  =  665.5
  double d3    = 309.5 ; // Z(M2)-Z(M1)   =  309.5
  double alpha =   1.39; // alpha (KICK?) =    1.39 

  if ( (z1>0.) || (z2>0.) ){
    d2=(z1/10.)-d1;
    d3=(z2-z1)/10.;
  }

  //double x1=0., y1=0.;// M1 hit coordinates
  //double x2=0., y2=0.;// M2 hit coordinates
  
  if ( (x1==0.) || (y1==0.) ) {
    xyFromPad(p1,x1,y1,procVersion);
  } else {
    x1=x1/10.;
    y1=y1/10.;
  }
  
  if ( (x2==0.) || (y2==0.) ) {
    xyFromPad(p2,x2,y2,procVersion);
  } else {
    x2=x2/10.;
    y2=y2/10.;
  }

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


  if (debug)    std::cout  <<"\t=> L0Muon::kine : "
                           << " p1 = " <<   p1.toString()
                           << " p2 = " <<   p2.toString()
                  //                            << " d1 = " <<   d1
                  //                            << " d2 = " <<   d2
                  //                            << " d3 = " <<   d3
                  //                            << " alpha= " << alpha
                  //                            << " x1 = " <<   x1
                  //                            << " y1 = " <<   y1
                  //                            << " x2 = " <<   x2
                  //                            << " y2 = " <<   y2
                  //                            << " sq= " <<    sq
                  //                            << " sr= " <<    sr
                  //                            << " sc= " <<    sc
                  //                            << " st= " <<    st
                  //                            << " ptm= " <<   ptm
                  //                            << " xslope= " <<xslope
                  //                            << " xin= " <<   xin
                           << " pt= " <<    pt 
                           << " theta= " << theta
                           << " phi= " << phi
                           << std::endl;

  kine.push_back(pt);
  kine.push_back(theta);
  kine.push_back(phi);
  return kine;

}

std::vector<double> L0Muon::kineV0(LHCb::MuonTileID p1, LHCb::MuonTileID p2, bool debug) {

  // the kinematic variables to be returned
  std::vector<double> kine;
  double pt,theta,phi;

  double d1    = 545.  ; // Z(MAG)-Z(VTX) =  545.  
  double d2    = 665.5 ; // Z(M1)-Z(MAG)  =  665.5
  double d3    = 309.5 ; // Z(M2)-Z(M1)   =  309.5
  double alpha =   1.39; // alpha (KICK?) =    1.39 

  double x1=0., y1=0.;// M1 hit coordinates
  double x2=0., y2=0.;// M2 hit coordinates

  xyFromPad(p1,x1,y1,0);
  xyFromPad(p2,x2,y2,0);

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


  if (debug)    std::cout  <<"---  L0Muon::kineV0 : "
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

int L0Muon::encodePT(double pT,int procVersion, bool debug) {
  
  int ipt=0;

  if (procVersion==-1) std::cout<<"L0Muon::ptEncoding wrong procVersion= "<<procVersion<<std::endl;
  
  // Encoding parameters
  static int nbits=7;
  static int ptMax = (1<<nbits)-1;
  static double dpT=40.;
  
  if (debug) std::cout<<"L0Muon::ptEncoding parameters are: nbits= "<<nbits<<" ptMax= "<<ptMax<<" dpT= "<<dpT<<std::endl;
  
  // Encode absolute value
  ipt= int(0.5+fabs(pT)/dpT);
  ipt = ipt>ptMax ? ptMax:ipt;
  
  // Encode charge
  if (pT>0) ipt|=(1<<nbits);

  if (debug) std::cout<<"L0Muon::ptEncoding pT= "<<pT<<" => "<<ipt<<std::endl;

  return ipt;
  
}
