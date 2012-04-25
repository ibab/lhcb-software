#include <cmath>
#include <set>
#include <vector>
#include <algorithm>

#include "L0MuonKernel/Tower.h"

#include "L0MuonKernel/ProcUtilities.h"

L0Muon::Tower::Tower() {

  // Init the FOI MAX (set the size of the tower)
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

  // Init the idmap
  for (int i =0; i<5;i++){
    if ( m_idmap[i].size()>0){      
      m_idmap[i].clear();
    }
  }
  
  // Init the bittable 
  int sta;
  int row;
  for ( sta = 0; sta < 5; sta++) {
    boost::dynamic_bitset<> tmprow(24+2*m_maxXFoI[sta]);
    for ( row = 0; row < 4+2*m_maxYFoI[sta]; row++ ) {
      m_bittable[sta].push_back(tmprow); 
    }
  }

  m_ignoreM1=false;
  m_ignoreM2=false;
  m_debug = false;

  m_lut=0;
}

L0Muon::Tower::~Tower() {}

void L0Muon::Tower::reset() {

  int sta;
  int row;
  for ( sta = 0; sta < 5; sta++ ) {
    for ( row = 0; row < 4+2*(m_maxYFoI[sta]); row++ ) {
      m_bittable[sta][row].reset();
    }
    if ( (sta!=0 || m_ignoreM1==false) ) m_idmap[sta].clear();
  }
}


void L0Muon::Tower::setBit(int sta, int row, int col) {
  // Fill the m_bittable with the fired pads.
  // m_bittable represent a tower in the PU local frame
  // with M3 granularity in each chamber.

  int xFoI = m_maxXFoI[sta];
  int yFoI = m_maxYFoI[sta];
  
  if (col < 0 || col >= (24+2* xFoI) ) {
    if (m_debug) std::cout << "--- Tower::setBit WARNING Column is not valid !" << std::endl;
    if (m_debug) std::cout << "--- Tower::setBit -> Station: " << sta << " Col: " << col <<" min=0 - max="<<24+2*xFoI<< std::endl;
    return;	      
  }
  if (row < 0 || row >= (4+2*yFoI) ) {
    if (m_debug) std::cout << "--- Tower::setBit WARNING Row is not valid ! " << std::endl;
    if (m_debug) std::cout << "--- Tower::setBit -> Station: " << sta << " Row: " << row <<" min=0 - max="<<4+2*yFoI<< std::endl;
    return;	      
  }
  
  if (m_debug) std::cout << "--- Tower::setBit SET Station: " << sta << " Row: " << row << std::endl;
  
  m_bittable[sta][row].set(col);
  
}


void L0Muon::Tower::setPadIdMap(int sta, 
                                std::pair<int, int> YX, 
                                LHCb::MuonTileID mid)
{
  m_idmap[sta][YX]= mid ;
}

LHCb::MuonTileID L0Muon::Tower::getPadIdMap(int sta, std::pair<int, int> XY)
{
  return m_idmap[sta].find(XY)->second ;
  
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


std::vector<L0Muon::PMuonCandidate> L0Muon::Tower::processTower(LHCb::MuonTileID & puID){

  std::vector<L0Muon::PMuonCandidate> puCandidates;
  if (m_debug) std::cout <<"--- Tower::processTower: Process Tower for PU " 
                         <<"Q"  << puID.quarter()+1 
                         <<"R"  << puID.region()+1 
                         << puID.nX()
                         << puID.nY() <<" foi:" 
                         << m_xfoi[0] <<" ," 
                         << m_yfoi[0] << std::endl;

  
  // Draw tower
  if (m_debug ) draw();


  // Seed cleaning
  if (m_debug) std::cout <<"--- Tower: Cleaning seeds" << std::endl;
  cleanSeed(m_bittable[2]);


  if (m_debug) std::cout <<"--- Tower::processTower: Start track search "<< std::endl;
  // Start track search
  int rowseed =0;
  std::vector< boost::dynamic_bitset<> >::iterator rowM3;
  // Loop over rows of M3
  for (rowM3 =m_bittable[2].begin(); rowM3 !=m_bittable[2].end();rowM3++){
    if (m_debug) std::cout <<"--- Tower::processTower:  M3 row= "<<rowseed<< std::endl;

    // Loop over columns of the M3 row
    for (boost::dynamic_bitset<>::size_type colM3 = 0; 
         colM3 < (*rowM3).size(); colM3++){  

      // If the M3 bit is set; a seed is found
      if ( (*rowM3).test(colM3)){

        int colseed =colM3;

        // Iterators over bittable rows
        std::vector< boost::dynamic_bitset<> >::iterator rowInd; 
        std::vector< boost::dynamic_bitset<> >::iterator rowCentral;
        // A bittable row
        boost::dynamic_bitset<> field;
        // Iterators over the columns of a bittable row
        boost::dynamic_bitset<>::size_type colInd;
        boost::dynamic_bitset<>::size_type colCentral;
        // Station index
        int sta;
 
        if (m_debug) std::cout <<"--- Tower::processTower: Seed found col,row= "<<colseed<<" "<<rowseed<< std::endl;

        // Flag candidate
        bool candFlag=true;

       // Stations M4 and M5
        for (sta=3; sta<5; sta++){
          if (m_debug) std::cout <<"--- Tower::processTower: Search in station sta= "<<sta<< std::endl;
          if (m_debug) std::cout <<"--- Tower::processTower: Max foi X,Y= "<<m_maxXFoI[sta]<<" , "<<m_maxYFoI[sta]<< std::endl;
          if (m_debug) std::cout <<"--- Tower::processTower:     foi X,Y= "<<m_xfoi[sta]<<" , "<<m_yfoi[sta]<< std::endl;
          // Loop over rows     
          rowCentral = m_bittable[sta].begin()+rowseed+m_maxYFoI[sta];
          if (m_debug) {  
            std::cout <<"--- Tower::processTower:   bit set M"<<(sta+1)<< std::endl;
            for (rowInd=rowCentral-m_yfoi[sta];
                 rowInd<=rowCentral+m_yfoi[sta];
                 rowInd++) {
              std::cout <<"--- Tower::processTower:   "<<(*rowInd)<< std::endl;
            }
          }
          for (rowInd=rowCentral-m_yfoi[sta];
               rowInd<=rowCentral+m_yfoi[sta];
               rowInd++) {
            candFlag=true;
            if (m_debug) std::cout <<"--- Tower::processTower:   full bitset size is "<<(*rowInd).size()<< std::endl;
            if (m_debug) std::cout <<"--- Tower::processTower:   full bitset is "<<(*rowInd)<< std::endl;
            field = (*rowInd)>>(colseed+m_maxXFoI[sta]-m_xfoi[sta]);
            if (m_debug) std::cout <<"--- Tower::processTower:   shift is "<< colseed+m_maxXFoI[sta]-m_xfoi[sta] 
                                   << " ( " <<colseed 
                                   <<" + " <<m_maxXFoI[sta] 
                                   <<" - " <<m_xfoi[sta] 
                                   <<" )" << std::endl;
            if (m_debug) std::cout <<"--- Tower::processTower:   shifted >> field is "<<field<< std::endl;
            field.resize(2*m_xfoi[sta]+1);
            if (m_debug) std::cout <<"--- Tower::processTower:   new size is "<< 2*m_xfoi[sta]+1 << std::endl;
            if (m_debug) std::cout <<"--- Tower::processTower:   resized field is "<<field<< std::endl;
            if (field.none()==false) break; // a hit has been found   
            candFlag=false;
          } // End of Loop over rows
          if ( candFlag==false) break;
        } // End of over bits in M4 and M5
        if (candFlag==false) continue; 
        // Conditions are fulfilled in M4 and M5; go on with search in M2 and M1
        if (m_debug) std::cout <<"--- Tower::processTower: M4 and M5 OK"<< std::endl;

        // Station M2
        LHCb::MuonTileID padM2;
        int offM2=0;
        // If M2 is ignored
        if (m_ignoreM2){
          int ipendulum=0;
          offM2 = ipendulum;
          std::pair<int,int> yxM2(rowseed,colseed);
          padM2 = getPadIdMap(2, yxM2); // Take the M3 pad (seed)
          padM2.setStation(1);
          candFlag=true;
        }// End If M2 is ignored
        // If M2 is used
        else{ 
          candFlag=false;
          sta=1;
          if (m_debug) std::cout <<"--- Tower::processTower: Search in station sta= "<<sta<< std::endl;
          if (m_debug) std::cout <<"--- Tower::processTower: Max foi X,Y= "<<m_maxXFoI[sta]<<" , "<<m_maxYFoI[sta]<< std::endl;
          if (m_debug) std::cout <<"--- Tower::processTower:     foi X,Y= "<<m_xfoi[sta]<<" , "<<m_yfoi[sta]<< std::endl;
          rowCentral = m_bittable[sta].begin()+rowseed+m_maxYFoI[sta];
          if (m_debug) std::cout <<"--- Tower::processTower:   full bitset size is "<<(*rowCentral).size()<< std::endl;
          if (m_debug) std::cout <<"--- Tower::processTower:   full bitset is "<<(*rowCentral)<< std::endl;
          field = (*rowCentral)>>(colseed+m_maxXFoI[sta]-m_xfoi[sta]);
          if (m_debug) std::cout <<"--- Tower::processTower:   shift is "<< colseed+m_maxXFoI[sta]-m_xfoi[sta]
                                 << " ( " <<colseed 
                                 <<" + "  <<m_maxXFoI[sta] 
                                 <<" - "  <<m_xfoi[sta] 
                                 <<" )"   << std::endl;
          if (m_debug) std::cout <<"--- Tower::processTower:   shifted >> field is "<<field<< std::endl;
          field.resize(2*m_xfoi[sta]+1);
          if (m_debug) std::cout <<"--- Tower::processTower:   new size is "<< 2*m_xfoi[sta]+1 << std::endl;
          if (m_debug) std::cout <<"--- Tower::processTower:   resized field is "<<field<< std::endl;
          // If a bit is set in the field
          if (field.none()==false) {
            colCentral = m_xfoi[sta];
            // Loop over colums in the field 
            for (int icol=0; icol<2*m_xfoi[sta]+1;icol++){
              if (m_debug) std::cout <<"--- Tower::processTower:     inside loop over columns icol= "<<icol<< std::endl;
              //             int ipendulum = (icol==0) ? 0 : int(pow(-1,icol+1)*int((icol+1)/2));
              int ipendulum = L0Muon::pendulumM2(icol, m_procVersion);
              if (m_debug) std::cout <<"--- Tower::processTower:     inside loop over columns ipendulum= "<<ipendulum<< std::endl;
              colInd=colCentral+ipendulum;
              if (m_debug) std::cout <<"--- Tower::processTower:     inside loop over columns bit is #= "
                                     <<m_xfoi[sta]+ipendulum<< std::endl;
              if (m_debug) std::cout <<"--- Tower::processTower:     inside loop over columns test= "
                                     <<field.test(colInd)<< std::endl;
              if (field.test(colInd)==true) {
                offM2 = ipendulum;
                std::pair<int,int> yxM2(rowseed,colseed+m_maxXFoI[sta]+ipendulum);
                padM2 = getPadIdMap(sta, yxM2);
                candFlag=true;
                if (m_debug) std::cout <<"--- Tower::processTower:     Bit ON found yxM2 is "
                                       <<yxM2.first<<","<<yxM2.second<< std::endl;
                if (m_debug) std::cout <<"--- Tower::processTower:     Bit ON found mid is "
                                       <<padM2.toString()<< std::endl;
                break;
              }
            } // End of Loop over colums in the field 
          } // End If a bit is set in the field
          if (candFlag==false) continue; 
        }// End if M2 is used
        
        // Conditions are fulfilled in M2; go on with search in M1
        if (m_debug) std::cout <<"--- Tower::processTower: M2 OK (offM2="<< offM2 << ")"<< std::endl;

        // Station M1
        LHCb::MuonTileID padM1;
        int offM1=0;  // expressed with M3 granularity (la tour est 'super-homogène') 
        candFlag=false;
        sta=0;
        if (m_debug) {
          std::cout <<"--- Tower::processTower: Search in station sta= "<<sta<< std::endl;
          std::cout <<"--- Tower::processTower: Max foi X,Y= "<<m_maxXFoI[sta]<<" , "<<m_maxYFoI[sta]<< std::endl;
          std::cout <<"--- Tower::processTower:     foi X,Y= "<<m_xfoi[sta]<<" , "<<m_yfoi[sta]<< std::endl;
        }
        
        // Extrapolation in M1
        int signM2 =offM2>0 ? +1:-1;
        //         int extrapM1=signM2*L0Muon::ExtrapolationM1[signM2*offM2];
        int extrapM1=signM2*L0Muon::extrapolationM1(signM2*offM2,m_procVersion);
        if (m_debug) std::cout <<"--- Tower::processTower:   extrapM1= "<<extrapM1<< std::endl;
        // If M1 is ignored
        if (m_ignoreM1==true){  
          if (m_debug) std::cout <<"--- Tower::processTower:   M1 is ignored"<< std::endl;
          std::pair<int,int> yxM1(rowseed,colseed+m_maxXFoI[sta]+extrapM1);
          padM1 = getPadIdMap(sta, yxM1); // xyM1 not filled in idmap inside event. Initialize in CoreUnit
          if (m_debug) std::cout <<"--- Tower::processTower:     forced Bit is NOT valid yxM1 is "
                                 <<yxM1.first<<","<<yxM1.second<< std::endl;
          if (!padM1.isValid()) { // If at the edge of a quarter, the extrapolated point might not be valid
            for (int icol=0; icol<2*m_xfoi[sta]+1;icol++){
              int ipendulum = L0Muon::pendulumM1(icol, m_procVersion);
              std::pair<int,int> yxM1_notcentral(rowseed,colseed+m_maxXFoI[sta]+extrapM1+ipendulum);
              if (m_debug) std::cout <<"--- Tower::processTower:       forced Bit for ipendulum = "<< ipendulum 
                                     <<" yxM1 is "
                                     <<yxM1_notcentral.first<<","<<yxM1_notcentral.second<< std::endl;
              padM1 = getPadIdMap(sta, yxM1_notcentral);
              if (padM1.isValid()) {
                if (m_debug) std::cout <<"--- Tower::processTower:     forced Bit for ipendulum IS VALID "<< std::endl;
                offM1 = ipendulum;
                break;
              }
            }
          }
          if (padM1.isValid()) {
            if (m_debug) {
              std::cout <<"--- Tower::processTower:     forced Bit ON found yxM1 is "
                        <<yxM1.first<<","<<yxM1.second<< std::endl;
              std::cout <<"--- Tower::processTower:     forced Bit ON found mid is "
                        <<padM1.toString()<< std::endl;
            }
            
            candFlag=true;
          } else {
            if (m_debug) std::cout <<"--- Tower::processTower:     forced Bit ON no valid pad found"<< std::endl;
          }
        } // End If M1 is ignored
        // If M1 is used
        else if (m_ignoreM1==false){           
          if (m_debug) std::cout <<"--- Tower::processTower:   M1 is used"<< std::endl;
          // Loop over bits in M1
          candFlag=false;
          rowCentral = m_bittable[sta].begin()+rowseed+m_maxYFoI[sta];
          if (m_debug) std::cout <<"--- Tower::processTower:   full bitset size is "<<(*rowCentral).size()<< std::endl;
          if (m_debug) std::cout <<"--- Tower::processTower:   full bitset is "<<(*rowCentral)<< std::endl;
          field = (*rowCentral)>>(colseed+m_maxXFoI[sta]+extrapM1-m_xfoi[sta]);
          if (m_debug) std::cout <<"--- Tower::processTower:   shift is "<< colseed+m_maxXFoI[sta]+extrapM1-m_xfoi[sta]
                                 << " ( " <<colseed 
                                 <<" + "  <<m_maxXFoI[sta] 
                                 <<" - "  <<extrapM1
                                 <<" - "  <<m_xfoi[sta] 
                                 <<" )"   << std::endl;
          if (m_debug) std::cout <<"--- Tower::processTower:   shifted >> field is "<<field<< std::endl;
          field.resize(2*m_xfoi[sta]+1);
          if (m_debug) std::cout <<"--- Tower::processTower:   new size is "<< 2*m_xfoi[sta]+1 << std::endl;
          if (m_debug) std::cout <<"--- Tower::processTower:   resized field is "<<field<< std::endl;
          // If a bit is set in the field
          if (field.none()==false) {
            colCentral = m_xfoi[sta];
            // Loop over colums in the field 
            for (int icol=0; icol<2*m_xfoi[sta]+1;icol++){
              //               int ipendulum = (icol==0) ? 0 : int(pow(-1,icol+1)*int((icol+1)/2));
              //               int ipendulum = (icol==0) ? 0 : int(pow(-1,icol)*int((icol+1)/2)); // start searching towards beam
              int ipendulum = L0Muon::pendulumM1(icol, m_procVersion);
              colInd=colCentral+ipendulum;
              if (m_debug) {
                std::cout <<"--- Tower::processTower:     inside loop over columns icol= "<<icol<< std::endl;
                std::cout <<"--- Tower::processTower:     inside loop over columns ipendulum= "
                          <<ipendulum<< std::endl;
                std::cout <<"--- Tower::processTower:     inside loop over columns bit is #= "
                          <<m_xfoi[sta]+ipendulum<< std::endl;
                std::cout <<"--- Tower::processTower:     inside loop over columns test= "
                          <<field.test(colInd)<< std::endl;
              }
              
              if (field.test(colInd)==true) {
                offM1 = ipendulum;
                std::pair<int,int> yxM1(rowseed,colseed+m_maxXFoI[sta]+extrapM1+ipendulum);
                padM1 = getPadIdMap(sta, yxM1);
                candFlag=true;
                if (m_debug) {
                  std::cout <<"--- Tower::processTower:     Bit ON found yxM1 is "
                            <<yxM1.first<<","<<yxM1.second<< std::endl;
                  std::cout <<"--- Tower::processTower:     Bit ON found mid is "
                            <<padM1.toString()<< std::endl;
                }
                
                break;
              }
            } // End of Loop over colums in the field 
          } // End If a bit is set in the field
        } // End If M1 is used
        // Conditions are fulfilled in M1; a candidate has been found
        if (candFlag==false) continue; 
        if (m_debug) std::cout <<"--- Tower::processTower: M1 OK"<< std::endl;
        
        // In version 0, the offset in M1 is given  as it is, i.e. in the granularity of M3.
        // In next version, it is given in the granularity of M1.
        offM1 = L0Muon::offsetM1(offM1,m_procVersion);
        
        if (m_debug) {
          std::cout <<"--- Tower::processTower: CANDIDATE FOUND"<< std::endl;
          std::cout <<"--- Tower::processTower:  colM3= "<< colseed <<" rowM3= "<<rowseed << std::endl;
          std::cout <<"--- Tower::processTower:  offM2= "<< offM2 << std::endl;
          std::cout <<"--- Tower::processTower:  offM1= "<< offM1 <<std::endl;
          
          std::cout <<"--- Tower::processTower: padM1= "<<padM1.toString()<< std::endl;
          std::cout <<"--- Tower::processTower: padM2= "<<padM2.toString()<< std::endl;
          
          std::cout <<"--- Tower::processTower: plut= "<<m_lut<< std::endl;
        }
        
        // Compute PT

        int ipt = -1;
        if (m_lut==0) {
          double pt = L0Muon::kine(padM1,padM2,m_procVersion,m_debug)[0];
          ipt=L0Muon::encodePT(pt,m_procVersion,m_debug);
          if (m_debug) std::cout <<"--- Tower::processTower: (from kine) pt= "<<pt<<" encoded= "<<ipt<<std::endl;
        } else {

          std::pair<int,int> yxM3(rowseed,colseed);
          LHCb::MuonTileID padM3  = getPadIdMap(2, yxM3);
          LHCb::MuonTileID m3pad  = padM3;

          int M2nx = colseed+offM2;
          int M2ny = rowseed;
          LHCb::MuonTileID m2pad(puID,MuonLayout(48,8),M2nx,M2ny);
          m2pad.setStation(1);
          if (!m2pad.isValid()){
            if ( m2pad.nX()>=2*m2pad.layout().xGrid() ) {
              std::vector<LHCb::MuonTileID> lpads =  m2pad.layout().tilesInRegion(m2pad,m2pad.region());
              //if (lpads.size()!=1) {
              //  std::cout <<"--- Tower::processTower: ERROR M2 PAD DOES NOT COVER EXACTLY ONE PAD IN UPPER REGION"<<std::endl;
              //}
              m2pad = lpads[0];
            } //else {
            //  std::cout <<"--- Tower::processTower: ERROR m2pad NOT Valid"<<std::endl;
            //}
          } 
          int M1nx = colseed+extrapM1;
          int M1ny = rowseed;  
          M1nx = L0Muon::addM1Offset(M1nx, offM1, m_procVersion);
          LHCb::MuonTileID m1pad(puID,MuonLayout(24,8),M1nx,M1ny);
          m1pad.setStation(0);
          if (!m1pad.isValid()){
            if ( m1pad.nX()>=2*m1pad.layout().xGrid() ) {
              std::vector<LHCb::MuonTileID> lpads =  m1pad.layout().tilesInRegion(m1pad,m1pad.region());
              //if (lpads.size()!=1) {
              //  std::cout <<"--- Tower::processTower: ERROR M1 PAD DOES NOT COVER EXACTLY ONE PAD IN UPPER REGION"<<std::endl;
              //}
              m1pad = lpads[0];
            } //else {
            //  std::cout <<"--- Tower::processTower: ERROR m1pad NOT Valid"<<std::endl;
            //}
          }
          ipt = m_lut->getEncodedPt(m2pad,m1pad);
          if (m_debug) std::cout <<"--- Tower::processTower: (from LUT) encoded= "<<ipt<<std::endl;
        }

        // Create MuonCandidate (without the pu and board info)
        L0Muon::PMuonCandidate muoncand( new L0Muon::MuonCandidate());
        muoncand->setColM3(colseed);
        muoncand->setRowM3(rowseed);
        muoncand->setOffM2(offM2);
        muoncand->setOffM1(offM1);
        muoncand->setPT(ipt&0x7F);
        muoncand->setCharge((ipt>>7)&1);
        puCandidates.push_back(muoncand);
        if (m_debug) std::cout <<"--- Tower::processTower: candidate pushed in vector"<< std::endl;

      } // End If the M3 bit is set
    } // End of Loop over columns
    rowseed++;
  } // End of Loop over rows  
  if (m_debug) std::cout <<"--- Tower::processTower: Out "<< std::endl;
  
  // Order the vector of candidates
  L0Muon::candidatesOrdering( & puCandidates, m_procVersion);
  // Return the vector of candidates
  return puCandidates;
  
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
