//#include <strstream>
#include "L0MuonKernel/Unit.h"


L0Muon::Unit::Unit() {

  m_parent =0;
  
};
    
L0Muon::Unit::~Unit() {};  

void L0Muon::Unit::setParent(L0Muon::Unit * unit)
{
  m_parent = unit;
}



void L0Muon::Unit::addInputRegister(L0Muon::Register* in, std::string rname) {

  m_inputs[rname] = in ;
}

void L0Muon::Unit::addInputRegister(L0Muon::Register* in) {
  std::string nm = in->name();
  m_inputs[nm] = in ;
}

void L0Muon::Unit::addOutputRegister(L0Muon::Register* in, std::string rname) {

  m_outputs[rname] = in ;
}

void L0Muon::Unit::addOutputRegister(L0Muon::Register* out) {
  std::string nm = out->name();
  m_outputs[nm] = out ;
}

void L0Muon::Unit::addUnit(L0Muon::Unit* punit, std::string uname) {

  m_units[uname] = punit ;
}

void L0Muon::Unit::addUnit(L0Muon::Unit* punit) {
  int nreg = m_units.size();
  char  strbuf[256];
  sprintf(strbuf,"%d",nreg);
  std::string iss = strbuf;
  m_units[iss] = punit ;
}

void L0Muon::Unit::releaseRegisters() {

  std::map<std::string,L0Muon::Register*>::iterator ir;
  if ( ! m_inputs.empty()){
    
  for ( ir = m_inputs.begin(); ir != m_inputs.end(); ir++) {

    L0Muon::Register* reg = (*ir).second;
    reg->reset();    
  }
  }
  if ( ! m_outputs.empty()){
  for ( ir = m_outputs.begin(); ir != m_outputs.end(); ir++) {
  L0Muon::Register* reg = (*ir).second;
    reg->reset();    
  }
  }
  
}  

void L0Muon::Unit::dumpRegisters(MsgStream * log) {

  std::map<std::string,L0Muon::Register*>::iterator ir;
  
  
  int ic = 1;
  for ( ir = m_inputs.begin(); ir != m_inputs.end(); ir++) {
   
    boost::dynamic_bitset<> bits = (*ir).second->getBitset();
    for (boost::dynamic_bitset<>::size_type i =0; i < bits.size();i++){
     
    }
    //*log << MSG::DEBUG << endreq;
    ic++;
  }
 
  ic = 1;
  for ( ir = m_outputs.begin(); ir != m_outputs.end(); ir++) {
   
    boost::dynamic_bitset<> bits = (*ir).second->getBitset();
    for (boost::dynamic_bitset<>::size_type i =0; i < bits.size();i++){
     
    }
  
    ic++; 
  }
}


void L0Muon::Unit::execute() {
  // execute the subunits
  if ( ! m_units.empty() ) {
    std::map<std::string,L0Muon::Unit*>::iterator iu;
    for ( iu = m_units.begin(); iu != m_units.end(); iu++ ) {
      (*iu).second->execute();

    }
  }  

}

void L0Muon::Unit::initialize() {

  // initialize the subunits
  if ( ! m_units.empty() ) {
    std::map<std::string,L0Muon::Unit*>::iterator iu;
    for ( iu = m_units.begin(); iu != m_units.end(); iu++ ) {
      (*iu).second->initialize();
    }
  } 
}

void L0Muon::Unit::finalize() {
  
  // finalize the subunits
  if ( ! m_units.empty() ) {
    std::map<std::string,L0Muon::Unit*>::iterator iu;
    for ( iu = m_units.begin(); iu != m_units.end(); iu++ ) {
      (*iu).second->finalize();
    }
  } 
}
