#include <iostream>

#include "L0MuonKernel/CoreUnit.h"
#include "L0MuonKernel/MuonTriggerUnit.h"


L0Muon::CoreUnit::CoreUnit() {
}
    
L0Muon::CoreUnit::CoreUnit(LHCb::MuonTileID id):L0MUnit(id) {
}
    
L0Muon::CoreUnit::CoreUnit(DOMNode* pNode):L0MUnit(pNode) {  
  m_tower = Tower();
}
    
L0Muon::CoreUnit::~CoreUnit() {};  

bool L0Muon::CoreUnit::makePads() {
  // Construct the logical pads list from the input tiles (OL+neighbours)
  // Return false if the pad list is empty
  
  m_pads.clear();
 
  std::map<std::string,Register*>::iterator ir;


  // Loop over input registers (OL and input neighbours) and construct logical pads
  for ( ir = m_inputs.begin(); ir != m_inputs.end(); ir++ ) {
    TileRegister* itr = dynamic_cast<TileRegister*>(ir->second);
    
    if (m_debug) std::cout << "*!! Core:makePads: register " << itr->name() << "is empty ?"<<itr->empty()<< std::endl;
  
    // If the input register is not empty
    if ( ! itr->empty() ) {
      if (m_debug) std::cout << "*!! Core:makePads: register key  " << ir->first   << std::endl;
      if (m_debug) std::cout << "*!! Core:makePads: register name " << itr->name() << std::endl;
      boost::dynamic_bitset<> r = itr->getBitset();
      if (m_debug) std::cout << "*!! Core:makePads: reg size" << " " <<r.size() << std::endl;
      std::vector<LHCb::MuonTileID> tmp = itr->firedTiles();
      std::vector<LHCb::MuonTileID>::iterator itmp;
      if (m_debug) std::cout << "*!! Core:makePads: fired tiles (" << tmp.size() <<"):"<< std::endl;
      for (itmp = tmp.begin(); itmp!= tmp.end(); itmp++){
        if (m_debug) std::cout << "*!! Core:makePads:   " << (*itmp).toString()<< std::endl;
      }

      itr->makePads();
      if (m_debug) std::cout << "*!! Core:makePads: itr->makePads() done" << std::endl;

      std::vector<LHCb::MuonTileID> pads = itr->Pads();
      if (m_debug) std::cout << "*!! Core:makePads: pads.size= "<<pads.size() << std::endl;
      std::vector<LHCb::MuonTileID>::iterator  ipads ;    
      for (ipads = pads.begin(); ipads != pads.end(); ipads++){
        m_pads.push_back(*ipads);
        if (m_debug) std::cout << "*!! Core:makePads:   " << (*ipads).toString() << std::endl;
      }
      if (m_debug) std::cout << "*!! Core:makePads: m_pads.size()= " << m_pads.size() << std::endl;
    } // End if the input register is not empty
  } // End of Loop over input registers

  if (m_debug) std::cout << "*!! Core:makePads: register  m_pads.empty() ? " <<m_pads.empty() << std::endl;
  if (m_pads.empty()) return false;
  if (m_debug) std::cout << "*!! Core:makePads: register  return true" << std::endl;
  return true;
}


bool L0Muon::CoreUnit::makeTower() {
  // Loop over the fired pads and set the bittable of the tower
  // (pad in the local PU frame with the M3 granularity)
  // and the dictionnary containing the relation between
  // the bits and the MuonTileID

  bool seedFound=false;

  unsigned int pureg = m_mid.region();

  // Layout of pads in M3
  MuonLayout layout(48,8);
  // Layout of PU
  MuonLayout pulayout(2,2);

  // Reference of the first pad (closest to the beam)
  LHCb::MuonTileID refpad = LHCb::MuonTileID(m_mid,layout,0,0);
  int refX = refpad.nX();
  int refY = refpad.nY();
  if (m_debug) std::cout << "*!! Core:makeTower " << "refpad " << refpad.toString() << std::endl;

  std::vector<LHCb::MuonTileID>::iterator ip ;
  std::vector<LHCb::MuonTileID>::iterator ipM3gran;

  // Loop over fired pads
  for (ip=m_pads.begin(); ip != m_pads.end(); ip++) {

    if (m_debug) std::cout << "*!! Core:makeTower " << "pad " << (*ip).toString() << std::endl;
    // Pad's station
    int sta = ip->station();

    // Pad's region
    unsigned int ipreg = ip->region();
 
    // if a pad is found in M3, a seed is found: set the flag
    if (sta==2) seedFound=true;

    // Vector of tiles with M3 granularity containing the fired pad
    std::vector<LHCb::MuonTileID> padsM3gran;    
    padsM3gran = layout.tiles(*ip);
 
    // Loop over the tiles in M3 granularity 
    for ( ipM3gran = padsM3gran.begin(); ipM3gran != padsM3gran.end(); ipM3gran++ ) {
      if (m_debug) std::cout << "*!! Core:makeTower ip->region pad in M3 granularity: " << (*ipM3gran).toString()<< std::endl;
  
      // Coordinates in Pu's region
      if (ipreg>pureg) {
        for (int ix=0;ix<2;++ix) {
          // Modify the coordinates to match the pu's region
          int nX = ix+ ipM3gran->nX()*2;
          for (int iy=0;iy<2;++iy) {
            int nY = iy+ ipM3gran->nY()*2;
            // Local coordinated of the pad (PU frame)
            int nXindex= nX-refX+m_tower.maxXFoi(sta);    
            int nYindex= nY-refY+m_tower.maxYFoi(sta);
            std::pair<int, int>  yx = std::make_pair(nYindex,nXindex);
         
            // Set the corresponding bit in the tower
            m_tower.setBit(sta, nYindex, nXindex );
            // Fill the map relating the local coordinates and the MuonTileID
            m_tower.setPadIdMap(sta, yx, *ip);
         
            if (m_debug) std::cout << "*!! Core:makeTower UPPER region"
                                   << " sta= "<<sta
                                   << " nX= "<<nX<< " nY= "<<nY
                                   << " y= "<<yx.first<<",x="<<yx.second
                                   << " *ip= "<<(*ip).toString()
                                   << " ipM3gran= "<<(*ipM3gran).toString()
                                   << std::endl;
          }
        }
     
      } else if (ipreg<pureg) {
        // Modify the coordinates to match the pu's region
        int nX = ipM3gran->nX()/2;
        int nY = ipM3gran->nY()/2;
        // Local coordinated of the pad (PU frame)
        int nXindex= nX-refX+m_tower.maxXFoi(sta);    
        int nYindex= nY-refY+m_tower.maxYFoi(sta);
        std::pair<int, int>  yx = std::make_pair(nYindex,nXindex);
     
        // Set the corresponding bit in the tower
        m_tower.setBit(sta, nYindex, nXindex );
        // Fill the map relating the local coordinates and the MuonTileID
        m_tower.setPadIdMap(sta, yx, *ip);

        if (m_debug) std::cout << "*!! Core:makeTower LOWER region" 
                               << " sta= "<<sta
                               << " nX= "<<nX<< " nY= "<<nY
                               << " yx= "<<yx.first<<","<<yx.second
                               << " *ip= "<<(*ip).toString()
                               << " ipM3gran= "<<(*ipM3gran).toString()
                               << std::endl;
      } else {
        // Local coordinated of the pad (PU frame)
        int nXindex= (ipM3gran->nX())-refX+m_tower.maxXFoi(sta);    
        int nYindex= (ipM3gran->nY())-refY+m_tower.maxYFoi(sta);
        std::pair<int, int>  yx = std::make_pair(nYindex,nXindex);
     
        // Set the corresponding bit in the tower
        m_tower.setBit(sta, nYindex, nXindex );
     
        // Fill the map relating the local coordinates and the MuonTileID
        m_tower.setPadIdMap(sta, yx, *ip);

        if (m_debug) std::cout << "*!! Core:makeTower SAME region"
                               << " sta= "<<sta
                               << " nX= "<<ipM3gran->nX()<< " nY= "<<ipM3gran->nY()
                               << " yx= "<<yx.first<<","<<yx.second
                               << " *ip= "<<(*ip).toString()
                               << " ipM3gran= "<<(*ipM3gran).toString()
                               << std::endl;
      }
   
    } // End of Loop over the tiles in M3 granularity 
  } // End of Loop over fired pads

  if (m_debug) std::cout << "*!! Core:makeTower seedFound " <<seedFound<< std::endl;
  return seedFound;
}

void L0Muon::CoreUnit::initializeM1TowerMap() {
  // When ignoring M1, the extrapolated position in M1 is used instead of
  // the hit. In that case, the IdMap in Tower does not contain the
  // M1 pad corresponding to the extrapolation.
  // The table of correspondance between position in tower and pad in M1
  // is filled in this function which should be called during the initialization 
  // phase.
  unsigned int pureg = m_mid.region();

  // Layout of pads in M3
  MuonLayout layout(48,8);
  // Layout of pads in M1
  MuonLayout layoutM1(24,8);
  // Layout of PU
  MuonLayout pulayout(2,2);

  // Reference of the first pad (closest to the beam)
  LHCb::MuonTileID refpad = LHCb::MuonTileID(m_mid,layout,0,0);
  int refX = refpad.nX();
  int refY = refpad.nY();
  if (m_debug) std::cout << "*!! Core:initializeM1TowerMap " << "refpad " << refpad.toString() << std::endl;

  std::vector<LHCb::MuonTileID>::iterator ip ;
  std::vector<LHCb::MuonTileID>::iterator ipM3gran;


  // Set station
  int sta=0;

  // Loop over M1 pads 
  std::vector<LHCb::MuonTileID> pads = layoutM1.tilesInArea(m_mid,m_tower.maxXFoi(1)/2,0);
  for (ip=pads.begin(); ip != pads.end(); ip++) {

    if (m_debug) std::cout << "*!! Core:initializeM1TowerMap " << "pad " << (*ip).toString() << std::endl;

    // Pad's region
    unsigned int ipreg = ip->region();

    // Vector of tiles with M3 granularity containing the fired pad
    std::vector<LHCb::MuonTileID> padsM3gran;    
    padsM3gran = layout.tiles(*ip);

    // Loop over the tiles in M3 granularity 
    for ( ipM3gran = padsM3gran.begin(); ipM3gran != padsM3gran.end(); ipM3gran++ ) {
      if (m_debug) std::cout << "*!! Core:makeTower ip->region pad in M3 granularity: " << (*ipM3gran).toString()<< std::endl;
     
      // Coordinates in Pu's region
      if (ipreg>pureg) {
        for (int ix=0;ix<2;++ix) {
          // Modify the coordinates to match the pu's region
          int nX = ix+ ipM3gran->nX()*2;
          for (int iy=0;iy<2;++iy) {
            int nY = iy+ ipM3gran->nY()*2;
            // Local coordinated of the pad (PU frame)
            int nXindex= nX-refX+m_tower.maxXFoi(sta);    
            int nYindex= nY-refY+m_tower.maxYFoi(sta);
            std::pair<int, int>  yx = std::make_pair(nYindex,nXindex);
            
            // Set the corresponding bit in the tower
            m_tower.setBit(sta, nYindex, nXindex );
            // Fill the map relating the local coordinates and the MuonTileID
            m_tower.setPadIdMap(sta, yx, *ip);
            
            if (m_debug) std::cout << "*!! Core:makeTower UPPER region"
                                   << " sta= "<<sta
                                   << " nX= "<<nX<< " nY= "<<nY
                                   << " y= "<<yx.first<<",x="<<yx.second
                                   << " *ip= "<<(*ip).toString()
                                   << " ipM3gran= "<<(*ipM3gran).toString()
                                   << std::endl;
          }
        }
        
      } else if (ipreg<pureg) {
        // Modify the coordinates to match the pu's region
        int nX = ipM3gran->nX()/2;
        int nY = ipM3gran->nY()/2;
        // Local coordinated of the pad (PU frame)
        int nXindex= nX-refX+m_tower.maxXFoi(sta);    
        int nYindex= nY-refY+m_tower.maxYFoi(sta);
        std::pair<int, int>  yx = std::make_pair(nYindex,nXindex);
        
        // Set the corresponding bit in the tower
        m_tower.setBit(sta, nYindex, nXindex );
        // Fill the map relating the local coordinates and the MuonTileID
        m_tower.setPadIdMap(sta, yx, *ip);

        if (m_debug) std::cout << "*!! Core:makeTower LOWER region" 
                               << " sta= "<<sta
                               << " nX= "<<nX<< " nY= "<<nY
                               << " yx= "<<yx.first<<","<<yx.second
                               << " *ip= "<<(*ip).toString()
                               << " ipM3gran= "<<(*ipM3gran).toString()
                               << std::endl;
      } else {
        // Local coordinated of the pad (PU frame)
        int nXindex= (ipM3gran->nX())-refX+m_tower.maxXFoi(sta);    
        int nYindex= (ipM3gran->nY())-refY+m_tower.maxYFoi(sta);
        std::pair<int, int>  yx = std::make_pair(nYindex,nXindex);
        
        // Set the corresponding bit in the tower
        m_tower.setBit(sta, nYindex, nXindex );
        
        // Fill the map relating the local coordinates and the MuonTileID
        m_tower.setPadIdMap(sta, yx, *ip);

        if (m_debug) std::cout << "*!! Core:makeTower SAME region"
                               << " sta= "<<sta
                               << " nX= "<<ipM3gran->nX()<< " nY= "<<ipM3gran->nY()
                               << " yx= "<<yx.first<<","<<yx.second
                               << " *ip= "<<(*ip).toString()
                               << " ipM3gran= "<<(*ipM3gran).toString()
                               << std::endl;
      }
      
    } // End of Loop over the tiles in M3 granularity 
  } // End of Loop over M1 pads
}

void L0Muon::CoreUnit::initialize() {

  // Get a pointer to the parent Crate Unit
  MuonTriggerUnit * pmuontrigger = dynamic_cast<MuonTriggerUnit *>( parentByType("MuonTriggerUnit"));

  // Set the NO M1 flag
  m_tower.setIgnoreM1(pmuontrigger->ignoreM1());
  if (pmuontrigger->ignoreM1()==true) initializeM1TowerMap();

  // Set the foi
  for (int ista=0; ista<5; ista++){ 
    m_tower.setFoi(ista,pmuontrigger->xFoi(ista),pmuontrigger->yFoi(ista));
  }
  
  // Set the pt parameters
  m_tower.setPtparam(pmuontrigger->ptParameters());

  // Set the emulator version
  m_tower.setProcVersion(pmuontrigger->procVersion());

  // Candidate Register handler for output candidates
  char buf[4096];
  char* format ;
  format = "CAND_PUQ%dR%d%d%d";
  sprintf(buf,format,m_mid.quarter()+1,m_mid.region()+1,m_mid.nX(),m_mid.nY());
  std::map<std::string,L0Muon::Register*>::iterator itout =  m_outputs.find(buf);
  if (itout==m_outputs.end()) {
    std::cout <<"L0Muon::CoreUnit::initialize "<<m_mid 
              <<" key "<<buf  <<" not found in input registers\n";
    exit(-1);
  }
  
  Register* reg =(*itout).second;
  m_candRegHandlerOut = CandRegisterHandler(reg) ;    

  L0MUnit::initialize();
  
}

void L0Muon::CoreUnit::setDebugMode(bool debug) {
  m_debug = debug;
  if (debug) std::cout <<"*!* "<< type() <<"::setDebugMode" << std::endl;
  m_tower.setDebugMode(debug);  
}


void L0Muon::CoreUnit::execute() {

  if (m_debug) std::cout << "*!* CoreUnit::execute " << std::endl;
  
  // Construct logical pads for input registers 
  // (fill the m_pads vector of MuonTileIDs)
  // Return if the pad list is empty
  if (makePads()==false) return;
  if (m_debug) std::cout << "*!* CoreUnit::execute after makePads " << std::endl; 

  // Reset the tower
  if (m_debug) std::cout << "*!* CoreUnit::execute reseting tower ..." << std::endl;
  m_tower.reset();
  if (m_debug) std::cout << "*!* CoreUnit::execute after tower reset." << std::endl;

  // Fill the tower with the fired pads (in M3 granularity)
  // Return if no seed has been found
  if (makeTower()==false) return;
  if (m_debug) std::cout << "*!* CoreUnit::execute after makeTower "<< std::endl;
  
  // Process the tower
  // Return if no candidate has been found
  std::vector<PMuonCandidate> candidates = m_tower.processTower(m_mid);
  if (m_debug) std::cout << "*!* CoreUnit::execute after m_tower.processTower "<< std::endl;
  if (m_debug) std::cout << "*!* CoreUnit::execute candidates size= "<<candidates.size()<< std::endl;
  if (candidates.empty()) return;

  // Fill the status register 
  int ncand=candidates.size();
  if (m_debug) std::cout << "*!* CoreUnit::execute candidate ncand= "<<ncand<< std::endl;
  // Fill the candidate register with first and last candidates (match according hardware behavior)
  if (ncand>0) {
    m_candRegHandlerOut.setMuonCandidate(candidates[0],0);
    if (ncand>1) m_candRegHandlerOut.setMuonCandidate(candidates[ncand-1],1);
  }
  
  if (ncand>2) ncand=3;
  m_candRegHandlerOut.setStatus(ncand);
  if (m_debug) std::cout << "*!* CoreUnit::execute after m_candRegHandlerOut.setCandStatus(ncand); ncand= "<<ncand<<std::endl;


//   // Fill the candidate register with first 2 candidates
//   if (ncand<3) { // TEMPORARY - THIS RESTRICTION IS THERE TO MATCH THE HARDWARE - JC 19/12/06
//     int icand=0; 
//     std::vector<PMuonCandidate>::iterator itcand;
//     for (itcand=candidates.begin();itcand<candidates.end();itcand++) {
//       if (m_debug) std::cout << "*!* CoreUnit::execute inside loop over candidates icand= "<<icand<< std::endl;
//       m_candRegHandlerOut.setMuonCandidate(*itcand,icand);
//       if (m_debug) std::cout << "*!* CoreUnit::execute inside loop over candidates : cand set in register "<< std::endl;
//       icand++;
//       if (icand==2) break;
//     }
//   } // END OF TEMPORARY CONDITION     
}

bool L0Muon::CoreUnit::preprocess() {

  if (m_debug) std::cout << "*!* CoreUnit::preprocess " << std::endl;
  
  // Reset the tower
  if (m_debug) std::cout << "*!* CoreUnit::preprocess reseting tower ..." << std::endl;
  m_tower.reset();
  if (m_debug) std::cout << "*!* CoreUnit::preprocess tower reset." << std::endl;

  // Construct logical pads for input registers 
  // (fill the m_pads vector of MuonTileIDs)
  // Return if the pad list is empty
  if (makePads()==false) return false;
  if (m_debug) std::cout << "*!* CoreUnit::preprocess after makePads " << std::endl; 

  // Fill the tower with the fired pads (in M3 granularity)
  // Return if no seed has been found
  if (makeTower()==false) return false;
  if (m_debug) std::cout << "*!* CoreUnit::preprocess after makeTower "<< std::endl;

  return true;
  
}

std::vector<L0Muon::PMuonCandidate> L0Muon::CoreUnit::process(std::vector<int> xfoi , std::vector<int> yfoi  )
{
  int xfoiM1=xfoi[0];
  int xfoiM2=xfoi[1];
  int xfoiM4=xfoi[3];
  int xfoiM5=xfoi[4];
  int yfoiM4=yfoi[3];
  int yfoiM5=yfoi[4];
  
  return process( xfoiM1, xfoiM2, xfoiM4, xfoiM5, yfoiM4, yfoiM5);
  
}

std::vector<L0Muon::PMuonCandidate> L0Muon::CoreUnit::process(int * xfoi , int * yfoi  )
{
  int xfoiM1=xfoi[0];
  int xfoiM2=xfoi[1];
  int xfoiM4=xfoi[3];
  int xfoiM5=xfoi[4];
  int yfoiM4=yfoi[3];
  int yfoiM5=yfoi[4];
  
  return process( xfoiM1, xfoiM2, xfoiM4, xfoiM5, yfoiM4, yfoiM5);
  
}

std::vector<L0Muon::PMuonCandidate> L0Muon::CoreUnit::process(int xfoiM1,int xfoiM2,int xfoiM4,int xfoiM5,int yfoiM4,int yfoiM5)
{

  
  if (m_debug) std::cout << "*!* CoreUnit::process " << std::endl;

  // Set the foi
  m_tower.setFoi(0,xfoiM1,0);
  m_tower.setFoi(1,xfoiM2,0);
  m_tower.setFoi(2,0,0);
  m_tower.setFoi(3,xfoiM4,yfoiM4);
  m_tower.setFoi(4,xfoiM5,yfoiM5);

  
  // Process the tower
  std::vector<L0Muon::PMuonCandidate> candidates = m_tower.processTower(m_mid);

  return candidates;
  
  
}


void L0Muon::CoreUnit::postexecute() {
  releaseInputRegisters();
  releaseOutputRegisters();
}




