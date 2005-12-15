#include "L0MuonKernel/MuonTriggerUnit.h"
#include "L0MuonKernel/CrateUnit.h"
#include "L0MuonKernel/Tell1CtrlUnit.h"
#include "L0MuonKernel/RawBufferCtrlUnit.h"
#include "L0mConf/L0MProNetBase.h"
#include "L0mConf/L0MDBBase.h"
#include "L0mConf/L0MProNetFactory.h"
#include "ProcessorKernel/RegisterFactory.h"
#include "ProcessorKernel/Property.h"
#include "MuonKernel/MuonTileID.h"
#include "MuonKernel/MuonLayout.h"

L0Muon::MuonTriggerUnit::MuonTriggerUnit(){

}

L0Muon::MuonTriggerUnit::MuonTriggerUnit(std::vector< std::string >  config_files, 
                                         std::vector<MuonTileID> config_pus,
                                         int rawBufferMode,
                                         int l0BufferMode){

  //================================================
  //=                                              =
  //=         Build the Units structure            =
  //=                                              =
  //= The Units created here are:                  =
  //=                                              =
  //= The units belonging to the MuonTriggerUnit:  =
  //=   - CrateUnit (up to 4)                      =
  //=   - Tell1CtrlUnit (1)                        =
  //=                                              =
  //= The sub-units of the Tell1CtrlUnit:          =
  //=   -RawBufferCtrlUnit                         =
  //=                                              =
  //=                                              =
  //================================================

  // Instanciate a database object
  L0MDBBase db= L0MDBBase();
  
  // Loop over the config files; Each one contains a database for a quarter.
  std::vector<int> vquarter;
  for (std::vector<std::string>::iterator ifile= config_files.begin();
        ifile!= config_files.end(); ifile++){

    // Connect to the data base in the config file
    db.connect((*ifile).c_str());

    // Instanciate a processor net factory
    L0MProNetFactory proNetFactory = L0MProNetFactory( &db);

    // Get the processor net in the database
    PL0MProNet pProNet = proNetFactory.createProNet();

    // Build the crate unit
    L0Muon::CrateUnit * pcrate = new L0Muon::CrateUnit(pProNet,config_pus,rawBufferMode,l0BufferMode);
    addUnit(pcrate);

    // Update the list of quarter used
    vquarter.push_back(pProNet->quarter());

  }  // End of Loop over the config files;

  // Close the database object
  db.close();

  // Build the Tell1 unit
  Tell1CtrlUnit *tell1ctrl = new Tell1CtrlUnit(m_mid);
  addUnit(tell1ctrl);

  // Build the raw buffer unit of the Tell1 linked to the controller board 
  RawBufferCtrlUnit *rawbufctrl = new RawBufferCtrlUnit(m_mid);
  tell1ctrl->addUnit(rawbufctrl);

  //=============================================
  //=                                           =
  //=         Build the TileRegisters           =
  //=                                           =
  //=============================================

  char buf[4096];
  char* format ;

  // Get a pointer to the register factory
  L0Muon::RegisterFactory* rfactory = L0Muon::RegisterFactory::instance();

  // Loop over the quarters.
  for (std::vector<int>::iterator itquarter=vquarter.begin(); itquarter<vquarter.end(); itquarter++){

    // Add the candidate registers of the ctrl board
    format = "CAND_CTRLQ%d";
    sprintf(buf,format, (*itquarter)+1);
    Register* candreg  = rfactory->searchRegister(buf);
    rawbufctrl->addInputRegister(candreg);

    // Add the candidate registers of the bcsu
    for (int iboard=0; iboard<12; iboard++){
      format = "CAND_BOARDQ%d_%d";               
      sprintf(buf,format,(*itquarter)+1,iboard);
      Register* candreg  = rfactory->searchRegister(buf);
      if (candreg==0) continue;
      if (rawBufferMode>1) rawbufctrl->addInputRegister(candreg,buf);
    }
    
  }// End of Loop over the quarters.

  // Rawbuffer Ctrl output register
  format = "RAWBUFFER_CTRL";
  sprintf(buf,format);
  Register* rawreg  = rfactory->createRegister(buf,L0Muon::BitsRawBufCtrlTot);
  rawbufctrl->addOutputRegister(rawreg);

}

L0Muon::MuonTriggerUnit::MuonTriggerUnit(DOMNode* pNode):L0MUnit(pNode){
}



L0Muon::MuonTriggerUnit::~MuonTriggerUnit() {}

void L0Muon::MuonTriggerUnit::initialize() {

  m_ignoreM1     =  m_properties["ignoreM1"];
  m_ptparameters =  m_properties["ptParameters"]; 
  m_xfoi         =  m_properties["foiXSize"]; 
  m_yfoi         =  m_properties["foiYSize"];

  L0Muon::Unit::initialize();
}


int L0Muon::MuonTriggerUnit::xFoi(int sta)
{
  int xfoi= m_xfoi[sta];
  return xfoi;  
}

int L0Muon::MuonTriggerUnit::yFoi(int sta)
{
  int yfoi= m_yfoi[sta];
  return yfoi;
}
