#include "ProcessorKernel/TileRegister.h"

L0Muon::TileRegister::TileRegister() {

}

L0Muon::TileRegister::TileRegister(int bits) : L0Muon::Register(bits) {

}

L0Muon::TileRegister::TileRegister(int bits, unsigned long pattern) : 
  L0Muon::Register(bits,pattern) {

}
    
L0Muon::TileRegister::TileRegister(int bits, 
                                   unsigned long pattern,
				   std::vector<MuonTileID> ids) : 
    L0Muon::Register(bits,pattern), m_ids(ids) {


}    
    
L0Muon::TileRegister::~TileRegister() {}    


std::vector<MuonTileID> L0Muon::TileRegister::firedTiles() {
  
  std::vector<MuonTileID> result;
  unsigned int ind;
  for ( ind = 0; ind<m_bitset.size(); ind++) {
    if (m_bitset.test(ind)) {
      result.push_back(tile(ind));    
    }  
  }
  return result;
}

void L0Muon::TileRegister::setTileVector(const std::vector<MuonTileID>& ids) {
  m_ids = ids;
}


void L0Muon::TileRegister::setTile(MuonTileID & id){
  unsigned int index =0;  
  for (unsigned int ind =0; ind < m_ids.size(); ind++){
    if ( m_ids[ind]== id){
      index =ind;
      m_bitset.set(index);
      break;
    }    
  }  
}

void L0Muon::TileRegister::setTag(std::vector<L0MTile> & tiles){

  std::vector<L0MTile>::iterator it;
  
  if (m_tilestag.size() != 0){
    m_tilestag.clear();
  }

  if (m_stripstag.size() != 0){
    m_stripstag.clear();
  }
  
  for (it = tiles.begin(); it != tiles.end(); it++) {
    if ( (*it).tag()==L0MBase::StripH ||
         (*it).tag()==L0MBase::StripV){
      m_tilestag.push_back(0);
      if ((*it).tag()==L0MBase::StripH) m_stripstag.push_back(0);
      if ((*it).tag()==L0MBase::StripV) m_stripstag.push_back(1);

    } else if ((*it).tag()==L0MBase::Pad || (*it).tag()==L0MBase::YCorner ||
               (*it).tag()==L0MBase::YBand) {
      m_tilestag.push_back(1);
      m_stripstag.push_back(0);

    } else if ( (*it).tag()==L0MBase::XBand ){
      std::vector<L0MTile> tmp = (*it).tiles();
      std::vector<L0MTile>::iterator itmp;
      itmp = tmp.begin();
      if ( (*itmp).tag()==L0MBase::Pad) {
        m_tilestag.push_back(1);
        m_stripstag.push_back(0);
      } else if ( (*itmp).tag()==L0MBase::StripH){
        m_tilestag.push_back(0);
        m_stripstag.push_back(0);
      } else if ( (*itmp).tag()==L0MBase::StripV){
        m_tilestag.push_back(0);
        m_stripstag.push_back(1);
      }
    }
  }
}



void L0Muon::TileRegister::makePads(){
 
  if (m_pads.size() != 0){
    m_pads.clear();
  }  
  std::vector<MuonTileID> tmp = firedTiles();
  //std::vector<MuonTileID> tilev = getTileVector();
  
  unsigned int i,j;
  if ( ! tmp.empty() ) {
    for ( i=0; i< m_bitset.size(); i++){

      if (m_bitset.test(i)) {
        if ( m_tilestag[i] == 1){
          MuonTileID mid = tile(i);
          m_pads.push_back(mid);
        }
        if ( m_tilestag[i] ==0){
          if ( i< m_bitset.size()-1) {
            if (m_bitset.test(i)) {
              for ( j = i+1; j < m_bitset.size(); j++){
                if (m_bitset.test(j)) {
                  if ( m_stripstag[i] != m_stripstag[j]) {
                    MuonTileID ip = tile(i);
                    MuonTileID jp = tile(j);
                    MuonTileID ijp = ip.intercept(jp);
                    if (ijp.isValid()){
                      m_pads.push_back(ijp);
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}





  



      
