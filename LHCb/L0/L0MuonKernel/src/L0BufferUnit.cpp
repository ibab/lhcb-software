#include "L0MuonKernel/L0BufferUnit.h"
#include "L0MuonKernel/ProcUnit.h"
#include "L0MuonKernel/CrateUnit.h"
#include "L0MuonKernel/MuonTriggerUnit.h"
#include "L0mConf/L0MBase.h" 
#include <cmath>
#include <iostream>

#define UNKNOWN "Unknown"

#define  XM1FOI_OFFSET    5*16+8
#define  XM2FOI_OFFSET    5*16+4
#define  XM4FOI_OFFSET    5*16+0
#define  XM5FOI_OFFSET    4*16+8
#define  YM4FOI_OFFSET    4*16+4
#define  YM5FOI_OFFSET    4*16+0
#define  CAND1_OFFSET     3*16+0
#define  CAND2_OFFSET     2*16+0
#define  STATUS_OFFSET    1*16+12
#define  BCPT_OFFSET      1*16+8

#define  OUTPUTFIELD_SIZE 4*16
// #define  L0BUFFER_SIZE    32*16
// #define  OL_SIZE          18*16  

#define  BCIDin_OFFSET    6*16+0
#define  OL_BCID_OFFSET   17*16+0
#define  BCID_SIZE        4


/**
   Constructor.
*/
L0Muon::L0BufferUnit::L0BufferUnit(){
  m_l0bufferFile = NULL;
};
/**
   Constructor.
*/
L0Muon::L0BufferUnit::L0BufferUnit(MuonTileID id):L0MUnit(id){
  m_l0bufferFile = NULL;
};

/**
   Constructor.
*/
L0Muon::L0BufferUnit::L0BufferUnit(DOMNode* pNode):L0MUnit(pNode){
  m_l0bufferFile = NULL;  
};

/**
   Open the Output file where to write the L0Buffers, and write the header.
*/
void L0Muon::L0BufferUnit::setOutputFile(MuonTileID puid, std::string filepath){

  // Open the output file 
  char * name = "%s/l0buf_sim_R%d%d%d.txt";
  char buf[4096];
  sprintf(buf,name,filepath.c_str(),puid.region()+1,puid.nX(),puid.nY());
  std::cout << "L0BufferUnit:: setOutputFile file name is "<< buf << std::endl;
  m_l0bufferFile = fopen(buf,"w");
  // // std::cout << "L0BufferUnit:: setOutputFile file is "<< buf << std::endl;

  // Write file header
  writeHeader();
}

/**
   Destructor
*/
L0Muon::L0BufferUnit::~L0BufferUnit(){
  if (m_l0bufferFile!=NULL) fclose(m_l0bufferFile);
}

/**
   Fill the L0Buffer output register.
*/
void L0Muon::L0BufferUnit::setL0buf(){
  if (m_debug) std::cout << "*!* L0BufferUnit::setL0buf" <<std::endl;
  
  // Get a pointer to the L0buffer output register
  TileRegister * pOutRegister = dynamic_cast<TileRegister*>(m_outputs["L0bOut"]);
  if (m_debug) std::cout << "*!* L0BufferUnit::setL0buf pOutRegister " <<pOutRegister<<std::endl;
  // Clear the ouptut register bit set
  pOutRegister->reset();

  std::vector<MuonTileID> lmids = pOutRegister->getTileVector();
  for (std::vector<MuonTileID>::iterator i_lmids = lmids.begin();i_lmids != lmids.end(); i_lmids ++){
    //if (m_debug) std::cout << "*!* L0BufferUnit::setL0buf  i_lmids: " <<i_lmids->toString()<<std::endl;
  }

  //
  // Update the input field of the bitset of the output register (OL + Neighbours) 
  //

  // Loop over input registers
  if (m_debug) std::cout << "*!* L0BufferUnit::setL0buf m_inputs.size()= " <<m_inputs.size()<<std::endl;
  std::map<std::string,L0Muon::Register*>::iterator ir;
  for (ir = m_inputs.begin(); ir!= m_inputs.end(); ir++){
    
    TileRegister * itr = dynamic_cast<TileRegister*>(ir->second);
    
    if (m_debug) std::cout << "*!* L0BufferUnit::setL0buf"
			   <<" itr->type()= " <<itr->type()
			   <<std::endl;
    // Filter the input register according to their type
    if (itr->type()=="OpticalLink" || itr->type()=="Neighbours") {

      // Get the list of fired tiles
      std::vector<MuonTileID> firedTilesList = itr->firedTiles();
      std::vector<MuonTileID>::iterator itfired;
      
      if (m_debug) std::cout << "*!* L0BufferUnit::setL0buf"
			     <<" itr->type()= " <<itr->type()
			     <<" firedTilesList.size()= "<< firedTilesList.size()<<std::endl;
     
      // For each fired tile, set to 1 the corresponding tiles in the output register bit set
      for (itfired=firedTilesList.begin(); itfired!= firedTilesList.end(); itfired++){
	if (m_debug) std::cout << "*!* L0BufferUnit::setL0buf  itfired: " <<itfired->toString()<<std::endl;
	pOutRegister->setTile(*itfired);
      }
    }
  }
  if (m_debug) std::cout << "*!* L0BufferUnit::setL0buf pOutRegister->firedTiles().size()= " 
			 <<pOutRegister->firedTiles().size()<<std::endl;

  //
  // Update the output field of the bitset of the output register (candidates + status + BCPT) 
  //

  // Get the bit set of the output register
  boost::dynamic_bitset<> outBitSet = pOutRegister->getBitset();

  //
  // Addresses and Status
  // 

  // Write the address of cand 1
  TileRegister * pCand1Register  = dynamic_cast<TileRegister*>(m_inputs["cand0"]);
  boost::dynamic_bitset<> cand1BitSet = pCand1Register->getBitset();
  for (boost::dynamic_bitset<>::size_type i =0; i < cand1BitSet.size();i++){
    int val=cand1BitSet[i] ;
    if (val) {
      outBitSet.set(i+CAND1_OFFSET);
    }
  }

  // Write the address of cand 2
  TileRegister * pCand2Register  = dynamic_cast<TileRegister*>(m_inputs["cand1"]);
  boost::dynamic_bitset<> cand2BitSet = pCand2Register->getBitset();
  for (boost::dynamic_bitset<>::size_type i =0; i < cand2BitSet.size();i++){
    int val=cand2BitSet[i] ;
    if (val) {
      outBitSet.set(i+CAND2_OFFSET);
    }
  }

  // Write the status
  TileRegister * pStatusRegister  = dynamic_cast<TileRegister*>(m_inputs["status"]);
  boost::dynamic_bitset<> statusBitSet = pStatusRegister->getBitset();
  for (boost::dynamic_bitset<>::size_type i =0; i < statusBitSet.size();i++){
    int val=statusBitSet[i] ;
    if (val) {
      outBitSet.set(i+STATUS_OFFSET);
    }
  }

  // Write the BCID
  boost::dynamic_bitset<> bcidSet(BCID_SIZE,(unsigned long) (m_l0EventNumber%16) );
  for (boost::dynamic_bitset<>::size_type i =0; i < bcidSet.size();i++){
    int val=bcidSet[i] ;
    if (val) {
      outBitSet.set(i+BCPT_OFFSET);
    }
  }

  // Update the bit set.
  pOutRegister->set(outBitSet);

}


void L0Muon::L0BufferUnit::initialize(){
  if (m_debug) std::cout << "*!* L0BufferUnit::initialize" <<std::endl;
  Unit* parent = parentByType("MuonTriggerUnit");
  std::string buildL0Buffer = parent->getProperty("BuildL0Buffer");
  m_buildL0Buffer = false;
  if (buildL0Buffer == "True") m_buildL0Buffer = true;
  m_l0EventNumber=0;
}

/**
   Execute event
*/
void L0Muon::L0BufferUnit::execute(){

  if (m_debug) std::cout << "*!* L0BufferUnit::execute m_buildL0Buffer= "<<m_buildL0Buffer <<std::endl;
  if ( ! m_buildL0Buffer ) return;
  
  setL0buf();
  if (m_debug) std::cout << "*!* L0BufferUnit::execute setL0buf done"<<std::endl;

  // Write out the L0Buffer
  writeEvent();
  if (m_debug) std::cout << "*!* L0BufferUnit::execute writeEvent done"<<std::endl;

  // Increment event counter
  m_l0EventNumber++;
  if (m_debug) std::cout << "*!* L0BufferUnit::execute l0EventNumber++ done"<<std::endl;
}


/*
  Write output file header 
*/
void L0Muon::L0BufferUnit::writeHeader(){

  if (m_l0bufferFile==NULL) return;

  int xFOI=0;
  int yFOI=0;

  int xM1 = xHardFoi(0);
  int xM2 = xHardFoi(1);
  int xM4 = xHardFoi(3);
  int xM5 = xHardFoi(4);

  int yM1 = yHardFoi(0);
  int yM2 = yHardFoi(1);
  int yM4 = yHardFoi(3);
  int yM5 = yHardFoi(4);

  xFOI |= ( xM5&0xF ) <<  0 ;
  xFOI |= ( xM4&0xF ) <<  4 ;
  xFOI |= ( xM2&0xF ) <<  8 ;
  xFOI |= ( xM1&0xF ) << 12 ;

  yFOI |= ( yM5&0xF ) <<  0 ;
  yFOI |= ( yM4&0xF ) <<  4 ;
  yFOI |= ( yM2&0xF ) <<  8 ;
  yFOI |= ( yM1&0xF ) << 12 ;
  

  fprintf(m_l0bufferFile,"#- PU\n");
  fprintf(m_l0bufferFile,"Q%d R%d%d%d\n",m_mid.quarter()+1,m_mid.region()+1,m_mid.nX(),m_mid.nY());
  fprintf(m_l0bufferFile,"#- FOI \n");
  fprintf(m_l0bufferFile,"%04x %04x\n",xFOI,yFOI);
  fprintf(m_l0bufferFile,"#- Mask \n");
  fprintf(m_l0bufferFile,"%04x %04x %04x\n",0,0xffff,0);

  
}

/**
   Write event to output file
*/
void L0Muon::L0BufferUnit::writeEvent(){

  if (m_debug) std::cout << "*!* L0BufferUnit::writeEvent m_l0bufferFile="<<m_l0bufferFile<<std::endl;
  if (m_l0bufferFile==NULL) return;

  // Write Event Header
  fprintf(m_l0bufferFile,"#- Header\n");
  fprintf(m_l0bufferFile,"%04x %04x\n",m_l0EventNumber%16,0);

  // Write L0Buffer
  fprintf(m_l0bufferFile,"#- L0Buffer\n");
  TileRegister * pL0bOut = dynamic_cast<TileRegister*>(m_outputs["L0bOut"]);
  if (m_debug) std::cout << "*!* L0BufferUnit::writeEvent pL0bOut="<<pL0bOut<<std::endl;
  if (pL0bOut!=0) pL0bOut->print_words(m_l0bufferFile);
  
  // Write Event Trailer
  fprintf(m_l0bufferFile,"#----\n");
}

/**
   Return the Foi in x for the given station
*/
int L0Muon::L0BufferUnit::xFoi(int sta){
  CrateUnit * pcrate = dynamic_cast<CrateUnit *>( parentByType("CrateUnit"));
  return pcrate->xFoi(sta);  
}

/**
   Return the Foi in y for the given station
*/
int L0Muon::L0BufferUnit::yFoi(int sta){
  CrateUnit * pcrate = dynamic_cast<CrateUnit *>( parentByType("CrateUnit"));
  return pcrate->yFoi(sta);  
}

/**
   Return the Foi in x for the given station in the hardware convention
*/
int L0Muon::L0BufferUnit::xHardFoi(int sta){
  int soft2hardxFoi[5]={2,1,0,4,4};
  return xFoi(sta)/soft2hardxFoi[sta];
  
}

/**
   Return the Foi in y for the given station in the hardware convention
*/
int L0Muon::L0BufferUnit::yHardFoi(int sta){
  int soft2hardyFoi[5]={1,1,0,1,1};
  return yFoi(sta)/soft2hardyFoi[sta];
}

