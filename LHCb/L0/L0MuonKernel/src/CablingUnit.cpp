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
    *log << MSG::DEBUG << " " << ir->first << " " <<endreq;
    boost::dynamic_bitset<> r = itr->getBitset();
    *log << MSG::DEBUG << " reg size" << " " <<r.size() << endreq;
    std::vector<MuonTileID> tmp = itr->firedTiles();
    std::vector<MuonTileID>::iterator itmp;
    *log << MSG::DEBUG << "Fired tiles " << endreq;
    for (itmp = tmp.begin(); itmp!= tmp.end(); itmp++){
      *log << MSG::DEBUG << " " << (*itmp).quarter() << " " 
           << (*itmp).region() << " " << (*itmp).nX() << " " 
           << (*itmp).nY()
           << endreq;
    }
    
 
    //============================================
    itr->makePads();

    std::vector<MuonTileID> pads = itr->Pads();
    std::vector<MuonTileID>::iterator  ipads ;    
    for (ipads = pads.begin(); ipads != pads.end(); ipads++){
      m_pads.push_back(*ipads);
      
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
    
    *log << MSG::DEBUG << "pad" << " " << (*ip).quarter() <<" " 
       << (*ip).region() << " " 
       << (*ip).nX() << " " 
       << (*ip).nY() << endreq;
    int nsta = ip->station();
    std::vector<MuonTileID> tmp;
    
    
    if ( ip->region() == nreg ) {
      *log << MSG::DEBUG << "ip->region==nreg" << endreq;
      tmp = layout.tiles(*ip);
    } else if ( ip->region() != nreg){
      *log << MSG::DEBUG << "ip->region != nreg" << endreq;
      *log << MSG::DEBUG << "quarter " << (*ip).quarter() << " region " 
           << (*ip).region() << " nreg " << nreg << endreq;
      tmp = layout.tilesInRegion((*ip), nreg); 
    }
    
    *log << MSG::DEBUG << "tiles for this pad" << endreq;
     for ( itmp = tmp.begin(); itmp != tmp.end(); itmp++ ) {
       *log << MSG::DEBUG << "sta" << 
         " " << nsta << " " << (*itmp).quarter() << " " 
            << (*itmp).region() << " " 
            << (*itmp).nX() << " " << (*itmp).nY() << endreq;
     }
     
     
     for ( itmp = tmp.begin(); itmp != tmp.end(); itmp++ ) {


       MuonTileID puid = pulayout.contains(*itmp);

       int nYindex= (itmp->nY())-refY+m_tower.maxYFoi(nsta);
       int nXindex= (itmp->nX())-refX+m_tower.maxXFoi(nsta);

       std::pair<int, int>  yx = std::make_pair(nYindex,nXindex);
       m_tower.setBit(log, nsta, nYindex, nXindex);
       *log << MSG::DEBUG << "XY" << " " << nXindex <<
         " " <<  nYindex << " " << "sta" << 
         " " << nsta << " " << (*itmp).quarter() << " " 
            << (*itmp).region() << " " 
            << (*itmp).nX() << " " << (*itmp).nY() << endreq;
       m_tower.setPadIdMap(nsta, yx, *ip);
       
     }
     m_tower.drawStation(nsta,log);

    
  }
  
  
  
    
  
  
}


void L0Muon::CablingUnit::initialize() {
}


void L0Muon::CablingUnit::execute() {

  Unit * myBoard = m_parent->parent();
 
  Unit * myCrate = myBoard->parent();
  CrateUnit * cr = dynamic_cast<CrateUnit*>(myCrate);
  MsgStream * log = cr->getMsgStream();
  
  m_tower.reset();

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
  
 
  m_cand = m_tower.puCandidates();


  if (m_cand.size()>0 && m_cand.size()<3){
    m_status = L0Muon::OK;
  } else if (m_cand.size()>2){
    m_status = L0Muon::PU_OVERFLOW;
  }
  *log << MSG::DEBUG << "Number of Candidates" <<" " 
       << m_cand.size() << endreq;
  if (m_cand.size()>0){
    
    //std::sort(m_cand.begin(),m_cand.end(),ComparePt());
    
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

  // for offset in ntuple
  m_offForCand=m_tower.candOffset();
  for (std::vector<std::pair<L0MuonCandidate*, std::vector<int> > >::
         iterator ioff =
         m_offForCand.begin(); ioff != m_offForCand.end(); ioff++){
    *log << MSG::DEBUG  << (*ioff).first->pt() << endreq;
  }
  
  
  

  for (std::vector<std::pair<L0MuonCandidate*, std::vector<int> > >::
         iterator ioff =
         m_offForCand.begin(); ioff != m_offForCand.end(); ioff++){
    
    cr->fillOffset(*ioff);
  
    //*log << MSG::INFO << "\nOffsets of candidates" <<" " << (*ioff).second;
    
}
  
  
  
}








void L0Muon::CablingUnit::finalize() {
  m_pads.clear();
  releaseRegisters();
}




