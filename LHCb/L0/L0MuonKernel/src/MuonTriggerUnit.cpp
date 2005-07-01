#include "L0MuonKernel/MuonTriggerUnit.h"
#include "L0MuonKernel/CrateUnit.h"
#include "L0mConf/L0MProNetBase.h"
#include "L0mConf/L0MXmlReader.h"

L0Muon::MuonTriggerUnit::MuonTriggerUnit(){

}

L0Muon::MuonTriggerUnit::MuonTriggerUnit(std::vector< std::string >  config_files, std::vector<MuonTileID> config_pus){
  L0MXmlReader * xmlRdr =new L0MXmlReader();
  for (std::vector<std::string>::iterator ifile= config_files.begin();
        ifile!= config_files.end(); ifile++){
      
    PL0MProNet pProNet = xmlRdr->proNetFromFile(*ifile);   
      
    L0Muon::CrateUnit * pcrate = new L0Muon::CrateUnit(pProNet,config_pus);
    addUnit(pcrate);
  }  
  // Some final cleaning
  delete xmlRdr;  
}

L0Muon::MuonTriggerUnit::MuonTriggerUnit(DOMNode* pNode):L0MUnit(pNode){
}



L0Muon::MuonTriggerUnit::~MuonTriggerUnit() {}

