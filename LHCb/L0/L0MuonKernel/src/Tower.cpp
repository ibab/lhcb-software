//#include <time.h>
#include <set>
#include <vector>
#include <algorithm>

#include "CLHEP/Vector/ThreeVector.h"

#include "GaudiKernel/IToolSvc.h" 
#include "MuonTools/IMuonTileXYZTool.h"

#include "L0MuonKernel/RegisterFactory.h"
#include "L0MuonKernel/Tower.h"
#include "L0MuonKernel/Link.h"


L0Muon::Tower::Tower() {

  m_maxXFoI[0] = 24; //12;
  m_maxXFoI[1] = 5;
  m_maxXFoI[2] = 0;
  m_maxXFoI[3] = 12; //3;
  m_maxXFoI[4] = 12; //3;
  
  m_maxYFoI[0] = 0;
  m_maxYFoI[1] = 0;
  m_maxYFoI[2] = 0;
  m_maxYFoI[3] = 1;
  m_maxYFoI[4] = 1;

  


  m_ncand =0;
  
  int sta;
  int row;
  for ( sta = 0; sta < 5; sta++) {
    boost::dynamic_bitset<> tmprow(24+2*m_maxXFoI[sta]);
    for ( row = 0; row < 4+2*m_maxYFoI[sta]; row++ ) {
      m_bittable[sta].push_back(tmprow);
         
    }
  }
  
  m_pt =0.;
  m_theta =0.;
  m_phi = 0.;

  m_usefulevent = 0;
  

}

L0Muon::Tower::~Tower() {}



void L0Muon::Tower::reset() {

  int sta;
  int row;
  for ( sta = 0; sta < 5; sta++ ) {
    for ( row = 0; row < 4+2*(m_maxYFoI[sta]); row++ ) {
      m_bittable[sta][row].reset();
    }
  }
  
}

void L0Muon::Tower::setBit(MsgStream *log, int sta, int row, int col) {

  int xFoI = m_maxXFoI[sta];
  int yFoI = m_maxYFoI[sta];
  
  if (col < 0 || col >= (24+2* xFoI) ) {
    *log << MSG::DEBUG << "Column is not valid !" << endreq;
    *log << MSG::DEBUG << "Station: " << sta << " Col: " << col << endreq;
    return;	      
  }
  if (row < 0 || row >= (4+2*yFoI) ) {
    *log << MSG::DEBUG << "Row is not valid ! " << endreq;
    *log << MSG::DEBUG << "Station: " << sta << " Row: " << row << endreq;
    return;	      
  }
  
  
  m_bittable[sta][row].set(col);
  
}


void L0Muon::Tower::setPadIdMap(int sta, 
                                std::pair<int, int> YX, 
                                MuonTileID mid)
{
  m_idmap[sta][YX]= mid ;
 
}

MuonTileID L0Muon::Tower::getPadIdMap(int sta, 
                                std::pair<int, int> XY)
{
  
  return m_idmap[sta].find(XY)->second ;
  
}



boost::dynamic_bitset<> L0Muon::Tower::getBits(int sta, 
                                               L0Muon::Tower::HitIndex seed, 
                                               int xfoi) {

  return L0Muon::Tower::getBits(sta, seed.first, seed.second, xfoi);

}

boost::dynamic_bitset<> L0Muon::Tower::getBits(int sta, 
                                               int x, 
                                               int y, 
                                               int xfoi) {
  int resize = xfoi*2+1;
  int xmin = x-xfoi;
  int xmax = x+xfoi;
  int rowsize = m_bittable[sta][y].size();
  boost::dynamic_bitset<> result(resize);
  int it;
  for(it = xmin; it<=xmax; it++) {
    if (it<0) result.reset(it-xmin);
    if (it>rowsize ) result.reset(it-xmin);
    result[it-xmin] = m_bittable[sta][y][it]; 
  }
  return result;

}

void L0Muon::Tower::draw(MsgStream * log) {

  int sta;
  for (sta=0; sta<5; sta++ ) {
    drawStation(sta, log);
  }
}

void L0Muon::Tower::drawStation(int sta, MsgStream * log) {

  int xFoI = m_maxXFoI[sta];
  int yFoI = m_maxYFoI[sta];
  int offset = 24 - xFoI;
  
  int rows = 4+2*yFoI;
  int col = 24+2*xFoI;
  StationMap stmap = m_bittable[sta];

  int ir;
  std::string blanc(offset,' ');
  //*log << MSG::DEBUG << "Station: " << " " << (sta+1) << endreq;

  *log << MSG::DEBUG << "Station: " << " " << (sta) << endreq;
  for( ir = 0; ir < rows; ir++) {
    for( int ic = 0; ic < col; ic++) {
      //*log << MSG::INFO << blanc << stmap[ir][ic] ;
      *log << MSG::DEBUG << stmap[ir][ic] ;
 }
    *log << MSG::DEBUG << endreq;
    
  }
}


void L0Muon::Tower::processTower(MuonTileID & puID, MsgStream * log){

  *log << MSG::DEBUG << "Process Tower for PU" 
       << " " << "R" << puID.region() << ","
       <<"Q" << puID.quarter() <<","
       << puID.nX() << "," 
       << puID.nY() <<" " <<
       m_xfoi[0] <<" " 
       <<  m_yfoi[0] << endreq;
  int ncand =0;

  int nseed =0;
  std::vector< boost::dynamic_bitset<> >::iterator irow;

  *log << MSG::DEBUG << "clean seeds" << endreq;
  m_clean.cleanSeed(m_bittable[2]);

  m_puCandidates.clear();
  m_offForCand.clear();
  
  int row =0;
  for (irow =m_bittable[2].begin(); irow !=m_bittable[2].end();irow++){
    for (boost::dynamic_bitset<>::size_type icol = 0; 
          icol < (*irow).size(); icol++){
      if ( (*irow).test(icol)){

        nseed++;
        int col =icol;
        std::pair<int, int> sd = std::make_pair(col,row);
        for (int ista =0; ista<5;ista++){
          m_ctower.reset();
        }
        
        m_ctower.setSeed(sd);
        
         //Start Search for seed sd

        *log << MSG::DEBUG << "start search for seed " << " " << sd.first << " " << sd.second << endreq;
        
         CandidateSearch * pCs = new CandidateSearch();
         pCs->resetBits();
       
         for (int ista =4 ; ista >= 0; ista--){
           m_ctower.setFoi(ista, m_xfoi[ista], m_yfoi[ista]);
                   
           int offset=0;
           if (ista ==0) { 
             offset = pCs->makeExtrapolation(log) ;
           } 
           if (m_maxYFoI[ista]==0){
             m_ctower.setBit(ista, m_bittable[ista], m_maxXFoI[ista],
                             m_maxYFoI[ista], offset);
           } else {
             m_ctower.setOrderedBit(ista, m_bittable[ista], m_maxXFoI[ista],
                                  m_maxYFoI[ista], offset);
           }
           
           m_ctower.drawBits(ista, log);
           drawStation(ista, log);
           ///boost::dynamic_bitset<> bits = m_ctower.getBit(ista);
           boost::dynamic_bitset<> bits = m_ctower.getBit();
           pCs->searchInSta(ista, bits); 
           //m_ctower.setPadIndex(ista,m_maxXFoI[ista], m_maxYFoI[ista],
           //                   offset, pCs->getHitPos(ista));
          
             
           m_ctower.setOrderedPadIndex(ista,m_maxXFoI[ista], m_maxYFoI[ista],
                                offset, pCs->getHitPos(ista), log);
 
           if (pCs->hitFoundInSta(ista) == false) {
             break ;
           }
           
           
           
         }
         
         

         if (pCs->CandidateFound()) {
           *log << MSG::DEBUG << "candidate found for seed " 
                << " " << sd.first << " " << sd.second << endreq;
           
           *log << MSG::DEBUG << "Tower for PU" 
                << " " << "R" << puID.region() << ","
                <<"Q" << puID.quarter() <<","
                << puID.nX() << "," 
                << puID.nY() <<" " << endreq;

           ncand = ncand++;
           //pCs->setCandidateAddrs(sd);
           ptcalc(log);
           //boost::dynamic_bitset<> bitsset = pCs->getCandidateAddrs();
           L0MuonCandidate * mycand =createCandidate(m_pt, m_theta ,m_phi, 
                                                     L0Muon::OK);
         

           m_puCandidates.push_back(mycand);

           //offset for ntuple
           std::vector<int> offsetx;
           std::pair<int, int> tmp[5];
           
           for (int ist = 0; ist <5; ist++){
             *log << MSG::DEBUG << "Filling offsets for sta" << " " 
                  << ist << endreq;
             //*log << MSG::DEBUG << "maxXFoi" <<" " << m_maxXFoI[ist]
             //   << endreq;
             //*log << MSG::DEBUG << "maxYFoi" <<" " << m_maxYFoI[ist]
             //   << endreq;
             //*log << MSG::DEBUG << "xFoi" <<" " << m_xfoi[ist]
             //   << endreq;
             //*log << MSG::DEBUG << "xFoi" <<" " << m_yfoi[ist]
             //   << endreq;


             if ( ist ==0){
               tmp[ist] = m_ctower.getPadIndex(ist);
               int extrap = pCs->makeExtrapolation(log) ;
               offsetx.push_back(tmp[ist].second -
                                     (sd.first+m_maxXFoI[ist]));
               
               offsetx.push_back(extrap);
               *log << MSG::DEBUG << "pad x index" <<" " << 
                 tmp[ist].second << endreq;

               *log << MSG::DEBUG << "pad y index" <<" " << 
                 tmp[ist].first << endreq;

               *log << MSG::DEBUG << "offset" <<" " << 
                 tmp[ist].second -(sd.first+m_maxXFoI[ist])<< endreq;  
     

             }
             
             if (ist == 1){
               tmp[ist] = m_ctower.getPadIndex(ist);
               offsetx.push_back(tmp[ist].second -
                                 (sd.first+m_maxXFoI[ist]));
               *log << MSG::DEBUG << "pad x index" <<" " << 
                 tmp[ist].second << endreq;

               *log << MSG::DEBUG << "pad y index" <<" " << 
                 tmp[ist].first << endreq;

               *log << MSG::DEBUG << "offset" <<" " << 
                 tmp[ist].second -(sd.first+m_maxXFoI[ist])<< endreq;
               
             }
             
           


             if (ist ==2){
               offsetx.push_back(0);
              
             }
             
             if (ist == 3 || ist ==4){

               tmp[ist] = m_ctower.getPadIndex(ist);
               int offy =tmp[ist].first -(sd.second);
               *log << MSG::DEBUG << "tmp.first" <<" " << tmp[ist].first
                    << endreq; 
               *log << MSG::DEBUG << "sd.second" <<" " << sd.second
                    << endreq;
               *log << MSG::DEBUG << "offsety" <<" " << 
                 tmp[ist].first -sd.second<< endreq;
               //hit nella stessa linea del seme
               if (offy == 0){
                 offsetx.push_back(tmp[ist].second - 
                                   (sd.first+m_maxXFoI[ist]));
                     *log << MSG::DEBUG << "hit pos in bitset" << " " << 
                          pCs->getHitPos(ist) << endreq;
                     
                     *log << MSG::DEBUG << "pad x index" <<" " << 
                       tmp[ist].second << endreq;

                     *log << MSG::DEBUG << "pad y index" <<" " <<
                       tmp[ist].first << endreq;

                     *log << MSG::DEBUG << "offset" <<" " << 
                       tmp[ist].second -(sd.first+m_maxXFoI[ist])<< endreq;


                 
                 offsetx.push_back(999);
                 offsetx.push_back(999);
                 
               } else if (offy > 0){
               offsetx.push_back(999);
               offsetx.push_back(999);

               offsetx.push_back(tmp[ist].second -
                               (sd.first+m_maxXFoI[ist]));

            
               *log << MSG::DEBUG << "hit pos in bitset" << " " << 
                 pCs->getHitPos(ist) << endreq;
               
               *log << MSG::DEBUG << "pad x index" <<" " << 
                 tmp[ist].second << endreq;

               *log << MSG::DEBUG << "pad y index" <<" " << 
                 tmp[ist].first << endreq;

               *log << MSG::DEBUG << "offset" <<" " << 
                 tmp[ist].second -(sd.first+m_maxXFoI[ist])<< endreq;

               

               } else if (offy < 0){
                 offsetx.push_back(999);
                 offsetx.push_back(tmp[ist].second -
                                 (sd.first+m_maxXFoI[ist]));
                 

                 offsetx.push_back(999);

                 *log << MSG::DEBUG << "hit pos in bitset" << " " << 
                   pCs->getHitPos(ist) << endreq;
                                 
                 *log << MSG::DEBUG << "pad x index" <<" " << 
                   tmp[ist].second << endreq;
            
                 *log << MSG::DEBUG << "pad y index" <<" " << 
                   tmp[ist].first << endreq;
                
                 *log << MSG::DEBUG << "offset" <<" " << 
                   tmp[ist].second -(sd.first+m_maxXFoI[ist])<< endreq;
               }
               
               
             
             
               
             }
             
           }
           
                     
           m_offsetx =std::make_pair(mycand, offsetx);
           
           m_offForCand.push_back(m_offsetx);
                   
           
         }      
 
         delete pCs;
         
         
      }
       
         
    }
    
    
    row++;
    
  }
  

  m_ncand = ncand;
}

  






L0MuonCandidate* L0Muon::Tower::createCandidate(double p, double th, 
                                                double phi,int flag) {

  std::vector<MuonTileID> m_mtid;
  std::pair<int, int> ind[5];
  for (int j =0; j<5; j++){
    ind[j] =  m_ctower.getPadIndex(j);
    m_mtid.push_back(getPadIdMap(j,ind[j]));
    
  }
  
  //std::vector<MuonTileID>::iterator itmp;
  

    L0MuonCandidate* lmc = new L0MuonCandidate(p,
                                               th, phi, m_mtid, flag) ;	   
    return lmc;
}



double L0Muon::Tower::ptcalc(MsgStream * log) {

  double d1 =m_ptparam[0];
  double d2 =m_ptparam[1]; 
  double d3 =m_ptparam[2];
  double alpha = m_ptparam[3];

  int scode;

  double x1;
  double y1;
  double x2;  
  double dx,dy,y,z,dz;

  
  std::pair<int, int> yxM1= m_ctower.getPadIndex(0);
 
  MuonTileID p1 = getPadIdMap(0, yxM1);
   
  MuonSystemLayout  layout=
    MuonSystemLayout(MuonStationLayout(MuonLayout(24,8)),
                     MuonStationLayout(MuonLayout(48,8)),
                     MuonStationLayout(MuonLayout(48,8)),
                     MuonStationLayout(MuonLayout(12,8)),
                     MuonStationLayout(MuonLayout(12,8)));

  std::vector<MuonTileID> tmp;
  std::vector<MuonTileID>::iterator itmp;
  if ( ! layout.isValidID(p1)){
    *log << MSG::DEBUG <<"layout for p1 is not valid" << endreq;

    *log << MSG::DEBUG <<"prima di tiles" << endreq;
    tmp = layout.tiles(p1);
    *log << MSG::DEBUG <<"dopo tiles" << endreq;
    double mx =0.;
    double my =0.;
    int count = tmp.size();
    *log << MSG::DEBUG <<"tmp size" << " " << count << endreq;
    
    //scode = m_iTileXYZTool->calcTilePosL0Muon(p1,x1,dx,y1,dy,z,dz);
    //*log << MSG::DEBUG << "x1 " << " " << x1 << endreq;
    for (itmp = tmp.begin(); itmp != tmp.end(); itmp++){
      MuonTileID p = layout.contains(*itmp);
     
      
      *log << MSG::DEBUG <<"tiles contained in layout" << endreq;
      *log << MSG::DEBUG <<"quarter"<< " " << p.quarter() << endreq;
      *log << MSG::DEBUG <<"region"<< " " << p.region() << endreq;
      *log << MSG::DEBUG <<"nX"<< " " << p.nX() << endreq;
      *log << MSG::DEBUG <<"nY"<< " " << p.nY() << endreq;
      scode = m_iTileXYZTool->calcTilePos(p,x1,dx,y1,dy,z,dz);  
      mx += x1;
      my += y1;
      
    }
    x1 = mx/count;
    y1 = my/count;
    *log << MSG::DEBUG << "x1 " << " " << x1 << endreq;
    
  } else {
    *log << MSG::DEBUG <<"layout for p1 is valid" << endreq;
    scode = m_iTileXYZTool->calcTilePos(p1,x1,dx,y1,dy,z,dz);
  }
  

  *log << MSG::DEBUG << "xy " << yxM1.first << " " << yxM1.second  << " " 
       << "MUONTILE ID IN M1 Q" << p1.quarter() << " R" << p1.region() << "  nx" << p1.nX() << " ny" << p1.nY()<< endreq;
  //<< "MUONTILE ID IN M1" << p1 << endreq;

  std::pair<int, int> yxM2= m_ctower.getPadIndex(1);
  MuonTileID p2 = getPadIdMap(1, yxM2);

  if ( ! layout.isValidID(p2)){
    *log << MSG::DEBUG <<"layout for p2 is not valid" << endreq;
    tmp = layout.tiles(p2);
    double mx =0.;
    double my =0.;
    int count = tmp.size();

    *log << MSG::DEBUG <<"tmp size" << " " << count << endreq;
    
    for (itmp = tmp.begin(); itmp != tmp.end(); itmp ++){
      MuonTileID p = layout.contains(*itmp);
      scode = m_iTileXYZTool->calcTilePos(p,x2,dx,y,dy,z,dz);  
      mx += x2;
      my += y;
      
    }
    x2 = mx/count;
    y = my/count;
    
  } else {
    scode = m_iTileXYZTool->calcTilePos(p2,x2,dx,y,dy,z,dz);
  }


  *log << MSG::DEBUG << "xy " << yxM2.first << " " << yxM2.second  << " " 
       << "MUONTILE ID IN M2 Q" << p2.quarter() << " R" << p2.region() << "  nx" << p2.nX() << " ny" << p2.nY()<< endreq;
  //<< "MUONTILE ID IN M2" << p2 << endreq;  

  std::pair<int, int> yxM3= m_ctower.getPadIndex(2);
  MuonTileID p3 = getPadIdMap(2, yxM3);

  *log << MSG::DEBUG << "xy " << yxM3.first << " " << yxM3.second  << " " 
       << "MUONTILE ID IN M3 Q" << p3.quarter() << " R" << p3.region() << "  nx" << p3.nX() << " ny" << p3.nY()<< endreq;


  x1 /= 10.;
  y1 /= 10.;
  x2 /= 10.;
  
  
  double x0 = x1 - d2*(x2-x1)/d3;
  double y0 = y1*d1/(d1+d2);
        
  double sq = (d1+d2)*x2-(d1+d2+d3)*x1;
  if(sq == 0.) {
   sq = 0.0001;
  } else {
    sq = 1./((d1+d2)*x2-(d1+d2+d3)*x1);
  }

  double sr = sqrt(((d2+d3)*x1 - d2*x2)*((d2+d3)*x1 - d2*x2) +
                   (d1*d3*y1/(d1+d2))*(d1*d3*y1/(d1+d2))   );
  double sc = sqrt((d1+d2)*(d1+d2)+y1*y1)/(d1+d2);
  double st = (d1*d3*d3+2.*(x1*(d3+d2)-x2*d2)*(x2-x1))/(d1*d3*d3);
  double ptm = fabs(alpha*sq*sr*sc*st);		     

  double xslope = (x2 - x1)/d3;
  double xin = x2 - xslope*(d1+d2+d3);
  if(xin < 0.) ptm = -ptm;

  // Pt should be in MeV
  m_pt = ptm*1000.;
  Hep3Vector v(x0,y0,d1);
  m_theta = v.theta();
  m_phi = v.phi();
  
  *log << MSG::DEBUG <<"Momentum" <<  " " <<  m_pt << endreq;
  *log << MSG::DEBUG << "Theta" << " " << m_theta << endreq;
  *log << MSG::DEBUG << " Phi" << " " << m_phi << endreq;

  return m_pt;

  
}
