//#include <time.h>
#include <cmath>
#include <set>
#include <vector>
#include <algorithm>

//#include "CLHEP/Vector/ThreeVector.h"
//#include "GaudiKernel/IToolSvc.h" 
//#include "MuonTools/IMuonTileXYZTool.h"

#include "ProcessorKernel/RegisterFactory.h"
#include "L0MuonKernel/Tower.h"
#include "L0MuonKernel/Candidate.h"


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


  for (int i =0; i<5;i++){
    if ( m_idmap[i].size()>0){      
      m_idmap[i].clear();
    }
  }
  
  m_ncand = 0;
  
  int sta;
  int row;
  for ( sta = 0; sta < 5; sta++) {
    boost::dynamic_bitset<> tmprow(24+2*m_maxXFoI[sta]);
    for ( row = 0; row < 4+2*m_maxYFoI[sta]; row++ ) {
      m_bittable[sta].push_back(tmprow); 
    }
  }

  m_pt    = 0.;
  m_theta = 0.;
  m_phi   = 0.;

  std::vector<double> tmpptparam;
  for (int i=0; i<5; i++){
    tmpptparam.push_back(0);
  }
  
  m_ptparam= tmpptparam;

  m_ignoreM1=false;
  
  m_debug = false;
  m_seeded = false; 
 
  m_ctower = CandidateTower();
  // Should be done once at creation time
  for (int ista =0 ; ista <5 0; ista++){
    m_ctower.setFoi(ista, m_xfoi[ista], m_yfoi[ista]);
  }
                 
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
  m_seeded = false;
}


void L0Muon::Tower::setBit(int sta, int row, int col) {
  int xFoI = m_maxXFoI[sta];
  int yFoI = m_maxYFoI[sta];
  
  if (col < 0 || col >= (24+2* xFoI) ) {
    if (m_debug) std::cout << "Column is not valid !" << std::endl;
    if (m_debug) std::cout << "Station: " << sta << " Col: " << col << std::endl;
    return;	      
  }
  if (row < 0 || row >= (4+2*yFoI) ) {
    if (m_debug) std::cout << "Row is not valid ! " << std::endl;
    if (m_debug) std::cout << "Station: " << sta << " Row: " << row << std::endl;
    return;	      
  }
  
  
  m_bittable[sta][row].set(col);
  if (sta == 2) m_seeded = true;
  
}


void L0Muon::Tower::setPadIdMap(int sta, 
                                std::pair<int, int> YX, 
                                MuonTileID mid)
{
  //for (int i=0; i <5; i++){
  //if (i==sta){
      
  m_idmap[sta][YX]= mid ;
  //}
  //}
  
}

MuonTileID L0Muon::Tower::getPadIdMap(int sta, std::pair<int, int> XY)
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

void L0Muon::Tower::draw() {

  int sta;
  for (sta=4; sta>=0; sta-- ) {
    drawStation(sta);
  }
  std::cout << std::endl;
}

void L0Muon::Tower::drawStation(int sta) {

  int xFoI = m_maxXFoI[sta];
  int yFoI = m_maxYFoI[sta];
  int offset = 24 - xFoI;
  
  int rows = 4+2*yFoI;
  int col = 24+2*xFoI;
  StationMap stmap = m_bittable[sta];

  int ir;
  std::string blanc(offset,' ');
  std::cout << "Station: " << " " << (sta+1) << std::endl;

  for( ir = rows - 1; ir >=0 ; ir--) {
    std::cout <<  blanc;
    for( int ic = 0; ic < col; ic++) {
      if (stmap[ir][ic]) {
        std::cout <<  "X" ;
      } else {
        std::cout <<  "_" ;
      }		     
    }
    std::cout << std::endl;    
  }
}


void L0Muon::Tower::processTower(MuonTileID & puID){

  if (m_debug) std::cout << "Tower: process Tower for PU" 
			 << " " << "R" << puID.region() << ","
			 <<"Q" << puID.quarter() <<","
			 << puID.nX() << "," 
			 << puID.nY() <<" " <<
		 m_xfoi[0] <<" " 
			 <<  m_yfoi[0] << std::endl;

  int ncand =0;
  int nseed =0;
  
  if (m_seeded && m_debug ) draw();

  if (m_seeded) {
    if (m_debug) std::cout << "Tower: cleaning seeds" << std::endl;
    cleanSeed(m_bittable[2]);
  }  
  //if (m_seeded && m_debug ) draw();

  m_puCandidates.clear();
  m_offForCand.clear();

  // added to set addresses for L0Buffer
  for (int icand=0; icand<2; icand++) {  
    m_addr[icand].resize(15);    
    for (boost::dynamic_bitset<>::size_type i =0; i<15;i++){
      m_addr[icand][i]=0; 
    } 
  }
  //======

  // 
  // Start track search
  // ------------------

  //
  // Loop over seeds (pads hit in M3)
  //
  int row =0;
  std::vector< boost::dynamic_bitset<> >::iterator irow;
  // Loop over the rows
  for (irow =m_bittable[2].begin(); irow !=m_bittable[2].end();irow++){
    // Loop over the columns
    for (boost::dynamic_bitset<>::size_type icol =0;icol<(*irow).size(); icol++){      
      // if the pad is hit 
      if ( (*irow).test(icol)){

        nseed++;
        int col =icol;
        std::pair<int, int> sd = std::make_pair(col,row);
	m_ctower.reset();
        m_ctower.setSeed(sd);
        
        //Start Search for seed sd

        if (m_debug) std::cout << "Tower: start search for seed " 
			       << " " << sd.first << "," << sd.second << std::endl;

	// Construct a new CandidateSearch
        CandidateSearch * pCs = new CandidateSearch();
        pCs->ignoreM1(m_ignoreM1);
        pCs->resetBits(); // ???? (à peine créer et déjà reseter !)
         
        // Loop over stations (M5,M4,M2 and eventually M1)
        int minsta = (m_ignoreM1) ? 1 : 0;
        for (int ista =4 ; ista >= minsta; ista--){
	  if (m_debug) std::cout << "Tower: look for hits in station  M"<<ista+1 << std::endl;
	  
	  // Compute extrapolation in M1
          int offset=0;             
          if (ista ==0) { 
            offset = pCs->makeExtrapolation() ;
	    if (m_debug) std::cout << "Tower: Extrapolation in station  M"<<ista+1 << "  offset:" <<offset <<std::endl;
          }

	  // if (m_debug) {
	  //  std::cout << "Tower:  m_bittable[ista]" << std::endl;
	  //  StationMap v = m_bittable[ista];
	  //  for (StationMap::iterator  iv=v.begin(); iv < v.end(); iv++) {
	  //    std::cout << (*iv) <<std::endl;
	  //  }
	  //}
	  //if (m_debug) std::cout << "Tower:  xfoi " << m_maxXFoI[ista] << " "<<m_xfoi[ista] <<std::endl;
	  //if (m_debug) std::cout << "Tower:  yfoi " << m_maxYFoI[ista] << " "<<m_yfoi[ista] <<std::endl;
	  
	  // ??? 
          if (m_maxYFoI[ista]==0){
            m_ctower.setBit(ista, m_bittable[ista], m_maxXFoI[ista],m_maxYFoI[ista], offset);
          } else {
            m_ctower.setOrderedBit(ista, m_bittable[ista], m_maxXFoI[ista],m_maxYFoI[ista], offset);
          }
             

          boost::dynamic_bitset<> bits = m_ctower.getBit();
          pCs->searchInSta(ista, bits); 
          m_ctower.setOrderedPadIndex(ista,m_maxXFoI[ista], m_maxYFoI[ista],
                                      offset, pCs->getHitPos(ista));
          
	  //if (m_debug) std::cout << "Tower: bits= " << bits << std::endl;
	  //if (m_debug) std::cout << "Tower: found hit in station M" << ista+1 << " " << pCs->getHitPos(ista) << std::endl;
	  //if (m_debug) std::cout << "Tower: flag= " << pCs->hitFoundInSta(ista)    << std::endl; 
          if (pCs->hitFoundInSta(ista) == false) {
            break ;
          }
        } // end of loop over stations

	// If a candidate has been found
        if (pCs->CandidateFound()) { 

	  //if (m_debug) std::cout << "candidate found for seed " 
	  //   << " " << sd.first << " " << sd.second << std::endl;
           
	  //if (m_debug) std::cout << "Tower for PU" 
	  //   << " " << "R" << puID.region() << ","
	  //   <<"Q" << puID.quarter() <<","
	  //   << puID.nX() << "," 
	  //   << puID.nY() <<" " << std::endl;

	  // Compute PT if a candidate has been found
          if (m_ignoreM1 ){           
            ptcalcIgnoreM1();

	    //m_pt =0;
	    //m_theta =0;
	    //m_phi =0;
          } else if ( ! m_ignoreM1){           
            ptcalc();
          }
           
	  //boost::dynamic_bitset<> bitsset = pCs->getCandidateAddrs();
          Candidate * mycand =createCandidate(m_pt, m_theta ,m_phi, 
					      L0MuonStatus::OK);

          m_puCandidates.push_back(mycand);

          //=================================================
          //added to set addresses for L0Buffer
          pCs->setCandidateAddrs(sd);
          boost::dynamic_bitset<> bitsset = pCs->getCandidateAddrs();
          
          if (ncand<2) {
            m_addr[ncand]=bitsset;
          }
          
          ncand = ncand++;
          

          //===============================
	  //offset for ntuple
          std::vector<int> offsetx;
          std::pair<int, int> tmp[5];

          for (int ist = 0; ist <5; ist++){

	    //if (m_debug) std::cout << "Filling offsets for sta" << " " 
	    //   << ist << std::endl;

               
            if ( ist ==0){
              if ( ! m_ignoreM1 ){
                tmp[ist] = m_ctower.getPadIndex(ist);
                int extrap = pCs->makeExtrapolation() ;
                offsetx.push_back(tmp[ist].second -
                                  (sd.first+m_maxXFoI[ist]));
               
                offsetx.push_back(extrap);
		//*if (m_debug) std::cout << "pad x index" <<" " << 
		//tmp[ist].second << std::endl;
                 
		//*if (m_debug) std::cout << "pad y index" <<" " << 
		//tmp[ist].first << std::endl;

		//*if (m_debug) std::cout << "offset" <<" " << 
		//tmp[ist].second -(sd.first+m_maxXFoI[ist])<< std::endl;  
     
                
              } else if ( m_ignoreM1 ){
                offsetx.push_back(31);
                int extrap = pCs->makeExtrapolation() ;
                offsetx.push_back(extrap);
              }
              
            }
            if (ist == 1){
              tmp[ist] = m_ctower.getPadIndex(ist);
              offsetx.push_back(tmp[ist].second -
                                (sd.first+m_maxXFoI[ist]));
	      //*if (m_debug) std::cout << "pad x index" <<" " << 
	      //tmp[ist].second << std::endl;

	      //*if (m_debug) std::cout << "pad y index" <<" " << 
	      //tmp[ist].first << std::endl;

	      //*if (m_debug) std::cout << "offset" <<" " << 
	      //tmp[ist].second -(sd.first+m_maxXFoI[ist])<< std::endl;
               
            }
            if (ist ==2){
              offsetx.push_back(0);
            }
             
            if (ist == 3 || ist ==4){

              tmp[ist] = m_ctower.getPadIndex(ist);
              int offy =tmp[ist].first -(sd.second);
	      //*if (m_debug) std::cout << "tmp.first" <<" " << tmp[ist].first
	      //<< std::endl; 
	      //*if (m_debug) std::cout << "sd.second" <<" " << sd.second
	      //   << std::endl;
	      //*if (m_debug) std::cout << "offsety" <<" " << 
	      //tmp[ist].first -sd.second<< std::endl;
	      //hit nella stessa linea del seme
              if (offy == 0){
                offsetx.push_back(tmp[ist].second - 
                                  (sd.first+m_maxXFoI[ist]));
		//*if (m_debug) std::cout << "hit pos in bitset" << " " << 
		//       pCs->getHitPos(ist) << std::endl;
                     
		//  *if (m_debug) std::cout << "pad x index" <<" " << 
		//    tmp[ist].second << std::endl;

		//   *if (m_debug) std::cout << "pad y index" <<" " <<
		//    tmp[ist].first << std::endl;

		//                     *if (m_debug) std::cout << "offset" <<" " << 
		//    tmp[ist].second -(sd.first+m_maxXFoI[ist])<< std::endl;


                 
                offsetx.push_back(999);
                offsetx.push_back(999);
                 
              } else if (offy > 0){
                offsetx.push_back(999);
                offsetx.push_back(999);

                offsetx.push_back(tmp[ist].second -
                                  (sd.first+m_maxXFoI[ist]));

            
		//*if (m_debug) std::cout << "hit pos in bitset" << " " << 
		//pCs->getHitPos(ist) << std::endl;
               
		//*if (m_debug) std::cout << "pad x index" <<" " << 
		//tmp[ist].second << std::endl;

		//*if (m_debug) std::cout << "pad y index" <<" " << 
		//tmp[ist].first << std::endl;

		//*if (m_debug) std::cout << "offset" <<" " << 
		//tmp[ist].second -(sd.first+m_maxXFoI[ist])<< std::endl;

               
                
              } else if (offy < 0){
                offsetx.push_back(999);
                offsetx.push_back(tmp[ist].second -
                                  (sd.first+m_maxXFoI[ist]));
                 

                offsetx.push_back(999);

		//*if (m_debug) std::cout << "hit pos in bitset" << " " << 
		//pCs->getHitPos(ist) << std::endl;
                                 
		//*if (m_debug) std::cout << "pad x index" <<" " << 
		//tmp[ist].second << std::endl;
            
		//*if (m_debug) std::cout << "pad y index" <<" " << 
		//tmp[ist].first << std::endl;
                
		// *if (m_debug) std::cout << "offset" <<" " << 
		//tmp[ist].second -(sd.first+m_maxXFoI[ist])<< std::endl;
              }
            }
          }
          m_offsetx =std::make_pair(mycand, offsetx);  
          m_offForCand.push_back(m_offsetx);
	} // end of candidate found
        delete pCs;
      } // end of hit pad
    } // end of loop over columns
    row++;
  } // end of loop over rows
  
  
  m_ncand = ncand;
  
}

  






L0Muon::Candidate* L0Muon::Tower::createCandidate(double p, double th, 
						  double phi,int flag) {

  std::vector<MuonTileID> m_mtid;
  std::pair<int, int> ind[5];
  for (int j =0; j<5; j++){    
    if (j==0){
      if ( m_ignoreM1){
	m_mtid.push_back(MuonTileID());
      } else if ( ! m_ignoreM1) {
        ind[j] =  m_ctower.getPadIndex(j);
        m_mtid.push_back(getPadIdMap(j,ind[j]));
      }
      
      
      
    } else if (j!=0){
      if (j==3 || j==4){           
	ind[j] =  m_ctower.getPadIndex(j);
       
	std::pair<int, int> tmpind = std::make_pair(ind[j].first+1,ind[j].second);
	m_mtid.push_back(getPadIdMap(j,tmpind));       
      } else {
       
	ind[j] =  m_ctower.getPadIndex(j);
       
	//std::pair<int, int> tmpind = std::make_pair(ind[j].first+1,ind[j].second);
	m_mtid.push_back(getPadIdMap(j,ind[j]));
     
      }
     
       
     
    }
   
   
       
   
  }
  
  
  
  
   
  
  
  //std::vector<MuonTileID>::iterator itmp;
  

  Candidate* lmc = new Candidate(p,
				 th, phi, m_mtid, flag) ;	   
  return lmc;
}


void L0Muon::Tower::xyFromPad(MuonTileID pad, double& x, double& y)  {

  double dx = 1.0;
  double dy = 2.5;
  double l1 = 1210.;
  double l2 = 1527.;
  double l3 = 1647.;
    
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


double L0Muon::Tower::ptcalc() {

    
  double d1 =m_ptparam[0];
  double d2 =m_ptparam[1]; 
  double d3 =m_ptparam[2];
  double alpha = m_ptparam[3];

  double x1=0., y1=0.;
  double x2=0., y2=0. ;  
    
  std::pair<int, int> yxM1= m_ctower.getPadIndex(0); 
  MuonTileID p1 = getPadIdMap(0, yxM1);

  std::pair<int, int> yxM2= m_ctower.getPadIndex(1);
  MuonTileID p2 = getPadIdMap(1, yxM2);
  
  xyFromPad(p1,x1,y1);
  xyFromPad(p2,x2,y2);
  
  //std::cout << "xyFromPad _1: " << p1.toString() << std::endl;
  //std::cout << "xyFromPad _2: " << p2.toString() << std::endl;
   
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
  m_pt = ptm*1000.;
  //   Hep3Vector v(x0,y0,d1);
  //   m_theta = v.theta();
  //   m_phi = v.phi();
  
  //*if (m_debug) std::cout <<"Momentum" <<  " " <<  m_pt << std::endl;
  //*if (m_debug) std::cout << "Theta" << " " << m_theta << std::endl;
  //*if (m_debug) std::cout << " Phi" << " " << m_phi << std::endl;


  //m_pts.push_back(m_pt);
  //m_th.push_back(m_theta);
  //m_ph.push_back(m_phi);

  return m_pt;

  
}


double L0Muon::Tower::ptcalcIgnoreM1() {

  double d1 =m_ptparam[0];
  double d2 =m_ptparam[1] + m_ptparam[2]; 
  double d3 =m_ptparam[4];
  double alpha = m_ptparam[3];

  double x1=0., y1=0.;
  double x2=0., y2=0. ;  
    
  std::pair<int, int> yxM1= m_ctower.getPadIndex(0); 
  MuonTileID p1 = getPadIdMap(1, yxM1);

  std::pair<int, int> yxM2= m_ctower.getPadIndex(1);
  MuonTileID p2 = getPadIdMap(2, yxM2);
  
  xyFromPad(p1,x1,y1);
  xyFromPad(p2,x2,y2);
  
 
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
  // Hep3Vector v(x0,y0,d1);
  //   m_theta = v.theta();
  //   m_phi = v.phi();
  
  //*if (m_debug) std::cout <<"Momentum" <<  " " <<  m_pt << std::endl;
  //*if (m_debug) std::cout << "Theta" << " " << m_theta << std::endl;
  //*if (m_debug) std::cout << " Phi" << " " << m_phi << std::endl;

  return m_pt; 
}

void L0Muon::Tower::cleanSeed(L0Muon::Tower::StationMap & map) {

  std::vector< boost::dynamic_bitset<> >::iterator irow;

  for (irow = map.begin(); irow != map.end(); irow ++){

    for (boost::dynamic_bitset<>::size_type icol=(*irow).size(); icol >=2 ; icol--){
      if ( (*irow).test(icol-2) ) {
        (*irow).reset(icol-1);
      }	        
    }           
  }              
}
