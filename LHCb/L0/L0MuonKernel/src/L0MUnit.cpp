#include <utility>  // For std::pair
#include <algorithm> 
#include "L0MuonKernel/L0MUnit.h"
#include "L0MuonKernel/UnitFactory.h"

L0Muon::L0MUnit::L0MUnit():Unit(){
}
L0Muon::L0MUnit::L0MUnit(LHCb::MuonTileID id):Unit(){
  m_mid = id;
}

L0Muon::L0MUnit::~L0MUnit(){
}

L0Muon::L0MUnit::L0MUnit(DOMNode* pNode):Unit() {
  //Get the attributes
  // // //std::cout << "<L0MUnit::L0MUnit> has node any attribute ?"<<pNode->hasAttributes()<<std::endl;
  DOMNamedNodeMap* di = pNode->getAttributes();
  // // //std::cout << "<L0MUnit::L0MUnit> how many ?"<<di->getLength()<<std::endl;
  std::string type = getAttributeStr(di, "type");
  // // //std::cout << "<L0MUnit::L0MUnit>   "<<"type="<<type<<std::endl;
  m_mid = LHCb::MuonTileID(getAttributeStr(di, "id"));
  // // //std::cout << "<L0MUnit::L0MUnit>   "<<"id= "<<m_mid.toString()<<std::endl;

  XMLCh* PROPERTY        = XMLString::transcode(XMLProperty.c_str());
  XMLCh* INPUTREGISTERS  = XMLString::transcode(XMLInputRegisters.c_str());
  XMLCh* OUTPUTREGISTERS = XMLString::transcode(XMLOutputRegisters.c_str());
  XMLCh* UNIT            = XMLString::transcode(XMLUnit.c_str());
  DOMNode *child = pNode->getFirstChild();
  while(child) {
    // // //std::cout << "<L0MUnit::L0MUnit> child ";
    // // //std::cout	<<" name is "<<XMLString::transcode(child->getNodeName()) ;
    // // //std::cout	<<" type is "<<child->getNodeValue();
    // // //std::cout	<<std::endl;
    if(XMLString::equals(child->getNodeName(),PROPERTY)) {
      propertyFromNode(child);
    } else if (XMLString::equals(child->getNodeName(),INPUTREGISTERS)) {
      registersFromNode(child,0);
    }else if (XMLString::equals(child->getNodeName(),OUTPUTREGISTERS)) {
      registersFromNode(child,1);
    }else if (XMLString::equals(child->getNodeName(),UNIT)) {
      unitFromNode(child);
    }
    // // //std::cout << "<L0MUnit::L0MUnit> next child ..."<<std::endl;
    child = child->getNextSibling();
  }
  XMLString::release(&PROPERTY       ); //Release memory used by the transcode method
  XMLString::release(&INPUTREGISTERS ); //Release memory used by the transcode method
  XMLString::release(&OUTPUTREGISTERS); //Release memory used by the transcode method
  XMLString::release(&UNIT           ); //Release memory used by the transcode method

}

void L0Muon::L0MUnit::propertyFromNode(DOMNode* pNode)
{
  //Get the attributes
  DOMNamedNodeMap* di = pNode->getAttributes();
  std::string name = getAttributeStr(di, "name");
  std::string value = getAttributeStr(di, "value");
  setProperty(name,value);
  // // //std::cout << "L0MUnit::propertyFromNode "<< " name: " <<name <<" value: " <<value<< std::endl;
  
}

void L0Muon::L0MUnit::registersFromNode(DOMNode* pNode,int io)
{
  
  //Get the attributes
  // // //std::cout << "<L0MUnit::registersFromNode> has node attributes ? "<<pNode->hasAttributes()<<std::endl;
  DOMNamedNodeMap* di = pNode->getAttributes();
  int size = getAttributeInt(di, "size");
  // // //std::cout << "<L0MUnit::registersFromNode> size= "<<size<<std::endl;
  if (size==0) return;
  L0Muon::RegisterFactory* rfactory = L0Muon::RegisterFactory::instance();
  DOMNode *child = pNode->getFirstChild();
  while(child) {
    // // //std::cout << "<L0MUnit::registersFromNode> child ";
    // // //std::cout <<" name is "<<XMLString::transcode(child->getNodeName()) ;
    // // //std::cout <<" type is "<<child->getNodeValue();
    // // //std::cout <<std::endl;
    if (child->getNodeType()!=3) {
      DOMNamedNodeMap* di = child->getAttributes();
      std::string name = getAttributeStr(di, "name");
      std::string key = getAttributeStr(di, "key");
      Register* preg = rfactory->searchRegister(name);
      // // //std::cout << "<L0MUnit::registersFromNode> name="<<name<<std::endl;
      // // //std::cout << "<L0MUnit::registersFromNode> key="<<key<<std::endl;
      // // //std::cout << "<L0MUnit::registersFromNode> preg="<<preg<<std::endl;
      // // //std::cout << "<L0MUnit::registersFromNode> preg->size()="<<preg->size()<<std::endl;
      switch(io){
      case 0:
        // // //std::cout << "<L0MUnit::registersFromNode> INPUT ..."<<std::endl;
        addInputRegister(preg,key);
        // // //std::cout << "<L0MUnit::registersFromNode> INPUT done"<<std::endl;
        break;
      case 1:
        // // //std::cout << "<L0MUnit::registersFromNode> OUTPUT ..."<<std::endl;
        addOutputRegister(preg,key);
        // // //std::cout << "<L0MUnit::registersFromNode> OUTPUT done"<<std::endl;
        break;
      }
      // // //std::cout << "<L0MUnit::registersFromNode> next child ..."<<std::endl;
    }
    child = child->getNextSibling();
  }
}

void L0Muon::L0MUnit::unitFromNode(DOMNode* pNode)
{
  //Get the attributes
  DOMNamedNodeMap* di = pNode->getAttributes();
  std::string type = getAttributeStr(di, "type");
  L0Muon::UnitFactory* ufactory = L0Muon::UnitFactory::instance();
  Unit* pu = ufactory->createUnit(pNode,type);
  addUnit(pu);
}

std::string L0Muon::L0MUnit::toXML(std::string tab){

  // // //std::cout << tab << "L0Muon::L0MUnit::toXML  type"<<type()<<"  IN\n";

  std::string xmlString=tab;
  
  char buf[8];
  std::string str;
  xmlString +="<"+XMLUnit+" ";
  xmlString +=" type = \""+type()+"\" ";
  xmlString +=" id = \""+mid().toString()+"\" ";
  xmlString +=" >\n";
  
  // // //std::cout << tab << "L0Muon::L0MUnit::toXML  xmlString="<<xmlString<<"\n";

  // Input Registers
  // // //std::cout << tab << "L0Muon::L0MUnit::toXML  Input Registers size="<<m_inputs.size()<<"\n";
  if (m_inputs.size()>0) {
    xmlString +=tab+"    ";
    xmlString +="<"+XMLInputRegisters+" ";
    sprintf(buf,"%d",m_inputs.size());
    str = buf;
    xmlString +=" size = \""+str+"\" ";
    xmlString +=" >\n";
    std::map<std::string,L0Muon::Register*>::iterator i_inputs;
    // // //std::cout << tab << "L0Muon::L0MUnit::toXML  Loop over Input Registers\n";
    for (i_inputs=m_inputs.begin();i_inputs!=m_inputs.end();i_inputs++){
      // // //std::cout << tab << "L0Muon::L0MUnit::toXML  Registers:"<<XMLRegisterRecord<<"\n";    
      // // //std::cout << tab << "L0Muon::L0MUnit::toXML  key: "<< (*i_inputs).first<<"\n";
      // // //std::cout << tab << "L0Muon::L0MUnit::toXML  name:"<< (*i_inputs).second->name()<<"\n";
      xmlString +=tab+"    "+"    ";
      xmlString +="<"+XMLRegisterRecord+" ";
      xmlString +=" key  = \""+(*i_inputs).first+"\" ";
      xmlString +=" name = \""+(*i_inputs).second->name()+"\" ";
      xmlString +="/>\n";
    }
    xmlString +=tab+"    ";
    xmlString +="</"+XMLInputRegisters+">\n";
  }

  // // //std::cout << tab << "L0Muon::L0MUnit::toXML  Input Registers done.\n";

  // Output Registers
  // // //std::cout << tab << "L0Muon::L0MUnit::toXML Output  Registers size="<<m_outputs.size()<<"\n";
  if (m_outputs.size()>0) {
    xmlString +=tab+"    ";
    xmlString +="<"+XMLOutputRegisters+" ";
    sprintf(buf,"%d",m_outputs.size());
    str = buf;
    xmlString +=" size = \""+str+"\" ";
    xmlString +=" >\n";
    std::map<std::string,L0Muon::Register*>::iterator i_outputs;
    // // //std::cout << tab << "L0Muon::L0MUnit::toXML  Loop over Output Registers\n";
    for (i_outputs=m_outputs.begin();i_outputs!=m_outputs.end();i_outputs++){
      // // //std::cout << tab << "L0Muon::L0MUnit::toXML  Registers:"<<XMLRegisterRecord<<"\n";    
      // // //std::cout << tab << "L0Muon::L0MUnit::toXML  key: "<< (*i_outputs).first<<"\n";
      // // //std::cout << tab << "L0Muon::L0MUnit::toXML  name:"<< (*i_outputs).second->name()<<"\n";
      xmlString +=tab+"    "+"    ";
      xmlString +="<"+XMLRegisterRecord+" ";
      xmlString +=" key  = \""+(*i_outputs).first+"\" ";
      xmlString +=" name = \""+(*i_outputs).second->name()+"\" ";
      xmlString +="/>\n";
    }
    xmlString +=tab+"    ";
    xmlString +="</"+XMLOutputRegisters+">\n";
  }
  // // //std::cout << tab << "L0Muon::L0MUnit::toXML  Output Registers done.\n";
  // SubUnits
  if ( ! m_units.empty() ) {
    std::vector<L0Muon::Unit*>::iterator  iu;
    for ( iu = m_units.begin(); iu != m_units.end(); iu++ ) {
      L0MUnit * pu = dynamic_cast<L0MUnit *>(*iu);
      xmlString += pu->toXML(tab+"    ");
    }
  } 
  // // //std::cout << tab << "L0Muon::L0MUnit::toXML SubUnits  done.\n";
  
  xmlString += tab;
  xmlString +="</"+XMLUnit+">\n";

  return xmlString;  
}

////////////////////////////////////////////////////////////////////////////////
//
//  getAttribute
//
////////////////////////////////////////////////////////////////////////////////

int L0Muon::L0MUnit::getAttributeInt(DOMNamedNodeMap* di, const char* key){
  
  const XMLCh* valX;
  int          valI;
  
  XMLCh* xmlKey = XMLString::transcode(key);
  
  valX = di->getNamedItem(xmlKey)->getNodeValue();
  valI = XMLString::parseInt(valX);
  
  XMLString::release(&xmlKey); //Release memory used by the transcode method
  return valI;

}

std::string L0Muon::L0MUnit::getAttributeStr(DOMNamedNodeMap* di, const char* key){

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

