#include "ProcessorKernel/TileRegister.h"
#include <cmath> // For definition of pow on windows

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
    } else {
      m_tilestag.push_back(0);
      m_stripstag.push_back(0);
      
    }
  }
}


/*
  Fill the vector m_pads
*/
void L0Muon::TileRegister::makePads(int flag){
 
  if (flag) {
    std::cout <<"<TileRegister::makePads> ";
    std::cout <<" IN ";
    std::cout <<"";
    std::cout <<std::endl;
  }

  if (flag) {
    std::cout <<"<TileRegister::makePads> ";
    std::cout <<" m_pads.size() ";
    std::cout <<m_pads.size();
    std::cout <<std::endl;
  }
  if (m_pads.size() != 0){
    m_pads.clear();
  }  
  std::vector<MuonTileID> tmp = firedTiles();
  if (flag) {
    std::cout <<"<TileRegister::makePads> ";
    std::cout <<" tmp.size() ";
    std::cout <<tmp.size();
    std::cout <<" tmp.empty() ";
    std::cout <<tmp.empty();
    std::cout <<std::endl;
  }
  //std::vector<MuonTileID> tilev = getTileVector();
  
  unsigned int i,j;
  if ( ! tmp.empty() ) {
    for ( i=0; i< m_bitset.size(); i++){
      MuonTileID mid = tile(i);
   if (flag) {
    std::cout <<"<TileRegister::makePads> ";
    std::cout <<" mid.station() ";
    std::cout <<  mid.station();
    std::cout <<" mid.region() ";
    std::cout <<  mid.region();
    std::cout <<" mid.isValid() ";
    std::cout <<  mid.isValid();
    std::cout <<std::endl;
  }
      if (mid.isValid()==0) continue;

  if (flag) {
    std::cout <<"<TileRegister::makePads> ";
    std::cout <<" i ";
    std::cout << i ;
    std::cout <<" m_bitset.test(i) ";
    std::cout << m_bitset.test(i) ;
    std::cout <<" m_tilestag.size() ";
    std::cout <<  m_tilestag.size();
    std::cout <<std::endl;
  }
      if (m_bitset.test(i)) {
  if (flag) {
    std::cout <<"<TileRegister::makePads> ";
    std::cout <<" m_tilestag[i] ";
    std::cout << m_tilestag[i] ;
    std::cout <<std::endl;
  }
        if ( m_tilestag[i] == 1){
  if (flag) {
    std::cout <<"<TileRegister::makePads> ";
    std::cout <<" pad already ! ";
    std::cout <<std::endl;
  }
          m_pads.push_back(mid);
        }
        if ( m_tilestag[i] ==0){
          if ( i< m_bitset.size()-1) {
            if (m_bitset.test(i)) {
              for ( j = i+1; j < m_bitset.size(); j++){
  if (flag) {
    std::cout <<"<TileRegister::makePads> ";
    std::cout <<" i ";
    std::cout <<  i;
    std::cout <<" j ";
    std::cout <<  j;
    std::cout <<std::endl;
  }
		
                if (m_bitset.test(j)) {
  if (flag) {
    std::cout <<"<TileRegister::makePads> ";
    std::cout <<" j ";
    std::cout << j ;
    std::cout <<" m_bitset.test(j) ";
    std::cout << m_bitset.test(j) ;
    std::cout <<" m_stripstag[i] ";
    std::cout <<  m_stripstag[i];
    std::cout <<" m_stripstag[j] ";
    std::cout <<  m_stripstag[j];
    std::cout <<std::endl;
  }

                  if ( m_stripstag[i] != m_stripstag[j]) {
                    MuonTileID ip = tile(i);
                    MuonTileID jp = tile(j);
  if (flag) {
    std::cout <<"<TileRegister::makePads> ";
    std::cout <<" ip.station() ";
    std::cout <<  ip.station();
    std::cout <<" ip.region() ";
    std::cout <<  ip.region();
    std::cout <<" ip.isValid() ";
    std::cout <<  ip.isValid();
    std::cout <<std::endl;
  }
  if (flag) {
    std::cout <<"<TileRegister::makePads> ";
    std::cout <<" jp.station() ";
    std::cout <<  jp.station();
    std::cout <<" jp.region() ";
    std::cout <<  jp.region();
    std::cout <<" jp.isValid() ";
    std::cout <<  jp.isValid();
    std::cout <<std::endl;
  }
                    MuonTileID ijp = ip.intercept(jp);
  if (flag) {
    std::cout <<"<TileRegister::makePads> ";
    std::cout <<" intercept:  ";
    std::cout <<" ijp.station() ";
    std::cout <<  ijp.station();
    std::cout <<" ijp.region() ";
    std::cout <<  ijp.region();
    std::cout <<" ijp.isValid() ";
    std::cout <<  ijp.isValid();
    std::cout <<std::endl;
  }
                    if (ijp.isValid()){
   if (flag) {
    std::cout <<"<TileRegister::makePads> ";
    std::cout <<" intercept valid:  ";
    std::cout <<std::endl;
  }
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

void L0Muon::TileRegister::print_bits(long unsigned int event, FILE *file){

  // Loop over the bits of the register 
  for (boost::dynamic_bitset<>::size_type i =0; i < m_bitset.size();i++){
    int val=m_bitset[i] ;
    MuonTileID  mid= tile(i);
    if (mid.isValid()){
      fprintf(file,"%5u %3d ( %2d,%2d ) Q%1d M%1d R%1d %2d %2d  %d\n"
	      ,event ,i 
	      ,mid.layout().xGrid()
	      ,mid.layout().yGrid()
	      ,mid.quarter()+1 
	      ,mid.station()+1 
	      ,mid.region()+1 
	      ,mid.nX()
	      ,mid.nY()
	      ,val
	      ); 
    } else {
      fprintf(file,"%5u %3d                           %d\n",event,i,val);
    }

  }
  fprintf(file,"-----------------------------------\n");
}


void L0Muon::TileRegister::print_words(FILE *file, int nwords_per_line){
  int ib=0;
  int word=0;
  int iw=0;
  for (boost::dynamic_bitset<>::size_type i =0; i < m_bitset.size();i++){
    
    int val=m_bitset[i] ;
    if (val>0) {
      word+= int(pow(2,ib));
    }
    
    ib++;
    if ((ib%16)==0) {
      ib=0;
      fprintf(file,"%04x ",word);
      //std::cout << "<L0BufferUnit::dump>  iword" << iword << std::endl; 
      word=0;
      iw++;
      if ((iw%nwords_per_line)==0) {
      fprintf(file,"\n");
      }
    }
  }
  if(ib!=0) {
    fprintf(file,"!!!\n");
  }

}
  

void L0Muon::TileRegister::print_tiles(FILE *file,int ntiles_per_line){
  int ic=0;
  for (std::vector<MuonTileID> ::iterator im = m_ids.begin();im!=m_ids.end();im++) {
    if ((ic % ntiles_per_line)==0)  fprintf(file,"\n\t");
    fprintf(file," M%d(%2d,%2d)Q%dR%d %2d-%2d ;",im->station()+1,im->layout().xGrid(),im->layout().yGrid(),im->quarter()+1,im->region()+1,im->nX(),im->nY());
    ic++;
  }
  fprintf(file,"\n");
}


      
