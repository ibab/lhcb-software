#include "L0MuonKernel/TileRegister.h"

L0Muon::TileRegister::TileRegister() {
  //m_type = L0Muon::STRIP;
  m_type = STRIP;
}


  

L0Muon::TileRegister::TileRegister(int bits) : L0Muon::Register(bits) {
  //m_type = L0Muon::STRIP;
  m_type = STRIP;
}

L0Muon::TileRegister::TileRegister(int bits, unsigned long pattern) : 
  L0Muon::Register(bits,pattern) {
  //m_type = L0Muon::STRIP;
  m_type = STRIP;
}
    
L0Muon::TileRegister::TileRegister(int bits, 
                                   unsigned long pattern,
				   std::vector<MuonTileID> ids) : 
    L0Muon::Register(bits,pattern), m_ids(ids) {
  // m_type = L0Muon::STRIP; 
  m_type = STRIP; 

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

void L0Muon::TileRegister::setType(int sta, int reg){
  MuonSystemLayout slulayout =
    MuonSystemLayout(MuonStationLayout(MuonLayout(0,0)),
                     MuonStationLayout(MuonLayout(8,1),
                                       MuonLayout(4,2),
                                       MuonLayout(2,2),
                                       MuonLayout(2,2)),
                     MuonStationLayout(MuonLayout(8,1),
                                       MuonLayout(4,2),
                                       MuonLayout(2,2),
                                       MuonLayout(2,2)),
                     MuonStationLayout(MuonLayout(0,0),
                                       MuonLayout(4,2),
                                       MuonLayout(2,2),
                                       MuonLayout(2,2)),
                     MuonStationLayout(MuonLayout(0,0),
                                       MuonLayout(4,2),
                                       MuonLayout(2,2),
                                       MuonLayout(2,2)));
  


  
  if( slulayout.regionLayout(sta,reg).isDefined() ) {
    // m_type= L0Muon::STRIP ;
    m_type= STRIP ;
  } else {
    //m_type =L0Muon::PAD ;
    m_type =PAD ;
  }
  
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


