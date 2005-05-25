#include "L0MuonKernel/L0BufferUnit.h"
#include "L0MuonKernel/ProcUnit.h"
#include "L0MuonKernel/CrateUnit.h"
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
L0Muon::L0BufferUnit::L0BufferUnit(int & type){
   
  m_bufferType = type; // 0 for standard; 1 for PLL

  m_l0bufferFile = NULL;
  m_l0EventNumber=0;
  m_buildL0Buffer = false;

};

/**
   Open the Output file where to write the L0Buffers, and write the header.
*/
void L0Muon::L0BufferUnit::setOutputFile(MuonTileID puid, std::string suffixe){

  // Open the output file 
  char * name = "/marmuon3/NewInputFiles/l0buf_sim_R%d%d%d_%s.txt";
  char buf[4096];
  sprintf(buf,name,puid.region()+1,puid.nX(),puid.nY(),suffixe.c_str());
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

  if (m_bufferType == 0) {
    setL0bufStd();
  } else {
    setOLPLL();
    setL0bufPLL();
  }
}


/**
   Fill the L0Buffer output register with the "standard" format.
*/
void L0Muon::L0BufferUnit::setL0bufStd(){
  
  // Get a pointer to the L0buffer output register
  TileRegister * pOutRegister = dynamic_cast<TileRegister*>(m_outputs["L0bOut"]);
  // Clear the ouptut register bit set
  pOutRegister->reset();


  //
  // Update the input field of the bitset of the output register (OL + Neighbours) 
  //

  // Loop over input registers
  std::map<std::string,L0Muon::Register*>::iterator ir;
  for (ir = m_inputs.begin(); ir!= m_inputs.end(); ir++){
    
    TileRegister * itr = dynamic_cast<TileRegister*>(ir->second);
    
    // Filter the input register according to their type
    if (itr->Type()=="OpticalLink" || itr->Type()=="Neighbours") {

      // Get the list of fired tiles
      std::vector<MuonTileID> firedTilesList = itr->firedTiles();
      std::vector<MuonTileID>::iterator itfired;
     
      // For each fired tile, set to 1 the corresponding tiles in the output register bit set
      for (itfired=firedTilesList.begin(); itfired!= firedTilesList.end(); itfired++){
	pOutRegister->setTile(*itfired);
      }
    }
  }

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

  // Write the BCID (use by PLL to check internal consistency)
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

/**
   Fill the L0Buffer output register with the PLL format.
*/
void L0Muon::L0BufferUnit::setL0bufPLL(){

  if (m_l0bufferFile==NULL) return;
  
  // 
  // Build the list of fired tiles
  //

  // Get the L0Buffer register in input
  TileRegister * pInRegister = dynamic_cast<TileRegister*>(m_inputs["L0bIn"]);
  // Make the pads (cross horizontal and vertical strips)
  pInRegister->makePads();
  // Get the list of fired pads 
  std::vector<MuonTileID> firedPadsList = pInRegister->Pads();
  // Get the list of fired tiles 
  std::vector<MuonTileID> firedTilesList = pInRegister->firedTiles();

    
  // Get a pointer to the L0buffer output register
  TileRegister * pOutRegister = dynamic_cast<TileRegister*>(m_outputs["L0bOut"]);
  // Clear bit set
  pOutRegister->reset();

  //
  // Update the input field of the bitset of the output register (OL + Neighbours) 
  //

  // Loop over the Tiles of the output l0buffer register
  std::vector<MuonTileID> l0bufferTiles =  pOutRegister->getTileVector();
  for (std::vector<MuonTileID>::iterator itl0buffer=l0bufferTiles.begin();
       itl0buffer!=l0bufferTiles.end();
       itl0buffer++){
    
    if (! itl0buffer->isValid()) continue;
    if (itl0buffer->station()>2) {
      // Loop over the fired pads for stations M4 & M5
      for (std::vector<MuonTileID> ::iterator itfired=firedPadsList.begin(); 
	   itfired!= firedPadsList.end(); 
	   itfired++){
	MuonTileID  intercept= itfired->intercept(*itl0buffer);
	if (intercept.isValid()){
	  pOutRegister->setTile(*itl0buffer);
	}
      }
    } else {
      // Loop over the fired tiles for stations M1, M2 & M3
      for (std::vector<MuonTileID> ::iterator itfired=firedTilesList.begin(); 
	   itfired!= firedTilesList.end(); 
	   itfired++){
	MuonTileID  intercept= itfired->intercept(*itl0buffer);

	MuonTileID  container= itl0buffer->containerID(itfired->layout());
	if ( intercept== (*itl0buffer) ){
	  pOutRegister->setTile(*itl0buffer);
	}
      }
    }
    
  }


  //
  // Update the output field of the bitset of the output register (candidates + status + BCID + FOI) 
  //

  // Get the bitset of the output register 
  boost::dynamic_bitset<> outBitSet = pOutRegister->getBitset();

  // Write the output field of the l0buffer (as it was written on the input l0buffer register)
  boost::dynamic_bitset<> inBitSet = pInRegister->getBitset();
  for (boost::dynamic_bitset<>::size_type i =0; i < OUTPUTFIELD_SIZE;i++){
    int val=inBitSet[i] ;
    if (val) {
      outBitSet.set(i);
    }
  }
  
  // Write the FOI in X for M1
  boost::dynamic_bitset<> xM1bitset(4,(unsigned long) xHardFoi(0));
  for (boost::dynamic_bitset<>::size_type i =0; i < xM1bitset.size();i++){
    int val=xM1bitset[i] ;
    if (val) {
      outBitSet.set(i+XM1FOI_OFFSET);
    }
  }

  // Write the FOI in X for M2
  boost::dynamic_bitset<> xM2bitset(4,(unsigned long) xHardFoi(1));
  for (boost::dynamic_bitset<>::size_type i =0; i < xM2bitset.size();i++){
    int val=xM2bitset[i] ;
    if (val) {
      outBitSet.set(i+XM2FOI_OFFSET);
    }
  }

  // Write the FOI in X for M4
  boost::dynamic_bitset<> xM4bitset(4,(unsigned long) xHardFoi(3));
  for (boost::dynamic_bitset<>::size_type i =0; i < xM4bitset.size();i++){
    int val=xM4bitset[i] ;
    if (val) {
      outBitSet.set(i+XM4FOI_OFFSET);
    }
  }

  // Write the FOI in X for M5
  boost::dynamic_bitset<> xM5bitset(4,(unsigned long) xHardFoi(4));
  for (boost::dynamic_bitset<>::size_type i =0; i < xM5bitset.size();i++){
    int val=xM5bitset[i] ;
    if (val) {
      outBitSet.set(i+XM5FOI_OFFSET);
    }
  }

  // Write the FOI in Y for M4
  boost::dynamic_bitset<> yM4bitset(4,(unsigned long) yHardFoi(3));
  for (boost::dynamic_bitset<>::size_type i =0; i < yM4bitset.size();i++){
    int val=yM4bitset[i] ;
    if (val) {
      outBitSet.set(i+YM4FOI_OFFSET);
    }
  }

  // Write the FOI in Y for M5
  boost::dynamic_bitset<> yM5bitset(4,(unsigned long) yHardFoi(4));
  for (boost::dynamic_bitset<>::size_type i =0; i < yM5bitset.size();i++){
    int val=yM5bitset[i] ;
    if (val) {
      outBitSet.set(i+YM5FOI_OFFSET);
    }
  }

  // Write the BCID an other time (use by PLL to check internal consistency)
  boost::dynamic_bitset<> bcidSet(BCID_SIZE,(unsigned long) (m_l0EventNumber%16) );
  for (boost::dynamic_bitset<>::size_type i =0; i < bcidSet.size();i++){
    int val=bcidSet[i] ;
    if (val) {
      outBitSet.set(i+BCIDin_OFFSET);
    }
  }

  // Update the bit set.
  pOutRegister->set(outBitSet);

}

/**
   Fill the OL output register with the PLL format.
*/
void L0Muon::L0BufferUnit::setOLPLL(){

  if (m_l0bufferFile==NULL) return;

  // 
  // Build the list of fired tiles
  //
  std::vector<MuonTileID> firedPadList;

  // Get the OLs in input 
  std::map<std::string,Register*>::iterator ir ;
  for (ir = m_inputs.begin(); ir!= m_inputs.end(); ir++){

    TileRegister * itr = dynamic_cast<TileRegister*>(ir->second);

    if (itr->Type()=="OpticalLink" ) {
      // // // std::cout << "L0BufferUnit::setOLPLL OpticalLink input register found: "<<  ir->first  << std::endl;
      std::vector<MuonTileID> inputFiredTiles = itr->firedTiles();
      for (std::vector<MuonTileID>::iterator itfired=inputFiredTiles.begin(); 
	   itfired!= inputFiredTiles.end(); 
	   itfired++){
	firedPadList.push_back(*itfired);
      }
    }
  }

  
  // // //   std::cout << "L0BufferUnit:setOLPLL firedPadList.size()= "<<  firedPadList.size()  << std::endl;

  //
  // Update the OL output register with the fired tiles
  //
    
  // Get a pointer to the OL output register
  TileRegister * pOutRegister = dynamic_cast<TileRegister*>(m_outputs["OLOut"]);
  // Clear the ouptut register bit set
  pOutRegister->reset();

  // Loop over the fired tiles
  for (std::vector<MuonTileID> ::iterator itfired=firedPadList.begin(); 
       itfired!= firedPadList.end(); 
       itfired++){
    pOutRegister->setTile(*itfired);
  }

  std::vector<MuonTileID> tmpFiredTiles  = pOutRegister->firedTiles();
  std::vector<MuonTileID> tmpTilesVector = pOutRegister->getTileVector();
  // // //   std::cout << "L0BufferUnit:setOLPLL " 
  // // // 	    << "  tmpFiredTiles.size()= "<<  tmpFiredTiles.size()  
  // // // 	    << "  tmpFiredTiles.size()= "<<  tmpTilesVector.size()
  // // // 	    << std::endl;


  //
  // Write the BCID on the 3rd word of the last link
  // 

  // Get the bitset of the output register 
  boost::dynamic_bitset<> outBitSet = pOutRegister->getBitset();

  // Write the BCID (use by PLL to check internal consistency)
  boost::dynamic_bitset<> bcidSet(BCID_SIZE,(unsigned long) (m_l0EventNumber%16));
  for (boost::dynamic_bitset<>::size_type i =0; i < bcidSet.size();i++){
    int val=bcidSet[i] ;
    if (val) {
      outBitSet.set(i+OL_BCID_OFFSET);
    }
  }
  
  // Update the bit set.
  pOutRegister->set(outBitSet);

}

void L0Muon::L0BufferUnit::initialize(){
  Unit* crate = parentByType("CrateUnit");
  if ( crate->getProperty("BuildL0Buffer") == "True") m_buildL0Buffer = true;
}

/**
   Execute event
*/
void L0Muon::L0BufferUnit::execute(){

  if ( ! m_buildL0Buffer ) return;
  
  setL0buf();

  // Write out the L0Buffer
  writeEvent();

  // Increment event counter
  m_l0EventNumber++;
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
  fprintf(m_l0bufferFile,"Q%d R%d%d%d\n",m_pu.quarter()+1,m_pu.region()+1,m_pu.nX(),m_pu.nY());
  fprintf(m_l0bufferFile,"#- FOI \n");
  fprintf(m_l0bufferFile,"%04x %04x\n",xFOI,yFOI);
  fprintf(m_l0bufferFile,"#- Mask \n");
  fprintf(m_l0bufferFile,"%04x %04x %04x\n",0,0xffff,0);

  
}

/**
   Write event to output file
*/
void L0Muon::L0BufferUnit::writeEvent(){

  if (m_l0bufferFile==NULL) return;

  // Write Event Header
  fprintf(m_l0bufferFile,"#- Header\n");
  fprintf(m_l0bufferFile,"%04x %04x\n",m_l0EventNumber%16,0);

  // Write Optical link in INPUT
  fprintf(m_l0bufferFile,"#- Optical links\n");
  TileRegister * pOLOut = dynamic_cast<TileRegister*>(m_outputs["OLOut"]);
  if (pOLOut!=0) pOLOut->print_words(m_l0bufferFile,3);

  // Write L0Buffer
  fprintf(m_l0bufferFile,"#- L0Buffer\n");
  TileRegister * pL0bOut = dynamic_cast<TileRegister*>(m_outputs["L0bOut"]);
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

