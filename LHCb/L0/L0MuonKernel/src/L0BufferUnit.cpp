#include "L0MuonKernel/L0BufferUnit.h"
#include "L0MuonKernel/L0mProcUnit.h"
#include "L0mConf/L0MBase.h" 
#include <cmath>
#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>
#include <stdio.h>

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
#define  L0BUFFER_SIZE    32*16
#define  OL_SIZE          18*16  

#define  BCIDin_OFFSET    6*16+0
#define  OL_BCID_OFFSET   17*16+0
#define  BCID_SIZE        4
/*
  Constructor.
*/
L0Muon::L0BufferUnit::L0BufferUnit(MuonTileID & pu,
				   std::vector<int> & foix,
				   std::vector<int> & foiy,
				   int & type){
   
  m_pu = pu;
  m_xfoi= foix;
  m_yfoi= foiy;   
  m_type = type;

  m_l0bufferFile = NULL;
  m_l0EventNumber=0;

};

/*
  Open the Output file where to write the L0Buffers, and write the header.
*/
void L0Muon::L0BufferUnit::setOutputFile(std::string l0buffertype){

  // Open the output file 

  //char * name = "/lhcb/users/cogan/public/TestBench/L0Buffers/l0buf_PUQ%dR%d%d%d_%s.txt";
  char * name = "/marmuon3/NewInputFiles/l0buf_PUQ%dR%d%d%d_%s.txt";
  char buf[4096];
  sprintf(buf,name,m_pu.quarter()+1,m_pu.region()+1,m_pu.nX(),m_pu.nY(),l0buffertype.c_str());
  m_l0bufferFile = fopen(buf,"w");

  writeHeader();
}

/*
  Destructor
*/
L0Muon::L0BufferUnit::~L0BufferUnit(){
  if (m_l0bufferFile!=NULL) fclose(m_l0bufferFile);
}

/*
  Fill the L0Buffer output register.
*/
void L0Muon::L0BufferUnit::setL0buf(){

  if (m_type == 0) {
    setL0bufStd();
  } else {
    setOLPLL();
    setL0bufPLL();
  }
}


/*
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
  std::map<std::string,Register*>::iterator ir ;
  for (ir = m_inputs.begin(); ir!= m_inputs.end(); ir++){
    
    TileRegister * itr = dynamic_cast<TileRegister*>(ir->second);

    // Filter the input register according to their type
    if (itr->Type()=="InputfieldOL" || itr->Type()=="InputfieldNeigh") {

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

/*
  Fill the L0Buffer output register with the PLL format.
*/
void L0Muon::L0BufferUnit::setL0bufPLL(){

  
  // 
  // Build the list of fired tiles
  //

  // Get the L0Buffer register in input
  TileRegister * pInRegister = dynamic_cast<TileRegister*>(m_inputs["L0bIn"]);
  // Make the pads (cross horizontal and vertical strips)
  pInRegister->makePads();
  // Get the list of fired pads 
  std::vector<MuonTileID> firedPadList = pInRegister->Pads();

    
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
    
    // Loop over the fired pads
    for (std::vector<MuonTileID> ::iterator itfired=firedPadList.begin(); 
	 itfired!= firedPadList.end(); 
	 itfired++){
      MuonTileID  intercept= itfired->intercept(*itl0buffer);
      if (intercept.isValid()){
	pOutRegister->setTile(*itl0buffer);
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
  boost::dynamic_bitset<> xM1bitset(4,(unsigned long) xFoi(0));
  for (boost::dynamic_bitset<>::size_type i =0; i < xM1bitset.size();i++){
    int val=xM1bitset[i] ;
    if (val) {
      outBitSet.set(i+XM1FOI_OFFSET);
    }
  }

  // Write the FOI in X for M2
  boost::dynamic_bitset<> xM2bitset(4,(unsigned long) xFoi(1));
  for (boost::dynamic_bitset<>::size_type i =0; i < xM2bitset.size();i++){
    int val=xM2bitset[i] ;
    if (val) {
      outBitSet.set(i+XM2FOI_OFFSET);
    }
  }

  // Write the FOI in X for M4
  boost::dynamic_bitset<> xM4bitset(4,(unsigned long) xFoi(3));
  for (boost::dynamic_bitset<>::size_type i =0; i < xM4bitset.size();i++){
    int val=xM4bitset[i] ;
    if (val) {
      outBitSet.set(i+XM4FOI_OFFSET);
    }
  }

  // Write the FOI in X for M5
  boost::dynamic_bitset<> xM5bitset(4,(unsigned long) xFoi(4));
  for (boost::dynamic_bitset<>::size_type i =0; i < xM5bitset.size();i++){
    int val=xM5bitset[i] ;
    if (val) {
      outBitSet.set(i+XM5FOI_OFFSET);
    }
  }

  // Write the FOI in Y for M4
  boost::dynamic_bitset<> yM4bitset(4,(unsigned long) yFoi(3));
  for (boost::dynamic_bitset<>::size_type i =0; i < yM4bitset.size();i++){
    int val=yM4bitset[i] ;
    if (val) {
      outBitSet.set(i+YM4FOI_OFFSET);
    }
  }

  // Write the FOI in Y for M5
  boost::dynamic_bitset<> yM5bitset(4,(unsigned long) yFoi(4));
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

/*
  Fill the OL output register with the PLL format.
*/
void L0Muon::L0BufferUnit::setOLPLL(){

  

  // 
  // Build the list of fired tiles
  //
  std::vector<MuonTileID> firedPadList;

  // Get the OLs in input 
  std::map<std::string,Register*>::iterator ir ;
  for (ir = m_inputs.begin(); ir!= m_inputs.end(); ir++){

    TileRegister * itr = dynamic_cast<TileRegister*>(ir->second);

    if (itr->Type()=="InputfieldOL" ) {
      std::vector<MuonTileID> inputFiredTiles = itr->firedTiles();
      for (std::vector<MuonTileID>::iterator itfired=inputFiredTiles.begin(); 
	   itfired!= inputFiredTiles.end(); 
	   itfired++){
	firedPadList.push_back(*itfired);
      }
    }
  }

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


/*
  Read the external file describing the buffer and return the L0MTileList
*/
std::vector<L0MTile> L0Muon::L0BufferUnit::tileListFromMap(int type){

  std::vector<L0MTile> ltiles;

  // Open the map file describing the l0buffer
  char name[4096];
  if (type == 0) {
    char * format = "/lhcb/users/cogan/public/TestBench/L0BufferMaps/map_l0buffer_PLL_Q%dR%d%d%d.txt";
    sprintf(name,format,m_pu.quarter()+1,m_pu.region()+1,m_pu.nX(),m_pu.nY());
  } else  if (type == 1){
    char * format = "/lhcb/users/cogan/public/TestBench/L0BufferMaps/map_OL_PLL_Q%dR%d%d%d.txt";
    sprintf(name,format,m_pu.quarter()+1,m_pu.region()+1,m_pu.nX(),m_pu.nY());
  } 
  FILE * mapfile = fopen(name,"r");
  if (mapfile==NULL) return ltiles;

  // Build the list of L0MTile
  // Fix the size of the list
  int size;
  if (type == 0){
    size = L0BUFFER_SIZE;
  } else if (type == 1) {
    size = OL_SIZE;
  }

  ltiles = readTileListFromMap(mapfile,size);

  fclose(mapfile);

  return ltiles;

}
/*
  Generic method to extract a tile list from the map
*/
std::vector<L0MTile> L0Muon::L0BufferUnit::readTileListFromMap(FILE * mapfile,int max){

  std::vector<L0MTile> ltiles(max);
  while (1)
  {
    // Read line
    int ibit;
    int Q;
    int R; 
    int sta;
    char type[8];
    int gridX;
    int gridY;
    int nX;
    int nY;


    if (fscanf(mapfile,"%d %d %d %d %s %d %d %d %d",&ibit,&Q,&R,&sta,type,&gridX,&gridY,&nX,&nY)==EOF) break;

    L0MBase::L0MTileType tag; 
    if      (type == "Pad")     { tag = L0MBase::Pad; }
    else if (type == "HS")      { tag = L0MBase::StripH; }
    else if (type == "VS")      { tag = L0MBase::StripV; }
    else { tag = L0MBase::Unknown; }
    
    // Build the L0MTile
     
    MuonLayout layout(gridX,gridY);
    MuonTileID mid(sta,0,0,layout,R,Q,nX,nY);
    L0MTile tile(mid,tag);
    ltiles[ibit]=tile;
  }

  return ltiles;
  
}


void L0Muon::L0BufferUnit::initialize(){
  if (m_debug) std::cout << "Initialize L0Buffer" << std::endl;  
}

void L0Muon::L0BufferUnit::execute(){
  

  setL0buf();

  if ( ! m_units.empty() ) {
    std::map<std::string,L0Muon::Unit*>::iterator iu;
    for ( iu = m_units.begin(); iu != m_units.end(); iu++ ) {
      if (m_debug) std::cout << "L0BufferUnit::execute " << "subUnit "<< (*iu).first << std::endl;
      (*iu).second->execute();
    }
  }

  // Write out the L0Buffer
  writeEvent();

  // Increment event counter
  m_l0EventNumber++;
}

void L0Muon::L0BufferUnit::finalize(){
}

/*
  Write output file header 
*/
void L0Muon::L0BufferUnit::writeHeader(){

  if (m_l0bufferFile==NULL) return;

  int xFOI=0;
  int yFOI=0;

  int xM1 = xFoi(0);
  int xM2 = xFoi(1);
  int xM4 = xFoi(3);
  int xM5 = xFoi(4);

  int yM1 = yFoi(0);
  int yM2 = yFoi(1);
  int yM4 = yFoi(3);
  int yM5 = yFoi(4);

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
  fprintf(m_l0bufferFile,"#- FOI (to be implemented)\n");
  fprintf(m_l0bufferFile,"%04x %04x\n",xFOI,yFOI);
  fprintf(m_l0bufferFile,"#- Mask (to be implemented)\n");
  fprintf(m_l0bufferFile,"%04x %04x %04x\n",0,0,0);

  
}

/*
  Write event to output file
*/
void L0Muon::L0BufferUnit::writeEvent(){

  if (m_l0bufferFile==NULL) return;


  // Write Event Header
  fprintf(m_l0bufferFile,"#- Header\n");
  fprintf(m_l0bufferFile,"%04x %04x\n",m_l0EventNumber,0);
  
  std::map<std::string,Register*>::iterator ir ;

 
  // Write Optical link in INPUT
  fprintf(m_l0bufferFile,"#- Optical links\n");
  for (ir = m_outputs.begin(); ir!= m_outputs.end(); ir++){
    TileRegister * itr = dynamic_cast<TileRegister*>(ir->second);
     if (itr->Type()=="OpticalLink" ) {
       itr->print_words(m_l0bufferFile);
     }
  }

  // Write L0Buffer
  fprintf(m_l0bufferFile,"#- L0Buffer\n");
  for (ir = m_outputs.begin(); ir!= m_outputs.end(); ir++){
    TileRegister * itr = dynamic_cast<TileRegister*>(ir->second);
     if (itr->Type()=="L0Buffer" ) {
       itr->print_words(m_l0bufferFile);
     }
  }

  fprintf(m_l0bufferFile,"#----\n");

  
}

int L0Muon::L0BufferUnit::xFoi(int sta)
{
  int xfoi= m_xfoi[sta];
  return xfoi;
  
}

int L0Muon::L0BufferUnit::yFoi(int sta)
{
  int yfoi= m_yfoi[sta];
  return yfoi;
  
}

