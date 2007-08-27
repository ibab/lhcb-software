// $Id: MuonCandidate.cpp,v 1.4 2007-08-27 09:32:26 jucogan Exp $

#include <cmath>
#include <vector>
#include "L0MuonKernel/MuonCandidate.h"
#include "L0MuonKernel/ProcUtilities.h"

#include <iostream>

L0Muon::MuonCandidate::MuonCandidate() {
  m_colM3  =0;
  m_rowM3  =0;
  m_offM2  =0;
  m_offM1  =0;
  m_pu     =0;
  m_quarter=0;
  m_board  =0;
  m_pT     =0;
  m_charge =0;
}

L0Muon::MuonCandidate::MuonCandidate(const MuonCandidate& cand) {
  m_colM3   = cand.colM3();
  m_rowM3   = cand.rowM3();
  m_offM2   = cand.offM2();
  m_offM1   = cand.offM1();
  m_pu      = cand.pu();
  m_quarter = cand.quarter();
  m_board   = cand.board();
  m_pT      = cand.pT();
  m_charge  = cand.charge();
}


std::string L0Muon::MuonCandidate::dump(std::string tab){
  std::string cand="";
  std::string sbuf;
  char buf[16];

  cand += tab+"|----- CAND -------|\n";
  sprintf(buf,"%4d",m_quarter );
  sbuf = buf;
  cand +=   tab+"|   quarter = " + sbuf +" |\n";
  sprintf(buf,"%4d",m_board   );
  sbuf = buf;
  cand +=   tab+"|   board   = " + sbuf +" |\n";
  sprintf(buf,"%4d",m_pu      );
  sbuf = buf;
  cand +=   tab+"|   pu      = " + sbuf +" |\n";
  sprintf(buf,"%4d",m_colM3   );
  sbuf = buf;
  cand +=   tab+"|   colM3   = " + sbuf +" |\n";
  sprintf(buf,"%4d",m_rowM3   );
  sbuf = buf;
  cand +=   tab+"|   rowM3   = " + sbuf +" |\n";
  sprintf(buf,"%4d",m_offM2   );
  sbuf = buf;
  cand +=   tab+"|   offM2   = " + sbuf +" |\n";
  sprintf(buf,"%4d",m_offM1   );
  sbuf = buf;
  cand +=   tab+"|   offM1   = " + sbuf +" |\n";
  sprintf(buf,"%4d",m_pT      );
  sbuf = buf;
  cand +=   tab+"|   pt      = " + sbuf +" |\n";
  sprintf(buf,"%4d",m_charge  );
  sbuf = buf;
  cand +=   tab+"|   charge  = " + sbuf +" |\n";
  cand += tab+"|------------------|\n";
  
  return cand;
  
}

// L0Muon::MuonCandidate::MuonTileID boardID(bool debug) const {
//   // MuonTileID of the board where the candidate was found
//   int region   = m_board/3;
//   int boardloc = 1+ m_board - region*3;
//   int boardnx  =  boardloc     & 1;
//   int boardny  = (boardloc>>1) & 1;  
//   LHCb::MuonTileID boardID(0,MuonLayout(1,1),region,m_quarter,boardnx,boardny);
//   if(debug)  std::cout<< "L0Muon::MuonCandidate::boardID  " << boardID.toString() <<std::endl;
// }

// L0Muon::MuonCandidate::MuonTileID puID(bool debug) const {
//   LHCb::MuonTileID boardID = boardID(debug);
//   // MuonTileID of the PU where the candidate was found
//   int punx =  m_pu     & 1;
//   int puny = (m_pu>>1) & 1;
//   LHCb::MuonTileID puID(boardID,MuonLayout(2,2),punx,puny);
//   if(debug)  std::cout<< "L0Muon::MuonCandidate::pads"
//                       <<" pu: "<<m_pu 
//                       <<" (x: "<<punx <<" , y: "<<puny <<")"
//                       <<" => puID: " << puID.toString() 
//                       <<std::endl;  
// }

// // std::vector<LHCb::MuonTileID> L0Muon::MuonCandidate::pads(int procVersion, bool debug) const{
 
// //   std::vector<LHCb::MuonTileID> tiles;

// //   // MuonTileID of the board where the candidate was found
// //   int region   = m_board/3;
// //   int boardloc = 1+ m_board - region*3;
// //   int boardnx  =  boardloc     & 1;
// //   int boardny  = (boardloc>>1) & 1;  
// //   LHCb::MuonTileID boardID(0,MuonLayout(1,1),region,m_quarter,boardnx,boardny);
// //   if(debug)  std::cout<< "L0Muon::MuonCandidate::pads boardID: " << boardID.toString() <<std::endl;

// //   // MuonTileID of the PU where the candidate was found
// //   int punx =  m_pu     & 1;
// //   int puny = (m_pu>>1) & 1;
// //   LHCb::MuonTileID puID(boardID,MuonLayout(2,2),punx,puny);
// //   if(debug)  std::cout<< "L0Muon::MuonCandidate::pads"
// //                       <<" pu: "<<m_pu 
// //                       <<" (x: "<<punx <<" , y: "<<puny <<")"
// //                       <<" => puID: " << puID.toString() 
// //                       <<std::endl;

// // //   // MuonTileID of the PU where the candidate was found
// // //   LHCb::MuonTileID puID = puID(debug);

// //   // MuonTileID of the seed of the candidate (pad in M3)
// //   LHCb::MuonTileID padM3ID(puID,MuonLayout(48,8),m_colM3,m_rowM3);
// //   padM3ID.setStation(2);
// //   if(debug)  std::cout<< "L0Muon::MuonCandidate::pads padM3ID: " << padM3ID.toString() 
// //                       << " (col: "<<m_colM3<<" , row: "<<m_rowM3<<" )"
// //                       <<std::endl;
  
// //   // MuonTileID of the pad in M2 of the candidate
// //   // M2 Offset can negative
// //   int offM2 = m_offM2;
// //   if (offM2>=0x7) offM2 = offM2-0x10;
// //   if(debug)  std::cout<< "L0Muon::MuonCandidate::pads offM2 = "<<offM2 <<std::endl;
// //   int M2nx = m_colM3+offM2;
// //   int M2ny = m_rowM3;
// //   LHCb::MuonTileID padM2ID(puID,MuonLayout(48,8),M2nx,M2ny);
// //   padM2ID.setStation(1);
  
// //   // If M2 pad is coming from a upper region, convert it
// //   if (!padM2ID.isValid()) {
// //     if ( padM2ID.nX()>=2*padM2ID.layout().xGrid() ) {
// //       std::vector<LHCb::MuonTileID> lpads =  padM2ID.layout().tilesInRegion(padM2ID,padM2ID.region());
// //       if (lpads.size()!=1) {
// //         std::cout<<" M2 PAD DOES NOT COVER EXACTLY ONE PAD IN UPPER REGION"<<std::endl;
// //         std::cout<<" padM2ID= "<<padM2ID.toString()<< "lpads.size()= "<<lpads.size()<<std::endl;
// //         return tiles;
        
// //       }
// //       padM2ID = lpads[0];
// //     } else {
// //       if(debug)  std::cout<< "WARNING: M2 PAD IS NOT VALID: padM2ID= " << padM2ID.toString()  <<" ?? "<<std::endl;
// //     }
// //   }

// //   // Extrapolation in M1
// //   int extrapM1 = L0Muon::extrapolationM1(abs(offM2),procVersion);
  
// //   extrapM1 = offM2>0 ? extrapM1 : -1*extrapM1;
// //   if(debug)  std::cout<< "L0Muon::MuonCandidate::pads extrapM1 = "<<extrapM1 <<std::endl;

// //   // MuonTileID of the pad in M1 of the candidate
// //   int offM1 = m_offM1>=0x7 ? m_offM1-0x10 : m_offM1;
// //   if(debug)  std::cout<< "L0Muon::MuonCandidate::pads offM1 = "<<offM1 <<std::endl;
  
// //   int M1nx = m_colM3+extrapM1;
// //   int M1ny = m_rowM3;  
// // // //   if (procVersion==0) {// M1 Offset is expressed with M3 granularity
// // // //     M1nx += offM1;    
// // // //     M1nx =  M1nx >> 1;
// // // //   } else{              // M1 Offset is expressed with M1 granularity
// // // //     M1nx =  M1nx >> 1;
// // // //     M1nx += offM1;
// // // //   }
// //   M1nx = L0Muon::addM1Offset(M1nx, offM1, procVersion);
  
// //   if(debug)  std::cout<< "L0Muon::MuonCandidate::pads M1nx = "<<M1nx <<" M1ny = "<<M1ny <<std::endl;
// //   LHCb::MuonTileID padM1ID(puID,MuonLayout(24,8),M1nx,M1ny);;
// //   padM1ID.setStation(0);
// //   if(debug) std::cout <<"L0Muon::MuonCandidate::pads padM1ID= "<<padM1ID.toString()<<std::endl;

// //   // If M1 pad is coming from a upper region, convert it
// //   if (!padM1ID.isValid()) {
// //     if ( padM1ID.nX()>=2*padM1ID.layout().xGrid() ) {
// //       std::vector<LHCb::MuonTileID> lpads =  padM1ID.layout().tilesInRegion(padM1ID,padM1ID.region());
// //       if (lpads.size()!=1) {
// //         std::cout <<" M1 PAD DOES NOT COVER EXACTLY ONE PAD IN UPPER REGION"<<std::endl;
// //         std::cout <<" padM1ID= "<<padM1ID.toString()<< " ; lpads.size()= "<<lpads.size()<<std::endl;
// //       }
// //       padM1ID = lpads[0];
// //     } else {
// //       if(debug)   std::cout << "WARNING: M1 PAD IS NOT VALID: padM1ID= " << padM1ID.toString()  <<" ?? "<<std::endl;
// //     }
// //   }

// //   // Fill the vector to be returned
// //   tiles.push_back(padM1ID);
// //   tiles.push_back(padM2ID);
// //   tiles.push_back(padM3ID);
  
// //   return tiles;
  
// // }

// // void  L0Muon::xyFromPad(LHCb::MuonTileID pad, double& x, double& y)  {

// //   double dx = 1.0;
// //   double dy = 2.5;
// // //   double l1 = ptparam[4]; //Z position of M1 = 1210.;
// // //   double l2 = ptparam[5]; //Z position of M2 = 1527.;
// // //   double l3 = ptparam[6]; //Z position of M3 = 1647.;
// //   double l1 = 1210.; //Z position of M1 = 1210.;
// //   double l2 = 1527.; //Z position of M2 = 1527.;
// //   double l3 = 1647.; //Z position of M3 = 1647.;
    
// //   int ns = pad.station();
// //   int nq = pad.quarter();
// //   int nr = pad.region();
// //   int nx = pad.nX();
// //   int ny = pad.nY();
    
// //   double nreg = 1.;
// //   if ( nr == 1) {
// //     nreg = 2.;
// //   } else if ( nr == 2) {
// //     nreg = 4.;
// //   } else if ( nr == 3) {
// //     nreg = 8.;
// //   }
  
// //   double factorX, factorY;
// //   factorX = 24./double(pad.layout().xGrid());
// //   factorY = 8./double(pad.layout().yGrid());
  
// //   x = dx*(nx+0.5)*nreg*factorX;
// //   y = dy*(ny+0.5)*nreg*factorY;
// //   if ( ns == 1 ) {
// //     x *= l2/l1;
// //     y *= l2/l1;
// //   } else if ( ns == 2 ) {
// //     x *= l3/l1;
// //     y *= l3/l1;
// //   }
// //   if ( nq == 1 ) {
// //     y = -y;
// //   } else if ( nq == 2 ) {
// //     x = -x;
// //     y = -y;
// //   } else if ( nq == 3 ) {
// //     x = -x;
// //   }    
    
// // }

// // // Compute pt, theta and phi
// // std::vector<double> L0Muon::kine(LHCb::MuonTileID p1, LHCb::MuonTileID p2, int procVersion, bool debug) {

// //   if (procVersion==0) return  L0Muon::kineV0(p1,p2,debug);

// //   //   MsgStream  msg( msgSvc(), name() );

// //   // the kinematic variables to be returned
// //   std::vector<double> kine;
// //   double pt,theta,phi;

// //   double d1    = 545.  ; // Z(MAG)-Z(VTX) =  545.  
// //   double d2    = 665.5 ; // Z(M1)-Z(MAG)  =  665.5
// //   double d3    = 309.5 ; // Z(M2)-Z(M1)   =  309.5
// //   double alpha =   1.39; // alpha (KICK?) =    1.39 

// //   double x1=0., y1=0.;// M1 hit coordinates
// //   double x2=0., y2=0.;// M2 hit coordinates

// //   xyFromPad(p1,x1,y1);
// //   xyFromPad(p2,x2,y2);

// //   double x0 = x1 - d2*(x2-x1)/d3;
// //   double y0 = y1*d1/(d1+d2);

// //   double sq = (d1+d2)*x2-(d1+d2+d3)*x1;
// //   if(sq == 0.) {
// //     sq = 0.0001;
// //   } else {
// //     sq = 1./((d1+d2)*x2-(d1+d2+d3)*x1);
// //   }

// //   //   std::cout << "*******Calculation pt " << std::endl;
// //   //   std::cout << "x1: " << x1 << std::endl;
// //   //   std::cout << "y1: " << y1 << std::endl;
// //   //   std::cout << "x2: " << x2 << std::endl;

// //   double sr = sqrt(((d2+d3)*x1 - d2*x2)*((d2+d3)*x1 - d2*x2) +
// //                    (d1*d3*y1/(d1+d2))*(d1*d3*y1/(d1+d2))   );
// //   double sc = sqrt((d1+d2)*(d1+d2)+y1*y1)/(d1+d2);
// //   double st = (d1*d3*d3+2.*(x1*(d3+d2)-x2*d2)*(x2-x1))/(d1*d3*d3);
// //   double ptm = fabs(alpha*sq*sr*sc*st);

// //   double xslope = (x2 - x1)/d3;
// //   double xin = x2 - xslope*(d1+d2+d3);
// //   if(xin < 0.) ptm = -ptm;

// //   // Pt should be in MeV
// //   pt = ptm*1000.;
// //   theta = atan(sqrt(x0*x0+y0*y0)/d1);
// //   if (fabs(x0)<0.0001) {
// //     phi = 3.14159/2.;
// //     if ( y0 < 0.) phi = -phi;
// //   } else {
// //     phi = atan(y0/x0);
// //     if ( phi > 0. && y0 < 0. ) {
// //       phi -= 3.14159;
// //     }
// //     if ( phi < 0. && y0 > 0. ) {
// //       phi += 3.14159;
// //     }
// //   }


// //   if (debug)    std::cout  <<"---  Utilities::ptcalc: "
// //                            << " p1 = " <<   p1.toString()
// //                            << " p2 = " <<   p2.toString()
// //                            << " d1 = " <<   d1
// //                            << " d2 = " <<   d2
// //                            << " d3 = " <<   d3
// //                            << " alpha= " << alpha
// //                            << " x1 = " <<   x1
// //                            << " y1 = " <<   y1
// //                            << " x2 = " <<   x2
// //                            << " y2 = " <<   y2
// //                            << " sq= " <<    sq
// //                            << " sr= " <<    sr
// //                            << " sc= " <<    sc
// //                            << " st= " <<    st
// //                            << " ptm= " <<   ptm
// //                            << " xslope= " <<xslope
// //                            << " xin= " <<   xin
// //                            << " pt= " <<    pt 
// //                            << " theta= " << theta
// //                            << " phi= " << phi
// //                            << std::endl;

// //   kine.push_back(pt);
// //   kine.push_back(theta);
// //   kine.push_back(phi);
// //   return kine;

// // }

// // std::vector<double> L0Muon::kineV0(LHCb::MuonTileID p1, LHCb::MuonTileID p2, bool debug) {
// //   //   MsgStream  msg( msgSvc(), name() );

// //   // the kinematic variables to be returned
// //   std::vector<double> kine;
// //   double pt,theta,phi;

// //   double d1    = 545.  ; // Z(MAG)-Z(VTX) =  545.  
// //   double d2    = 665.5 ; // Z(M1)-Z(MAG)  =  665.5
// //   double d3    = 309.5 ; // Z(M2)-Z(M1)   =  309.5
// //   double alpha =   1.39; // alpha (KICK?) =    1.39 

// //   double x1=0., y1=0.;// M1 hit coordinates
// //   double x2=0., y2=0.;// M2 hit coordinates

// //   xyFromPad(p1,x1,y1);
// //   xyFromPad(p2,x2,y2);

// //   double x0 = x1 - d2*(x2-x1)/d3;
// //   double y0 = y1*d1/(d1+d2);

// //   double sq = (d1+d2)*x2-(d1+d2+d3)*x1;
// //   if(sq == 0.) {
// //     sq = 0.0001;
// //   } else {
// //     sq = 1./((d1+d2)*x2-(d1+d2+d3)*x1);
// //   }

// //   //   std::cout << "*******Calculation pt " << std::endl;
// //   //   std::cout << "x1: " << x1 << std::endl;
// //   //   std::cout << "y1: " << y1 << std::endl;
// //   //   std::cout << "x2: " << x2 << std::endl;

// //   double sr = sqrt(((d2+d3)*x1 - d2*x2)*((d2+d3)*x1 - d2*x2) +
// //                    (d1*d3*y1/(d1+d2))*(d1*d3*y1/(d1+d2))   );
// //   double sc = sqrt((d1+d2)*(d1+d2)+y1*y1)/(d1+d2);
// //   double st = (d1*d3*d3+2.*(x1*(d3+d2)-x2*d2)*(x2-x1))/(d1*d3*d3);
// //   double ptm = fabs(alpha*sq*sr*sc*st);

// //   double xslope = (x2 - x1)/d3;
// //   double xin = x2 - xslope*(d1+d2+d3);
// //   if(xin < 0.) ptm = -ptm;

// //   // Pt should be in MeV
// //   pt = ptm*1000.;
// //   theta = atan(sqrt(x0*x0+y0*y0)/d1);
// //   if (fabs(x0)<0.0001) {
// //     phi = 3.14159/2.;
// //     if ( y0 < 0.) phi = -phi;
// //   } else {
// //     phi = atan(y0/x0);
// //     if ( phi > 0. && y0 < 0. ) {
// //       phi -= 3.14159;
// //     }
// //     if ( phi < 0. && y0 > 0. ) {
// //       phi += 3.14159;
// //     }
// //   }


// //   if (debug)    std::cout  <<"---  Utilities::ptcalc: "
// //                            << " p1 = " <<   p1.toString()
// //                            << " p2 = " <<   p2.toString()
// //                            << " d1 = " <<   d1
// //                            << " d2 = " <<   d2
// //                            << " d3 = " <<   d3
// //                            << " alpha= " << alpha
// //                            << " x1 = " <<   x1
// //                            << " y1 = " <<   y1
// //                            << " x2 = " <<   x2
// //                            << " y2 = " <<   y2
// //                            << " sq= " <<    sq
// //                            << " sr= " <<    sr
// //                            << " sc= " <<    sc
// //                            << " st= " <<    st
// //                            << " ptm= " <<   ptm
// //                            << " xslope= " <<xslope
// //                            << " xin= " <<   xin
// //                            << " pt= " <<    pt 
// //                            << " theta= " << theta
// //                            << " phi= " << phi
// //                            << std::endl;

// //   kine.push_back(pt);
// //   kine.push_back(theta);
// //   kine.push_back(phi);
// //   return kine;

// // }
