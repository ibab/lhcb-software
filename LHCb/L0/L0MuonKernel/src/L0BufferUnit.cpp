#include <iostream>

#include "L0MuonKernel/L0BufferUnit.h"
#include "L0MuonKernel/MuonTriggerUnit.h"


L0Muon::L0BufferUnit::L0BufferUnit() {
}
    
L0Muon::L0BufferUnit::L0BufferUnit(LHCb::MuonTileID id):L0MUnit(id) {
}
    
L0Muon::L0BufferUnit::L0BufferUnit(DOMNode* pNode):L0MUnit(pNode) {  
}
    
L0Muon::L0BufferUnit::~L0BufferUnit() {};  


void L0Muon::L0BufferUnit::initialize() {

  // Initialize the event counter
  m_evtCounter = 0;

  // Contruct the filename root from the type and id of the unit
  char idbuf[4096];
  sprintf(idbuf,"Q%dR%d%d%d",m_mid.quarter()+1,m_mid.region()+1,m_mid.nX(),m_mid.nY());
  std::string root = type() + idbuf;
  // Get the path of the file from the property of the muontrigger unit
  MuonTriggerUnit * pmuontrigger = dynamic_cast<MuonTriggerUnit *>( parentByType("MuonTriggerUnit"));
  std::string path = pmuontrigger->getProperty("L0BufferPath");
  std::string filename = path+"/"+root+".txt";
  // Open output file
  m_outputFile = fopen(filename.c_str(),"w");
  
}

void L0Muon::L0BufferUnit::execute() {
  // Dump buffer into file
  char buf[4096];
  char* format ;
  format = "%s_Q%dR%d%d%d";
  sprintf(buf,format,type().c_str(),m_mid.quarter()+1,m_mid.region()+1,m_mid.nX(),m_mid.nY());
  std::map<std::string,L0Muon::Register*>::iterator itoutputs =  m_outputs.find(buf);
  if (itoutputs==m_outputs.end()) {
    std::cout <<"L0Muon::L0BufferUnit::execute output register not found ; key is "<< buf<<"\n";
    return;
  }
  
  Register* outputreg =(*itoutputs).second;
  outputreg->print_words(m_outputFile);
  fprintf(m_outputFile,"----\n");
  
}

void L0Muon::L0BufferUnit::postexecute() {
  // Increment event counter
  m_evtCounter++;
  // Reset output registers (set bits to 0) 
  releaseOutputRegisters();
  // Dump buffer into file
}



void L0Muon::L0BufferUnit::finalize() {
  // Close file
  fclose(m_outputFile);
  
}




