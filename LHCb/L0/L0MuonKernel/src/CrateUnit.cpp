#include <utility>  // For std::pair
#include <algorithm> 
#include "L0MuonKernel/CrateUnit.h"

#include "L0MuonKernel/MuonTriggerUnit.h"
#include "L0MuonKernel/BCSUnit.h"
#include "L0MuonKernel/Tell1ProcUnit.h"
#include "L0MuonKernel/CtrlUnit.h"
#include "L0MuonKernel/RawBufferProcUnit.h"
#include "L0MuonKernel/L0BufferBCSUnit.h"
#include "L0MuonKernel/L0BufferSlaveUnit.h"
#include "L0MuonKernel/L0BufferCtrlUnit.h"
#include "ProcessorKernel/RegisterFactory.h"
#include "MuonKernel/MuonTileID.h"

L0Muon::CrateUnit::CrateUnit(PL0MProNet & pProNet,
                             std::vector<MuonTileID> config_pus,
                             int rawBufferMode,
                             int l0BufferMode){

  //=============================================
  //=                                           =
  //=   Set configuration dependant variables   =
  //=                                           =
  //=============================================

  MuonTileID mid(0);
  mid.setQuarter(pProNet->quarter());

  m_mid= mid;
  
  //================================================
  //=                                              =
  //=         Build the Units structure            =
  //=                    &                         =
  //=         Build the TileRegisters              =
  //=                                              =
  //= The Units created here are:                  =
  //=                                              =
  //= The units belonging to the CrateUnit:        =
  //=   - BoardUnit (up to 12 per crate)           =
  //=   - CtrlUnit (max 1 per crate)               =
  //=   - Tell1ProcUnit (max 1 per crate)          =
  //=                                              =
  //= The sub-units of the BoardUnit:              =
  //=   - ProcUnit (up to 4 per board)             =
  //=   - BCSUUnit (max 1 per board)               =
  //=                                              =
  //= The sub-units of the BCSUnit:                =
  //=   - L0BufferBCSUnit (max 1 per BCSU)         =
  //=                                              =
  //= The sub-units of the CtrlUnit:               =
  //=   - L0BufferSlaveUnit (max 1 per Ctrl)       =
  //=   - L0BufferCtrlUnit (max 1 per ctrl)        =
  //=                                              =
  //= The sub-units of the Tell1ProcUnit:          =
  //=   - RawBufferProcUnit (max 1 per tell1       =
  //=                                              =
  //================================================

  char buf[4096];
  char buf_alias[4096];
  char* format ;

  // Clear the units tree
  if ( ! m_units.empty() ) {
    m_units.clear();
  }

  // Get a pointer to the register factory
  L0Muon::RegisterFactory* rfactory = L0Muon::RegisterFactory::instance();

  // Build the Tell1 unit
  Tell1ProcUnit *tell1proc = new Tell1ProcUnit(m_mid);

  // Build the Raw buffer unit of the Tell1 linked to the Proc boards  
  RawBufferProcUnit *rawbufproc = new RawBufferProcUnit(m_mid);
  tell1proc->addUnit(rawbufproc);

  // Build the Crl unit and its l0buffer units
  CtrlUnit *ctrl = new CtrlUnit(m_mid);
  L0BufferSlaveUnit *l0bufferctrl  = new L0BufferSlaveUnit(m_mid);
  L0BufferCtrlUnit *l0bufferslave = new L0BufferCtrlUnit(m_mid);
  if (l0BufferMode==1) ctrl->addUnit(l0bufferctrl);
  if (l0BufferMode==1) ctrl->addUnit(l0bufferslave);
  
  //
  // Loop over the boards in the processor
  // -------------------------------------
  //
  bool ok;
  int boardIndex=0;
  L0MTileList boards = pProNet->boards();
  L0MTileList::iterator ib;
  for (ib= boards.begin(); ib != boards.end(); ib++){
    boardIndex++;

    // Check that the board contains at least one of the requested PU's if
    // the list of PU's is given explicitely
    ok = true;
    if (config_pus.size() > 0) {
      ok = false;
      std::vector<MuonTileID>::iterator ipu;
      for ( ipu = config_pus.begin(); ipu != config_pus.end(); ipu++ ) {
        MuonTileID good_board = ipu->containerID(MuonLayout(1,1));
        if ( (*ib) == good_board ) {
          ok = true;
          break;
        }
      }
    } else {
      ok = true;
    } 
    if ( ! ok ) continue; 

    //
    // Build the Board Unit and it's sub units
    //
    BoardUnit * board = new BoardUnit(*ib);
    addUnit(board);

    BCSUnit * bcsu = new BCSUnit(*ib);
    L0BufferBCSUnit * l0bufferbcsu = new L0BufferBCSUnit(*ib);
    if (l0BufferMode==1) bcsu->addUnit(l0bufferbcsu);
    
    //
    // Loop over the PUs in the board
    // ------------------------------
    //
    L0MTileList pusb = pProNet->pusInBoard(*ib);
    L0MTileList::iterator ipusb;
    for ( ipusb = pusb.begin(); ipusb != pusb.end(); ipusb++ ){
            
      // Check that this PU was requested
      ok = true;
      if (config_pus.size() > 0) {
        ok = false;
        std::vector<MuonTileID>::iterator ipu = std::find(config_pus.begin(),
                                                          config_pus.end(),
                                                          (*ipusb));
        if ( ipu != config_pus.end() ) {
          ok = true;
        }
      } else {
        ok = true;
      }	 
	       
      // If we want this PU
      if ( ok ) {
        //
        // Build the ProcUnit 
        //
        L0MPuNodeBase puNode = pProNet->puNode(*ipusb);
        Unit * pProcUnit = new ProcUnit(puNode,rawBufferMode,l0BufferMode);
        board->addUnit(pProcUnit);
        
        // Add the Candidate registers of the PU
        // in the ouput registers list of the BCSU and Tell1
        // (the registers have been created in ProcUnit)
        format = "CAND_PU%s";
        sprintf(buf,format, puNode.name().c_str());
        Register* candreg  = rfactory->searchRegister(buf);
        format = "PU%d";
        sprintf(buf,format,puNode.hardID());
        bcsu->addInputRegister(candreg,buf);
        l0bufferbcsu->addInputRegister(candreg,buf);
        rawbufproc->addInputRegister(candreg,buf);

        // Add the input tile registers of the PU
        // in the input registers list of the RawBufferProc
        // (the registers have been created in ProcUnit)
        format = "RAWDATAIN%s_%d";
        int index=0;
        while (1) {
          sprintf(buf,format, puNode.name().c_str(),index);
          TileRegister* tilereg  = rfactory->searchTileRegister(buf);
          if (tilereg==0) break;
          index++;
          if (rawBufferMode>0) rawbufproc->addInputRegister(tilereg);
         }
        
        if (l0BufferMode!=0 || rawBufferMode!=0) {
          // Add the formatted input tile registers of the PU
          // in the input registers list of the RawBufferProc
          // (the registers have been created in ProcUnit)
          format = "FORMATTEDDATAIN_%s";
          sprintf(buf,format, puNode.name().c_str());
          TileRegister* tilereg  = rfactory->searchTileRegister(buf);
          rawbufproc->addInputRegister(tilereg);
        }
        
      }	// End If we want this PU
    } // End of Loop over the PUs in the board
    
    // BCSU candidates register
    format = "CAND_BCSUQ%dR%d%d%d";
    sprintf(buf,format, m_mid.quarter()+1,(*ib).region()+1,(*ib).nX(),(*ib).nY());
    Register* candreg  = rfactory->createRegister(buf,L0Muon::BitsCandRegTot);
    // Alias use for the raw buffer of the TELL1 attached to the controler cards
    format = "CAND_BOARDQ%d_%d";               
    sprintf(buf_alias,format,(*ib).quarter()+1,boardIndex-1);
    rfactory->createAlias(buf,buf_alias);

    candreg->setType("Candidates");
    bcsu->addOutputRegister(candreg);
    l0bufferbcsu->addInputRegister(candreg);
    format = "BOARD%d";
    sprintf(buf,format,boardIndex-1);
    ctrl->addInputRegister(candreg,buf);
    l0bufferctrl->addInputRegister(candreg,buf);
    l0bufferslave->addInputRegister(candreg,buf);
    rawbufproc->addInputRegister(candreg,buf);
    
    // Add sub-units to the board 
    board->addUnit(bcsu);

  }  // End of Loop over the boards in the processor

  // Controller candidates register
  format = "CAND_CTRLQ%d";
  sprintf(buf,format, m_mid.quarter()+1);
  Register* candreg  = rfactory->createRegister(buf,L0Muon::BitsCandRegTot);
  ctrl->addOutputRegister(candreg);
  l0bufferctrl->addInputRegister(candreg);
  l0bufferslave->addInputRegister(candreg);

  // Rawbuffer Proc output register
  format = "RAWBUFFER_PROCQ%d";
  sprintf(buf,format, m_mid.quarter()+1);
  Register* rawreg  = rfactory->createRegister(buf,L0Muon::BitsRawBufProcTot);
  rawbufproc->addOutputRegister(rawreg);

  // Add sub units to the crate 
  addUnit(ctrl);
  addUnit(tell1proc);

  
}

L0Muon::CrateUnit::CrateUnit(DOMNode* pNode):L0MUnit(pNode){
}

L0Muon::CrateUnit::~CrateUnit(){
}

// // void L0Muon::CrateUnit::initialize() {
  
// //   MuonTriggerUnit * parent = dynamic_cast<MuonTriggerUnit *>( parentByType("MuonTriggerUnit"));

// //   // m_ignoreM1     =  m_properties["ignoreM1"];
// //   // m_ptparameters =  m_properties["ptparameters"]; 
// //   // m_xfoi         =  m_properties["foiXSize"]; 
// //   // m_yfoi         =  m_properties["foiYSize"]; 
// //   m_ignoreM1     =  parent->getProperty("ignoreM1");
// //   m_ptparameters =  parent->getProperty("ptparameters"); 
// //   m_xfoi         =  parent->getProperty("foiXSize"); 
// //   m_yfoi         =  parent->getProperty("foiYSize"); 
 
// //   L0Muon::Unit::initialize();
// // }


// // int L0Muon::CrateUnit::xFoi(int sta)
// // {
// //   int xfoi= m_xfoi[sta];
// //   return xfoi;
  
// // }

// // int L0Muon::CrateUnit::yFoi(int sta)
// // {
// //   int yfoi= m_yfoi[sta];
// //   return yfoi;
  
// // }
