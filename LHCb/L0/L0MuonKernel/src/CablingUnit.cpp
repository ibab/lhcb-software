#include <iostream>

#include "L0MuonKernel/CablingUnit.h"
#include "L0MuonKernel/CrateUnit.h"
#include "L0MuonKernel/L0BufferUnit.h"
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
      if (m_debug) std::cout << "Cabling::makePads: register key  " << ir->first   << std::endl;
      if (m_debug) std::cout << "Cabling::makePads: register name " << itr->name() << std::endl;
      boost::dynamic_bitset<> r = itr->getBitset();
      if (m_debug) std::cout << "Cabling::makePads: reg size" << " " <<r.size() << std::endl;
      std::vector<MuonTileID> tmp = itr->firedTiles();
      std::vector<MuonTileID>::iterator itmp;
      if (m_debug) std::cout << "Cabling::makePads: fired tiles: " << std::endl;
      for (itmp = tmp.begin(); itmp!= tmp.end(); itmp++){
	//if (m_debug) std::cout << "Cabling::makePads:   " << (*itmp).toString()<< std::endl;
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
      if (m_debug) std::cout << "Cabling: ip->region != nreg" << std::endl;
      if (m_debug) std::cout << "Cabling: quarter " << (*ip).quarter() << " region " << (*ip).region() << " nreg " << nreg << std::endl;
      tmp = layout.tilesInRegion((*ip), nreg); 
    }
    
    for ( itmp = tmp.begin(); itmp != tmp.end(); itmp++ ) {


      MuonTileID puid = pulayout.contains(*itmp);
      
      int nYindex= (itmp->nY())-refY+m_tower.maxYFoi(nsta);
      int nXindex= (itmp->nX())-refX+m_tower.maxXFoi(nsta);
      
      std::pair<int, int>  yx = std::make_pair(nYindex,nXindex);
      
      
      m_tower.setBit(nsta, nYindex, nXindex );
      
//       if (m_debug) std::cout << "Cabling:   XY" 
// 			     << " " << nXindex  
// 			     << " " << nYindex 
// 			     << " " << (*itmp).toString() << std::endl;
      m_tower.setPadIdMap(nsta, yx, *ip);
      
    }     
  } 
}

void L0Muon::CablingUnit::bootstrap() {
  // Get a pointer to the parent Crate Unit
  CrateUnit * pcrate = dynamic_cast<CrateUnit *>( parentByType("CrateUnit"));

  // Set the foi
  for (int ista=0; ista<5; ista++){ 
    m_tower.setFoi(ista,pcrate->xFoi(ista),pcrate->yFoi(ista));
    if (m_debug) std::cout << "Cabling: FOI sta M" <<ista+1<< "X: "<< pcrate->xFoi(ista)<< "Y: "<< pcrate->yFoi(ista) << std::endl; 
  }
      
  // Set the pt parameters
  m_tower.setPtparam(pcrate->ptParameters());

  // Set the NO M1 flag
  m_tower.setIgnoreM1(pcrate->ignoreM1());
  
  L0Muon::Unit::bootstrap();
}

void L0Muon::CablingUnit::initialize() {

  L0Muon::Unit::initialize();
  m_cand.clear();
  m_offForCand.clear();
  if (m_debug) m_tower.setDebugMode();
  
  
}

void L0Muon::CablingUnit::execute() {

  if (m_debug) std::cout << "Cabling: executing PU " << m_parent->name() << std::endl; 

  // Get a pointer to the Board Unit
  Unit * myBoard = m_parent->parent();
  BoardUnit * bu = dynamic_cast<BoardUnit*>(myBoard);
  
  // Get a pointer to the BCSU
  BestCandidateSelectionUnit* bcsu= 
    dynamic_cast<BestCandidateSelectionUnit*> (bu->subUnit("bcsu"));
  
  // Get a pointer to the Crate Unit
  //  CrateUnit * cr = dynamic_cast<CrateUnit*>(myBoard->parent());

  
  // Reset the tower
  m_tower.reset();

  makePads();
  if (m_debug) std::cout << "Cabling: after makePads " << std::endl; 
  makeTower();
  if (m_debug) std::cout << "Cabling: after makeTower "<< std::endl; 

  
  //IMuonTileXYZTool *iTileXYZTool = cr->getMuonTool();  
  //m_tower.setMuonToolInTower(iTileXYZTool);
  
  m_tower.processTower(m_pu);
  m_cand = m_tower.puCandidates();

  // Debug
  /*if (m_debug) std::cout << "m_offForCand.size" << m_offForCand.size() << std::endl;
 
  std::vector< std::pair<PCandidate, std::vector<int> > >::iterator ioff;  
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

  // Addresses for candidates (L0Buffers)

  L0Muon::RegisterFactory* rfactory = L0Muon::RegisterFactory::instance();
  L0BufferUnit* l0buffer= dynamic_cast<L0BufferUnit*>(m_parent->subUnit("l0buf"));
     
  for (int icand= 0; icand<2; icand++){
    int nbits = m_tower.addr(icand).size();
    
    // Prepare registers with addresser for candidates
    char * name = "(R%d,%d,%d)_addr_candidate%d";
    char buf[4096];     
    sprintf(buf,name,m_pu.region(), m_pu.nX(), m_pu.nY(),icand+1);
    TileRegister* pReg= rfactory->createTileRegister(buf,nbits);
    pReg->setType("Outputfield");     
    pReg->set(m_tower.addr(icand));
         
    //load address in l0buffer
    if (l0buffer != NULL){
      char * name = "cand%d";      
      char buf[4096];      
      sprintf(buf,name,icand);      
      l0buffer->addInputRegister(pReg,buf);
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
    char * name = "status";      
    char buf[4096];      
    sprintf(buf,name);      
    l0buffer->addInputRegister(pReg,buf);
  }
  //      
  if (bcsu != NULL){
    bcsu->addInputRegister(pReg);
  }
        
  // load Candidates and Offsets in BCSU (the first two candidates are transmitted to BCSU
  if (m_cand.size()>0 && m_cand.size()<3){
    for (std::vector<PCandidate>::iterator icand= m_cand.begin();
	 icand < m_cand.end(); icand++){
      bcsu->loadCandidates(*icand);           
    }
    if (m_cand.size()==1){   
      //PCandidate pcand(new Candidate(L0MuonStatus::PU_EMPTY)) ;        
      //bcsu->loadCandidates(pcand);	      
    }
    bcsu->loadStatus(L0MuonStatus::OK);
  }
        
  if (m_cand.size() >2){  
    for (std::vector<PCandidate>::iterator icand= m_cand.begin();
	 icand < m_cand.begin()+2; icand++){
      bcsu->loadCandidates(*icand);
            
    }          
    bcsu->loadStatus(L0MuonStatus::OK);
  }
        
  if (m_cand.size()==0){
    m_status =0;
    //PCandidate pcand(new Candidate(L0MuonStatus::PU_EMPTY));
    //bcsu->loadCandidates(pcand);
    //PCandidate pcand1(new Candidate(L0MuonStatus::PU_EMPTY));
    //bcsu->loadCandidates(pcand1);
    bcsu->loadStatus(1);	  
  }        
}


void L0Muon::CablingUnit::finalize() {
  m_pads.clear();
  releaseRegisters();
}




