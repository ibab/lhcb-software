#include <iostream>
#include "ProcessorKernel/Unit.h"
#include "ProcessorKernel/RegisterFactory.h"


L0Muon::Unit::Unit() {

  m_parent =0;
  m_debug = false;
  //   m_name = "Unknown";  

}

    
L0Muon::Unit::~Unit() {} 

void L0Muon::Unit::setParent(L0Muon::Unit * unit)
{
  m_parent = unit;
}

// L0Muon::Unit * L0Muon::Unit::subUnit(std::string type)
// {
//   std::vector<L0Muon::Unit*>::iterator iu;
//   for (iu=m_units.begin();iu!=m_units.end();iu++){
//     if( (*iu)->type()==type) return (*iu);
//   }
//   return 0;
  
// }


L0Muon::Property L0Muon::Unit::getProperty(std::string name) {
  std::map<std::string,L0Muon::Property>::iterator im;
  im = m_properties.find(name);
  if (im!=m_properties.end()) {
    return (*im).second;
  } else {
    return L0Muon::Property("Unknown");
  }
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


void L0Muon::Unit::addUnit(L0Muon::Unit* punit) {
  if (punit->parent()!=0) {
    std::cout <<"** ERROR: Unit :"<<type()<<std::endl;
    std::cout <<"** child unit"<<punit->type()<<"already has a parent"<<std::endl;
    std::cout <<"** parent points to= "<<punit->parent()<<" ("<<punit->parent()->type() <<")"<< std::endl;
    exit(-1);
  }
  punit->setParent(this);
  m_units.push_back(punit);
}

void L0Muon::Unit::releaseRegisters() {
  releaseInputRegisters();
  releaseOutputRegisters();
  
}
void L0Muon::Unit::releaseInputRegisters() {

  if ( ! m_inputs.empty()){    
    std::map<std::string,L0Muon::Register*>::iterator ir;
    for ( ir = m_inputs.begin(); ir != m_inputs.end(); ir++) {
      L0Muon::Register* reg = (*ir).second;
      reg->reset();    
    }
  }
  
}  
void L0Muon::Unit::releaseOutputRegisters() {

  if ( ! m_outputs.empty()){
    std::map<std::string,L0Muon::Register*>::iterator ir;
    for ( ir = m_outputs.begin(); ir != m_outputs.end(); ir++) {
      L0Muon::Register* reg = (*ir).second;
      reg->reset();    
    }
  }
  
}  

void L0Muon::Unit::dumpRegisters() {

  std::map<std::string,L0Muon::Register*>::iterator ir;
  
  
  int ic = 1;
  std::cout << "Input Registers" << std::endl;
  for ( ir = m_inputs.begin(); ir != m_inputs.end(); ir++) {
    boost::dynamic_bitset<> bits = (*ir).second->getBitset();
    for (boost::dynamic_bitset<>::size_type i =0; i < bits.size();i++){
      std::cout << bits[i] << std::endl;
    }
    std::cout << std::endl;
    ic++;
  }
 
  ic = 1;
  std::cout << "Output Registers" << std::endl;
  for ( ir = m_outputs.begin(); ir != m_outputs.end(); ir++) {
   
    boost::dynamic_bitset<> bits = (*ir).second->getBitset();
    for (boost::dynamic_bitset<>::size_type i =0; i < bits.size();i++){
      std::cout << bits[i] << std::endl;    
    }
    std::cout << std::endl; 
    ic++; 
  }
}

void L0Muon::Unit::setDebugMode(bool debug) {

  m_debug = debug;
  if (m_debug) std::cout <<"*** "<< type() <<"::setDebugMode " << debug<<std::endl;
  // set debug level for all the subunits
  if ( ! m_units.empty() ) {
    std::vector<L0Muon::Unit*>::iterator  iu;
    for ( iu = m_units.begin(); iu != m_units.end(); iu++ ) {
      (*iu)->setDebugMode(debug);
    }
  } 
}

void L0Muon::Unit::setProperties(std::map<std::string,L0Muon::Property> properties) {
  //   std::cout <<"Unit::setProperties IN"<<std::endl;
  m_properties.clear();
  //   std::cout <<"Unit::setProperties  m_properties cleared "<<std::endl;
  //   std::cout <<"Unit::setProperties  m_properties.size()= "<< m_properties.size() <<std::endl;
  std::map<std::string,L0Muon::Property>::iterator iproperties;
  for (iproperties=properties.begin(); iproperties!=properties.end(); iproperties++){
    //     std::cout <<"Unit::setProperties (loop)  "<<iproperties->first<<" "<<iproperties->second<<std::endl;
    m_properties[iproperties->first]=iproperties->second;
  }
  // m_properties = properties;
  //   std::cout <<"Unit::setProperties OUT"<<std::endl;
}

void L0Muon::Unit::initialize() {

  if (m_debug) std::cout <<"*** "<< type() <<"::initialize" << std::endl;
  // initialize the subunits
  if ( ! m_units.empty() ) {
    std::vector<L0Muon::Unit*>::iterator  iu;
    for ( iu = m_units.begin(); iu != m_units.end(); iu++ ) {
      (*iu)->initialize();
    }
  } 
}

void L0Muon::Unit::preexecute() {

  if (m_debug) std::cout <<"*** "<< type() <<"::preexecute" << std::endl;
  // preexecute the subunits
  if ( ! m_units.empty() ) {
    std::vector<L0Muon::Unit*>::iterator  iu;
    for ( iu = m_units.begin(); iu != m_units.end(); iu++ ) {
      (*iu)->preexecute();
    }
  } 
}

void L0Muon::Unit::execute() {
  if (m_debug) std::cout <<"*** "<< type() <<"::execute" << std::endl;
  // execute the subunits
  if ( ! m_units.empty() ) {
    std::vector<L0Muon::Unit*>::iterator  iu;
    for ( iu = m_units.begin(); iu != m_units.end(); iu++ ) {
      (*iu)->execute();

    }
  }  
}

  
void L0Muon::Unit::postexecute() {
  if (m_debug) std::cout <<"*** "<< type() <<"::postexecute" << std::endl;
  // postexecute the subunits
  if ( ! m_units.empty() ) {
    std::vector<L0Muon::Unit*>::iterator  iu;
    for ( iu = m_units.begin(); iu != m_units.end(); iu++ ) {
      (*iu)->postexecute();
    }
  } 
}

void L0Muon::Unit::finalize() {
  if (m_debug) std::cout <<"*** "<< type() <<"::finalize" << std::endl;
  // finalize the subunits
  if ( ! m_units.empty() ) {
    std::vector<L0Muon::Unit*>::iterator  iu;
    for ( iu = m_units.begin(); iu != m_units.end(); iu++ ) {
      (*iu)->finalize();
    }
  } 
}

L0Muon::Unit* L0Muon::Unit::parentByType(std::string utype) {

  L0Muon::Unit* uparent = parent();
  if (uparent) {
    //if (m_debug) std::cout << "parentByType: <"<<uparent->type() << ">\n";
    if ( uparent->type() != utype ) {
      uparent = uparent->parentByType(utype);
    }
  }    
  return uparent;
}


void L0Muon::Unit::dump(int offset) {

  std::string blanc(offset,' ');

  std::cout << blanc << "+--------- Unit -----------" << std::endl;
  //   std::cout << blanc << "| Unit name '" << m_name << "' of type " << type() << std::endl;
  std::cout << blanc << "| Unit type " << type() << std::endl;
  if (m_parent) {
    std::cout << blanc << "| Parent type " << m_parent->type() << std::endl;
    //     std::cout << blanc << "| Parent name '" << m_parent->name() << "' of type " 
    //               << m_parent->type() << std::endl;
  } else {
    std::cout << blanc << "| No Parent" << std::endl;
  }
  
  if (m_units.size() > 0) {
    std::cout << blanc << "| Subunits:" << std::endl;  
    std::vector<L0Muon::Unit*>::iterator  isu;
    for ( isu = m_units.begin(); isu != m_units.end(); isu++) {
      std::cout << blanc << "| type " << (*isu)->type() << std::endl;
    }  
  }
  std::cout << blanc << "+--------------------------" << std::endl;    
}

void L0Muon::Unit::dumpUnitTree(int offset) {

  dump(offset);

  if ( ! m_units.empty() ) {
    std::vector<L0Muon::Unit*>::iterator  iu;
    for ( iu = m_units.begin(); iu != m_units.end(); iu++ ) {
      (*iu)->dumpUnitTree(offset+4);
    }
  } 
}
