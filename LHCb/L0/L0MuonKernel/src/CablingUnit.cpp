#include <iostream>

#include "L0MuonKernel/CablingUnit.h"
#include "L0MuonKernel/CrateUnit.h"
#include "L0MuonKernel/BuildL0BufferUnit.h"
#include "L0MuonKernel/BestCandidateSelectionUnit.h"


L0Muon::CablingUnit::CablingUnit() {  
    
 
  m_status = 0;
 
}
    
L0Muon::CablingUnit::~CablingUnit() {};  

void L0Muon::CablingUnit::makePads() {
  m_pads.clear();
 
  std::map<std::string,Register*>::iterator ir;

  
  for ( ir = m_inputs.begin(); ir != m_inputs.end(); ir++ ) {
    TileRegister* itr = dynamic_cast<TileRegister*>(ir->second);
    if ( ! itr->empty() ) {
      //if (m_debug) std::cout << "Register name " << itr->name() << std::endl;
      //if (m_debug) std::cout << "Cabling: " << " " << ir->first << " " <<std::endl;
      boost::dynamic_bitset<> r = itr->getBitset();
      //if (m_debug) std::cout << "Cabling: " << " reg size" << " " <<r.size() << std::endl;
      std::vector<MuonTileID> tmp = itr->firedTiles();
      std::vector<MuonTileID>::iterator itmp;
      if (m_debug) std::cout << "Cabling: " << "Fired tiles: " << std::endl;
      for (itmp = tmp.begin(); itmp!= tmp.end(); itmp++){
	if (m_debug) std::cout << " " << (*itmp).toString()<< std::endl;
      }

      itr->makePads();

      std::vector<MuonTileID> pads = itr->Pads();
      std::vector<MuonTileID>::iterator  ipads ;    
      for (ipads = pads.begin(); ipads != pads.end(); ipads++){
	m_pads.push_back(*ipads);

      }
    }           
  }      
}


void L0Muon::CablingUnit::makeTower() {

  unsigned int nreg = m_pu.region();
  MuonLayout layout(48,8);
  MuonLayout pulayout(2,2);

  MuonTileID refpad = MuonTileID(m_pu,layout,0,0);
  int refX = refpad.nX();
  int refY = refpad.nY();

  std::vector<MuonTileID>::iterator ip ;
  std::vector<MuonTileID>::iterator itmp;

  
  for (ip=m_pads.begin(); ip != m_pads.end(); ip++) {

    //if (m_debug) std::cout << "Cabling: " << "pad " << (*ip).toString() << std::endl;
    int nsta = ip->station();
    std::vector<MuonTileID> tmp;
    
    
    if ( ip->region() == nreg ) {
      // if (m_debug) std::cout << "Cabling: " << "ip->region==nreg" << std::endl;
      tmp = layout.tiles(*ip);
    } else if ( ip->region() != nreg){
      //if (m_debug) std::cout << "ip->region != nreg" << std::endl;
      //if (m_debug) std::cout << "quarter " << (*ip).quarter() << " region " 
      //    << (*ip).region() << " nreg " << nreg << std::endl;
      tmp = layout.tilesInRegion((*ip), nreg); 
    }
    
    // if (m_debug) {
//       std::cout << "Cabling: " << "tiles for this pad" << std::endl;
//       for ( itmp = tmp.begin(); itmp != tmp.end(); itmp++ ) {
// 	std::cout << "station: " << nsta << " " << (*itmp).toString() << std::endl;
//       }
//     } 
     
     for ( itmp = tmp.begin(); itmp != tmp.end(); itmp++ ) {


       MuonTileID puid = pulayout.contains(*itmp);

       int nYindex= (itmp->nY())-refY+m_tower.maxYFoi(nsta);
       int nXindex= (itmp->nX())-refX+m_tower.maxXFoi(nsta);

       std::pair<int, int>  yx = std::make_pair(nYindex,nXindex);

     
       m_tower.setBit(nsta, nYindex, nXindex );

       //if (m_debug) std::cout << "XY" << " " << nXindex <<
       //" " <<  nYindex << " " << "sta" << 
       //" " << nsta << " " << (*itmp).quarter() << " " 
       ///  << (*itmp).region() << " " 
       //<< (*itmp).nX() << " " << (*itmp).nY() << std::endl;
       m_tower.setPadIdMap(nsta, yx, *ip);
             
     }     
  } 
}


void L0Muon::CablingUnit::initialize() {

  L0Muon::Unit::initialize();
  m_cand.clear();
  m_offForCand.clear();
  if (m_debug) m_tower.setDebugMode();
  
  
}

void L0Muon::CablingUnit::execute() {

  if (m_debug) std::cout << "Cabling: executing PU " << m_parent->name() << std::endl; 

  Unit * myBoard = m_parent->parent();
  BoardUnit * bu = dynamic_cast<BoardUnit*>(myBoard);
  
  BestCandidateSelectionUnit* bcsu= dynamic_cast<BestCandidateSelectionUnit*>
    (bu->subUnit("bcsu"));
  
 
  Unit * myCrate = myBoard->parent();
  CrateUnit * cr = dynamic_cast<CrateUnit*>(myCrate);
  
  m_tower.reset();

  makePads();
  makeTower();

  
  L0mProcUnit * mpu = dynamic_cast<L0mProcUnit*>(m_parent);
  
  
   for (int ista=0; ista<5; ista++){ 
     m_tower.setFoi(ista,mpu->xFoi(ista),mpu->yFoi(ista));
   }
      
   m_tower.setPtparam(mpu->ptParameters());
   m_tower.setIgnoreM1(mpu->ignoreM1());

  //IMuonTileXYZTool *iTileXYZTool = cr->getMuonTool();
  
  //m_tower.setMuonToolInTower(iTileXYZTool);
  m_tower.processTower(m_pu);
  m_cand = m_tower.puCandidates();

  m_offForCand =m_tower.candOffset();


  // Debug
  /*if (m_debug) std::cout << "m_offForCand.size" << m_offForCand.size() << std::endl;
 
  std::vector< std::pair<Candidate*, std::vector<int> > >::iterator ioff;  
  for (ioff = m_offForCand.begin(); ioff != m_offForCand.end(); ioff++){
    
    std::vector<int> tmp =(*ioff).second;
    

    if (m_debug) std::cout << "Pt of the candidate = " << (*ioff).first->pt()
        << std::endl;
    
    if (m_debug) std::cout << "Offsets = " << tmp[0] << std::endl;
    if (m_debug) std::cout << "Offsets = " << tmp[1] << std::endl;
    if (m_debug) std::cout << "Offsets = " << tmp[2] << std::endl;
    if (m_debug) std::cout << "Offsets = " << tmp[3] << std::endl;
    if (m_debug) std::cout << "Offsets = " << tmp[4] << std::endl;
    if (m_debug) std::cout << "Offsets = " << tmp[5] << std::endl;
    if (m_debug) std::cout << "Offsets = " << tmp[6] << std::endl;
    if (m_debug) std::cout << "Offsets = " << tmp[7] << std::endl;
    if (m_debug) std::cout << "Offsets = " << tmp[8] << std::endl;
    if (m_debug) std::cout << "Offsets = " << tmp[9] << std::endl;
    
    
  }
  */  
  //=============================================
 
  if (m_cand.size()>0 && m_cand.size()<3){
    m_status = L0MuonStatus::OK;
  } else if (m_cand.size()>2){
    m_status = L0MuonStatus::PU_OVERFLOW;
  }
  if (m_debug) std::cout << "Cabling: " << "Number of Candidates" <<" " 
      << m_cand.size() << std::endl;

    //addresses for candidates (L0Buffers)


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
    
    
     
        
        // load Candidates and Offsets in BCSU (the first two candidates are transmitted to BCSU
        if (m_cand.size()>0 && m_cand.size()<3){
          for (std::vector<Candidate*>::iterator icand= m_cand.begin();
               icand < m_cand.end(); icand++){
            bcsu->loadCandidates(*icand);           
          }
          

          if (m_cand.size()==1){            
            bcsu->loadCandidates(new Candidate(L0MuonStatus::PU_EMPTY));            
          }
          
          bcsu->loadStatus(L0MuonStatus::OK);
          
        }
        
        if (m_cand.size() >2){  
          for (std::vector<Candidate*>::iterator icand= m_cand.begin();
               icand < m_cand.begin()+2; icand++){
            bcsu->loadCandidates(*icand);
            
          }          
          bcsu->loadStatus(L0MuonStatus::OK);
        }
        
        if (m_cand.size()==0){
          m_status =0;
          bcsu->loadCandidates(new Candidate(L0MuonStatus::PU_EMPTY));
          bcsu->loadCandidates(new Candidate(L0MuonStatus::PU_EMPTY));
          bcsu->loadStatus(1);
          
        }
        
          
          
        
        if (m_offForCand.size()>0 && m_offForCand.size()<3){
          
    
          for (std::vector< std::pair<Candidate*, 
                 std::vector<int> > >::iterator ioff= m_offForCand.begin();
               ioff < m_offForCand.end(); ioff++){
            
           
            bcsu->loadOffsets(*ioff);
            
          }
          
        
    
        if (m_offForCand.size()==1){
            
          std::vector<int> tmp;
            
          for (int iv =0; iv<10; iv++){
                     
            tmp.push_back(0);
              
          }
            
      
          std::pair<Candidate* , std::vector<int> > empty = 
            std::make_pair(new Candidate(L0MuonStatus::PU_EMPTY), tmp);
            

            bcsu->loadOffsets(empty);
            
            
          }
          
      
      
      
    
    
    
          
        }
        
        
 
        if (m_offForCand.size()>2){
          
          for (std::vector< std::pair<Candidate*, 
                 std::vector<int> > >::iterator ioff= m_offForCand.begin();
               ioff < m_offForCand.begin()+2; ioff++){
            
            bcsu->loadOffsets(*ioff);
            
          }
          
        }
        
        if (m_offForCand.size()==0){
          
          std::vector<int> tmp;
          
          for (int iv =0; iv<10; iv++){
            
       
            tmp.push_back(0);
            
          }
          
          
          std::pair<Candidate* , std::vector<int> > empty = 
            std::make_pair(new Candidate(L0MuonStatus::PU_EMPTY), tmp);
          
          bcsu->loadOffsets(empty);
          
          bcsu->loadOffsets(empty);
          
        }
        
        
}





void L0Muon::CablingUnit::finalize() {
  m_pads.clear();
  releaseRegisters();
}




