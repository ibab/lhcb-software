//#include <time.h>
//#include <strstream>
#include "L0MuonKernel/CablingUnit.h"
#include "L0MuonKernel/CrateUnit.h"
#include "L0MuonKernel/BuildL0BufferUnit.h"
#include "L0MuonKernel/BestCandidateSelectionUnit.h"
#include "MuonTools/IMuonTileXYZTool.h"


L0Muon::CablingUnit::CablingUnit() {  
    
 
  m_status = 0;
 
 
}
    
L0Muon::CablingUnit::~CablingUnit() {};  

void L0Muon::CablingUnit::makePads(MsgStream & log) {
  m_pads.clear();
 
  std::map<std::string,Register*>::iterator ir;

  
  for ( ir = m_inputs.begin(); ir != m_inputs.end(); ir++ ) {
    TileRegister* itr = dynamic_cast<TileRegister*>(ir->second);
    log << MSG::DEBUG << " " << ir->first << " " <<endreq;
    boost::dynamic_bitset<> r = itr->getBitset();
    log << MSG::DEBUG << " reg size" << " " <<r.size() << endreq;
    std::vector<MuonTileID> tmp = itr->firedTiles();
    std::vector<MuonTileID>::iterator itmp;
    log << MSG::DEBUG << "Fired tiles " << endreq;
    for (itmp = tmp.begin(); itmp!= tmp.end(); itmp++){
      log << MSG::DEBUG << " " << (*itmp).quarter() << " " 
          << (*itmp).region() << " " << (*itmp).nX() << " " 
          << (*itmp).nY()
          << endreq;
    }
    
 
    itr->makePads();

    std::vector<MuonTileID> pads = itr->Pads();
    std::vector<MuonTileID>::iterator  ipads ;    
    for (ipads = pads.begin(); ipads != pads.end(); ipads++){
      m_pads.push_back(*ipads);
      
    }
    

       

    
    
  }
  
  
  
}






void L0Muon::CablingUnit::makeTower(MsgStream & log) {

  unsigned int nreg = m_pu.region();
  MuonLayout layout(48,8);
  MuonLayout pulayout(2,2);

  MuonTileID refpad = MuonTileID(m_pu,layout,0,0);
  int refX = refpad.nX();
  int refY = refpad.nY();

  std::vector<MuonTileID>::iterator ip ;
  std::vector<MuonTileID>::iterator itmp;

  
  for (ip=m_pads.begin(); ip != m_pads.end(); ip++) {

    log << MSG::DEBUG << "pad" << " " << (*ip).quarter() <<" " 
        << (*ip).region() << " " 
        << (*ip).nX() << " " 
        << (*ip).nY() << endreq;
    int nsta = ip->station();
    std::vector<MuonTileID> tmp;
    
    
    if ( ip->region() == nreg ) {
      log << MSG::DEBUG << "ip->region==nreg" << endreq;
      tmp = layout.tiles(*ip);
    } else if ( ip->region() != nreg){
      log << MSG::DEBUG << "ip->region != nreg" << endreq;
      log << MSG::DEBUG << "quarter " << (*ip).quarter() << " region " 
          << (*ip).region() << " nreg " << nreg << endreq;
      tmp = layout.tilesInRegion((*ip), nreg); 
    }
    
    log << MSG::DEBUG << "tiles for this pad" << endreq;
    for ( itmp = tmp.begin(); itmp != tmp.end(); itmp++ ) {
      log << MSG::DEBUG << "sta" << 
        " " << nsta << " " << (*itmp).quarter() << " " 
          << (*itmp).region() << " " 
          << (*itmp).nX() << " " << (*itmp).nY() << endreq;
    }
     
     
     for ( itmp = tmp.begin(); itmp != tmp.end(); itmp++ ) {


       MuonTileID puid = pulayout.contains(*itmp);

       int nYindex= (itmp->nY())-refY+m_tower.maxYFoi(nsta);
       int nXindex= (itmp->nX())-refX+m_tower.maxXFoi(nsta);

       std::pair<int, int>  yx = std::make_pair(nYindex,nXindex);

     
       m_tower.setBit(nsta, nYindex, nXindex, log);

       //log << MSG::DEBUG << "XY" << " " << nXindex <<
       //" " <<  nYindex << " " << "sta" << 
       //" " << nsta << " " << (*itmp).quarter() << " " 
       ///  << (*itmp).region() << " " 
       //<< (*itmp).nX() << " " << (*itmp).nY() << endreq;
       m_tower.setPadIdMap(nsta, yx, *ip);
      
       
     }
     
     
     
  }
  
  
  
  
    
  
  
}


void L0Muon::CablingUnit::initialize() {
  L0Muon::Unit::initialize();
}

void L0Muon::CablingUnit::initialize(MsgStream & log) {

  if ( ! m_units.empty() ) {
    
    std::map<std::string,L0Muon::Unit*>::iterator iu;
    
    for ( iu = m_units.begin(); iu != m_units.end(); iu++ ) {
      (*iu).second->initialize(log);
      
    }
    
  }
  else {
          
    return ;
    
    
  }
  
}


void L0Muon::CablingUnit::execute() {
  L0Muon::Unit::execute();
}

void L0Muon::CablingUnit::execute(MsgStream & log) {

  Unit * myBoard = m_parent->parent();
  BoardUnit * bu = dynamic_cast<BoardUnit*>(myBoard);
  
  BestCandidateSelectionUnit* bcsu= dynamic_cast<BestCandidateSelectionUnit*>
    (bu->subUnit("bcsu"));
  
 
  Unit * myCrate = myBoard->parent();
  CrateUnit * cr = dynamic_cast<CrateUnit*>(myCrate);
  
  m_tower.reset();

  makePads(log);
  makeTower(log);

  
  L0mProcUnit * mpu = dynamic_cast<L0mProcUnit*>(m_parent);
  
  
   for (int ista=0; ista<5; ista++){
     
     m_tower.setXfoi(ista,mpu->xFoi(ista));
     m_tower.setYfoi(ista,mpu->yFoi(ista));
   }
   
   m_tower.setPtparam(mpu->ptParameters());
   m_tower.setIgnoreM1(mpu->ignoreM1());

  IMuonTileXYZTool *iTileXYZTool = cr->getMuonTool();
  
  m_tower.setMuonToolInTower(iTileXYZTool);
  m_tower.processTower(m_pu, log);
  m_cand = m_tower.puCandidates();

  if (m_cand.size()>0 && m_cand.size()<3){
    m_status = L0Muon::OK;
  } else if (m_cand.size()>2){
    m_status = L0Muon::PU_OVERFLOW;
  }
  log << MSG::DEBUG << "Number of Candidates" <<" " 
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

  // for offset in ntuple
 
  m_offForCand=m_tower.candOffset();
 
  //for (std::vector<std::pair<L0MuonCandidate*, std::vector<int> > >::
  //     iterator ioff =
  //     m_offForCand.begin(); ioff != m_offForCand.end(); ioff++){
  //*m_log << MSG::DEBUG  << (*ioff).first->pt() << endreq;
  //}
  
  

    for (std::vector<std::pair<L0MuonCandidate*, std::vector<int> > >::
           iterator ioff =
           m_offForCand.begin(); ioff != m_offForCand.end(); ioff++){
      cr->fillOffset(*ioff);
    }
    

    //Call to L0Buffer for PU and BCSU: 
    //addresses for candidates 

    //bool buffer = mpu->writeL0Buffer();
    if ( mpu->writeL0Buffer() ){
      L0Muon::RegisterFactory* rfactory = L0Muon::RegisterFactory::instance();
      BuildL0BufferUnit* l0buffer= dynamic_cast<BuildL0BufferUnit*>(m_parent->subUnit("l0buf"));
     
      for (int icand= 0; icand<2; icand++){
        int nbits = m_tower.addr(icand).size();
    
      // Prepare registers with addresser for candidates
          char * name = "(R%d,%d,%d)_addr_candidate%d";
          char buf[4096];     
          sprintf(buf,name,m_pu.region(), m_pu.nX(), m_pu.nY(),icand+1);
          TileRegister* pReg= rfactory->createTileRegister(buf,nbits);
          pReg->setType("Outputfield");     
          pReg->set(m_tower.addr(icand));
         
          //l0ad address in l0buffer
          if (l0buffer != NULL){
            l0buffer->addInputRegister(pReg);
          }
 
          if (bcsu != NULL){
            bcsu->addInputRegister(pReg);
          }
          
      }
      
        
        
    
      //
      // Status world for candidates: Prepare register with status word
      char * name = "(R%d,%d,%d)_status";      
      char buf[4096];      
      sprintf(buf,name,m_pu.region(), m_pu.nX(), m_pu.nY());      
      unsigned long int status= m_cand.size()<3 ? m_cand.size() : 3ul;
      boost::dynamic_bitset<> bits(4,status);
      int nbits = bits.size();
      TileRegister* pReg= rfactory->createTileRegister(buf,nbits);
      pReg->setType("Outputfield");
      pReg->set(bits);

      // load register in l0buf
      
      if (l0buffer != NULL){
        l0buffer->addInputRegister(pReg);
      }
          //      
      if (bcsu != NULL){
        bcsu->addInputRegister(pReg);
      }
      
    }
    
    
     
        
        // load L0MuonCandidates in BCSU
        //==============
        if (m_cand.size()>0 && m_cand.size()<3){
          for (std::vector<L0MuonCandidate*>::iterator icand= m_cand.begin();
               icand < m_cand.end(); icand++){
            bcsu->loadCandidates(*icand);           
          }
          

          if (m_cand.size()==1){            
            bcsu->loadCandidates(new L0MuonCandidate(L0Muon::PU_EMPTY));            
          }
          
          bcsu->loadStatus(L0Muon::OK);
          
        }
        
        if (m_cand.size() >2){  
          for (std::vector<L0MuonCandidate*>::iterator icand= m_cand.begin();
               icand < m_cand.begin()+2; icand++){
            bcsu->loadCandidates(*icand);
            
          }          
          bcsu->loadStatus(L0Muon::OK);
        }
        
        if (m_cand.size()==0){
          m_status =0;
          bcsu->loadCandidates(new L0MuonCandidate(L0Muon::PU_EMPTY));
          bcsu->loadCandidates(new L0MuonCandidate(L0Muon::PU_EMPTY));
          bcsu->loadStatus(1);
          
        }
        
          
          
    

  
        
}

        











void L0Muon::CablingUnit::finalize() {
  m_pads.clear();
  releaseRegisters();
}




