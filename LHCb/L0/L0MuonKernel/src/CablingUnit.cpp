//#include <time.h>
//#include <strstream>
#include "L0MuonKernel/CablingUnit.h"
#include "L0MuonKernel/CrateUnit.h"
#include "MuonTools/IMuonTileXYZTool.h"


L0Muon::CablingUnit::CablingUnit() {  
    
      
}
    
L0Muon::CablingUnit::~CablingUnit() {};  

void L0Muon::CablingUnit::makePads(MsgStream * log) {
  m_pads.clear();
  //dumpRegisters(log);
  std::map<std::string,Register*>::iterator ir;

  for ( ir = m_inputs.begin(); ir != m_inputs.end(); ir++ ) {
    TileRegister* itr = dynamic_cast<TileRegister*>(ir->second);
    *log << MSG::DEBUG << " " << ir->first << " " << itr->type() <<endreq;
    boost::dynamic_bitset<> r = itr->getBitset();
    *log << MSG::DEBUG << " reg size" << " " <<r.size() << endreq;
    std::vector<MuonTileID> tmp = itr->firedTiles();
    std::vector<MuonTileID>::iterator itmp;
    for (itmp = tmp.begin(); itmp!= tmp.end(); itmp++){
      *log << MSG::DEBUG << " " << *itmp << endreq;
    }
    
    //if ( itr->type() == L0Muon::STRIP ){
    if ( itr->type() == 0 ){
      if ( ! tmp.empty() ) {
        int tmpsize = tmp.size();
        *log << MSG::DEBUG << "tmpsize" << " " << tmpsize << endreq;
        int i,j;
 
        for (itmp = tmp.begin(); itmp!= tmp.end(); itmp++){
          *log << MSG::DEBUG << " " << *itmp << endreq;
        }  
        for ( i = 0; i<(tmpsize-1); i++ ) {
          for ( j=i+1; j<tmpsize; j++) {
            MuonTileID ip = tmp[i];
            *log << MSG::DEBUG << "make pads: Strip" << " " << tmp[i] << endreq;
            MuonTileID jp = tmp[j];
            *log << MSG::DEBUG << "make pads:Strip" << " " << tmp[j] << endreq;

            MuonTileID ijp = ip.intercept(jp);
            if ( ijp.isValid() ) {
              m_pads.push_back(ijp);
              *log << MSG::DEBUG << "insert in pad " << ijp << endreq;
              
            }
            
            
          }
          
          
        }
        
        
      }
      
    } else {
      
      if ( ! tmp.empty() ) {
        
        for (std::vector<MuonTileID>::iterator iv = tmp.begin(); 
             iv != tmp.end();iv++){
          
          m_pads.push_back(*iv);
          *log << MSG::DEBUG << "insert in pad the pad " << *iv << endreq;
    
        }
      }
    }
  }
  
}

void L0Muon::CablingUnit::makeTower(MsgStream * log) {
  unsigned int nreg = m_pu.region();
  MuonLayout layout(48,8);
  MuonLayout pulayout(2,2);

  MuonTileID refpad = MuonTileID(m_pu,layout,0,0);
  int refX = refpad.nX();
  int refY = refpad.nY();

  std::vector<MuonTileID>::iterator ip ;
  std::vector<MuonTileID>::iterator itmp;
  for (ip=m_pads.begin(); ip != m_pads.end(); ip++) {
    
    *log << MSG::DEBUG << "pads" << " " << *ip << endreq;
    int nsta = ip->station();
    std::vector<MuonTileID> tmp;
    if ( ip->region() == nreg ) {
      tmp = layout.tiles(*ip);
    } else {
      tmp = layout.tilesInRegion(*ip,nreg); 
    }
    
     for ( itmp = tmp.begin(); itmp != tmp.end(); itmp++ ) {


       MuonTileID puid = pulayout.contains(*itmp);
       std::pair<int, int>  yx =
         std::make_pair(itmp->nY()-refY+m_tower.maxYFoi(nsta),
                        itmp->nX()-refX+m_tower.maxXFoi(nsta));
         
       m_tower.setBit(log, nsta,
                      itmp->nY()-refY+m_tower.maxYFoi(nsta),
                      itmp->nX()-refX+m_tower.maxXFoi(nsta));
       *log << MSG::DEBUG << "XY" << itmp->nX()-refX+m_tower.maxXFoi(nsta) <<
         " " <<  itmp->nY()-refY+m_tower.maxYFoi(nsta) << " " << "sta" << 
         " " << nsta << " " << *itmp << endreq;       
     
       m_tower.setPadIdMap(nsta, yx, *ip);
     
     }
    
  }
  
  
  
    
  
  
}


void L0Muon::CablingUnit::initialize() {
}


void L0Muon::CablingUnit::execute() {

  Unit * myBoard = m_parent->parent();
  //BoardUnit * bd = dynamic_cast<BoardUnit*>(myBoard);
 
  Unit * myCrate = myBoard->parent();
  CrateUnit * cr = dynamic_cast<CrateUnit*>(myCrate);
  MsgStream * log = cr->getMsgStream();
  
  makePads(log);
  makeTower(log);

  
  L0mProcUnit * mpu = dynamic_cast<L0mProcUnit*>(m_parent);
  
  
   for (int ista=0; ista<5; ista++){
     
     m_tower.setXfoi(ista,mpu->xFoi(ista));
     m_tower.setYfoi(ista,mpu->yFoi(ista));
   }
   
   m_tower.setPtparam(mpu->ptParameters());


  IMuonTileXYZTool *iTileXYZTool = cr->getMuonTool();
  m_tower.setMuonToolInTower(iTileXYZTool);
  m_tower.processTower(m_pu, log);
  
  //if (m_tower.numberOfCand() > 0){
  drawTower(log);
    //}
  m_cand = m_tower.puCandidates();

  if (m_cand.size()>0 && m_cand.size()<2){
    m_status = L0Muon::OK;
  } else if (m_cand.size()>2){
    m_status = L0Muon::PU_OVERFLOW;
  }
  *log << MSG::DEBUG << "Number of Candidates" <<" " 
       << m_cand.size() << endreq;
  if (m_cand.size()>0){
    for (std::vector<L0MuonCandidate*>::iterator icand= m_cand.begin();
         icand != m_cand.end(); icand++){
      cr->fillCandidates(*icand);
      
    }
    
    cr->setStatus(L0Muon::OK);
  }
  
  
  if (m_cand.size()==0){
    m_status =0;
    cr->setStatus(1);
  }
  


  m_tower.reset();
  
  
}







void L0Muon::CablingUnit::finalize() {
  m_pads.clear();
  releaseRegisters();
}




