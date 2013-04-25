#include "L0MuonKernel/UnitFactory.h"
#include "L0MuonKernel/MuonTriggerUnit.h"
#include "L0MuonKernel/CtrlUnit.h"
#include "L0MuonKernel/CrateUnit.h"
#include "L0MuonKernel/BoardUnit.h"
#include "L0MuonKernel/BCSUnit.h"
#include "L0MuonKernel/ProcUnit.h"
#include "L0MuonKernel/CoreUnit.h"
#include "L0MuonKernel/FormattingUnit.h"
#include "L0MuonKernel/FormattingOutUnit.h"

L0Muon::UnitFactory* L0Muon::UnitFactory::m_instance=0;

L0Muon::UnitFactory::UnitFactory() : m_topUnit(0) {
}

L0Muon::UnitFactory::~UnitFactory() {
}

L0Muon::UnitFactory* L0Muon::UnitFactory::instance() {
  
//   std::cout <<"L0Muon::UnitFactory::instance m_instance= "<<m_instance<<std::endl;
  
  if ( m_instance == 0 ) {
//     std::cout <<"L0Muon::UnitFactory::instance new instance ! m_instance= "<<m_instance<<std::endl;
    m_instance = new L0Muon::UnitFactory;
  } 
  return m_instance;
}

void L0Muon::UnitFactory::reset() {
  m_instance=0;
}

L0Muon::Unit* L0Muon::UnitFactory::createUnit(DOMNode* pNode, std::string type) {
  
  // // //std::cout << "<UnitFactory::createUnit> has node any attribute ?"<<pNode->hasAttributes()<<std::endl;
  if (type == "MuonTriggerUnit"){
    // // //std::cout <<"<UnitFactory::createUnit> MuonTriggerUnit"<<std::endl;
    MuonTriggerUnit* pu = new MuonTriggerUnit(pNode);
    // // //std::cout <<"<UnitFactory::createUnit> MuonTriggerUnit done"<<std::endl;
    return pu;
  } else if (type == "CtrlUnit"){
    // // //std::cout <<"<UnitFactory::createUnit> CtrlUnit"<<std::endl;
    CtrlUnit* pu = new CtrlUnit(pNode);
    return pu;
  } else if (type == "CrateUnit"){
    // // //std::cout <<"<UnitFactory::createUnit> CrateUnit"<<std::endl;
    CrateUnit* pu = new CrateUnit(pNode);
    // // //std::cout <<"<UnitFactory::createUnit> CrateUnit done"<<std::endl;
    return pu;
  } else if (type == "BoardUnit"){
    // // //std::cout <<"<UnitFactory::createUnit> BoardUnit"<<std::endl;
    BoardUnit* pu = new BoardUnit(pNode);
    return pu;
  } else if (type == "BCSUnit"){
    // // //std::cout <<"<UnitFactory::createUnit> BCSUnit"<<std::endl;
    BCSUnit* pu = new BCSUnit(pNode);
    return pu;
  } else if (type == "ProcUnit"){
    // // //std::cout <<"<UnitFactory::createUnit> ProcUnit"<<std::endl;
    ProcUnit* pu = new ProcUnit(pNode);
    return pu;
  } else if (type == "CoreUnit"){
    // // //std::cout <<"<UnitFactory::createUnit> CoreUnit"<<std::endl;
    CoreUnit* pu = new CoreUnit(pNode);
    return pu;
  } else if (type == "FormattingUnit"){
    // // //std::cout <<"<UnitFactory::createUnit> FormattingUnit"<<std::endl;
    FormattingUnit* pu = new FormattingUnit(pNode);
    return pu;
  } else if (type == "FormattingOutUnit"){
    // // //std::cout <<"<UnitFactory::createUnit> FormattingOutUnit"<<std::endl;
    FormattingOutUnit* pu = new FormattingOutUnit(pNode);
    return pu;
  } else {
    std::cout <<"<L0Muon::UnitFactory::createUnit> ERROR: UNKNOWN UNIT TYPE "<<type<<"\n";
    return 0;
  }
  
}

// L0Muon::Unit * L0Muon::UnitFactory::fromXML(DOMNode* pNode){
void L0Muon::UnitFactory::fromXML(DOMNode* pNode){
  
//   L0Muon::Unit * top=0;
  m_topUnit  =0;
  DOMNode *child = pNode->getFirstChild();
  while(child){
    if (child->getNodeType()==3) {
      child = child->getNextSibling();
      continue;
    }
    DOMNamedNodeMap* di = child->getAttributes();
    std::string type = getAttributeStr(di, "type");
//     top = createUnit(child,type);
    m_topUnit = createUnit(child,type);
    break;
  }
//   return top;
  
}
////////////////////////////////////////////////////////////////////////////////
//
//  getAttribute
//
////////////////////////////////////////////////////////////////////////////////

int L0Muon::UnitFactory::getAttributeInt(DOMNamedNodeMap* di, const char* key){
  
  const XMLCh* valX;
  int          valI;
  
  XMLCh* xmlKey = XMLString::transcode(key);
  
  valX = di->getNamedItem(xmlKey)->getNodeValue();
  valI = XMLString::parseInt(valX);
  
  XMLString::release(&xmlKey); //Release memory used by the transcode method
  return valI;

}

std::string L0Muon::UnitFactory::getAttributeStr(DOMNamedNodeMap* di, const char* key){

  const XMLCh* valX;
  char*  valC;
  std::string  valS;
  
  XMLCh* xmlKey = XMLString::transcode(key);
  
  valX = di->getNamedItem(xmlKey)->getNodeValue();
  valC = XMLString::transcode(valX);
  valS = valC;
  
  XMLString::release(&xmlKey); //Release memory used by the transcode method
  XMLString::release(&valC);   //Release memory used by the transcode method
  return valS;
  
}
